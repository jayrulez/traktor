/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Paper/Editor/UIPagePreviewControl.h"

#include "Core/Log/Log.h"
#include "Core/Math/Color4f.h"
#include "Core/Math/Matrix44.h"
#include "Core/Misc/ObjectStore.h"
#include "Core/Misc/SafeDestroy.h"
#include "Core/Settings/PropertyBoolean.h"
#include "Core/Settings/PropertyFloat.h"
#include "Core/Settings/PropertyGroup.h"
#include "Core/Settings/PropertyInteger.h"
#include "Database/Database.h"
#include "Editor/IEditor.h"
#include "Paper/BitmapFont/BitmapFontRenderer.h"
#include "Paper/Draw2D.h"
#include "Paper/FontManager.h"
#include "Paper/Ui/UIElement.h"
#include "Paper/Ui/UIPage.h"
#include "Paper/Ui/UIContext.h"
#include "Render/Context/RenderContext.h"
#include "Render/Frame/RenderGraph.h"
#include "Render/IRenderSystem.h"
#include "Render/IRenderView.h"
#include "Render/Resource/ShaderFactory.h"
#include "Render/Resource/TextureFactory.h"
#include "Resource/ResourceManager.h"
#include "Ui/Application.h"
#include "Ui/Itf/IWidget.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.paper.UIPagePreviewControl", UIPagePreviewControl, ui::Widget)

UIPagePreviewControl::UIPagePreviewControl(editor::IEditor* editor,
	db::Database* database,
	resource::IResourceManager* resourceManager,
	render::IRenderSystem* renderSystem)
	: m_editor(editor)
	, m_database(database)
	, m_resourceManager(resourceManager)
	, m_renderSystem(renderSystem)
{
}

bool UIPagePreviewControl::create(ui::Widget* parent)
{
	if (!ui::Widget::create(parent, ui::WsNoCanvas))
		return false;

	Ref< ObjectStore > objectStore = m_editor->getObjectStore();

	// Create render view.
	Ref< const PropertyGroup > settings = m_editor->getSettings();
	render::RenderViewEmbeddedDesc desc;
	desc.depthBits = 16;
	desc.stencilBits = 8;
	desc.multiSample = settings->getProperty< int32_t >(L"Editor.MultiSample", 4);
	desc.multiSampleShading = settings->getProperty< float >(L"Editor.MultiSampleShading", 0.0f);
	desc.allowHDR = settings->getProperty< bool >(L"Editor.HDR", true);
	desc.waitVBlanks = 1;
	desc.syswin = getIWidget()->getSystemWindow();

	m_renderView = m_renderSystem->createRenderView(desc);
	if (!m_renderView)
		return false;

	m_renderContext = new render::RenderContext(4 * 1024 * 1024);
	m_renderGraph = new render::RenderGraph(m_renderSystem, desc.multiSample);

	// Create Draw2D renderer.
	m_renderer = new Draw2D();
	if (!m_renderer->create(m_resourceManager, m_renderSystem, 1))
		return false;

	// Create FontManager and BitmapFontRenderer.
	m_fontManager = new FontManager(m_resourceManager);
	m_fontRenderer = new BitmapFontRenderer(m_resourceManager, m_renderSystem, m_renderer);

	// Add widget event handlers.
	addEventHandler< ui::SizeEvent >(this, &UIPagePreviewControl::eventSize);
	addEventHandler< ui::PaintEvent >(this, &UIPagePreviewControl::eventPaint);

	// Register idle event handler.
	m_idleEventHandler = ui::Application::getInstance()->addEventHandler< ui::IdleEvent >(this, &UIPagePreviewControl::eventIdle);

	m_timer.reset();
	return true;
}

void UIPagePreviewControl::destroy()
{
	ui::Application::getInstance()->removeEventHandler(m_idleEventHandler);

	safeDestroy(m_renderer);
	safeDestroy(m_renderGraph);
	safeClose(m_renderView);
	safeDestroy(m_resourceManager);

	m_renderSystem = nullptr;

	ui::Widget::destroy();
}

void UIPagePreviewControl::setUIPage(UIPage* uiPage)
{
	m_uiPage = uiPage;
	if (m_uiPage)
	{
		log::info << L"UIPagePreviewControl: UIPage set" << Endl;
		if (m_uiPage->getRoot())
			log::info << L"UIPagePreviewControl: Has root element" << Endl;
		else
			log::info << L"UIPagePreviewControl: No root element" << Endl;
	}
	else
		log::warning << L"UIPagePreviewControl: UIPage is null" << Endl;
	update();
}

void UIPagePreviewControl::eventSize(ui::SizeEvent* event)
{
	const ui::Size sz = event->getSize();

	if (m_renderView)
		m_renderView->reset(sz.cx, sz.cy);
}

void UIPagePreviewControl::eventPaint(ui::PaintEvent* event)
{
	if (!m_renderView)
		return;

	const ui::Size sz = getInnerRect().getSize();

	// Render view events; reset view if it has become lost.
	render::RenderEvent re;
	while (m_renderView->nextEvent(re))
		if (re.type == render::RenderEventType::Lost)
			m_renderView->reset(sz.cx, sz.cy);

	// Setup clear render pass.
	render::Clear clear;
	clear.mask = render::CfColor | render::CfDepth | render::CfStencil;
	clear.colors[0] = Color4f(0.2f, 0.2f, 0.2f, 1.0f);
	clear.depth = 1.0f;
	clear.stencil = 0;

	Ref< render::RenderPass > clearPass = new render::RenderPass(L"Clear");
	clearPass->setOutput(render::RGTargetSet::Output, clear, render::TfAll, render::TfAll);
	m_renderGraph->addPass(clearPass);

	// Render UI if we have a UIPage.
	if (m_uiPage && m_uiPage->getRoot())
	{
		// Use UIPage dimensions for layout instead of window size
		const float pageWidth = (float)m_uiPage->getWidth();
		const float pageHeight = (float)m_uiPage->getHeight();

		// Create orthographic projection for 2D rendering (Y increases downward in screen space).
		const Matrix44 projection = orthoLh(0.0f, 0.0f, (float)sz.cx, -(float)sz.cy, -1.0f, 1.0f);

		Ref< render::RenderPass > uiPass = new render::RenderPass(L"UI");
		uiPass->setOutput(render::RGTargetSet::Output, render::TfAll, render::TfAll);
		uiPass->addBuild([=, this](const render::RenderGraph&, render::RenderContext* renderContext) {
			m_renderer->begin(0, projection);

			// Create UI context
			UIContext uiContext(m_renderer, m_fontManager, m_fontRenderer);

			// Measure and arrange UI using UIPage dimensions
			auto root = m_uiPage->getRoot();
			Vector2 availableSize(pageWidth, pageHeight);
			root->measure(availableSize, &uiContext);
			root->arrange(Vector2::zero(), availableSize);

			// Render UI with debug visualization
			root->renderDebug(&uiContext);

			m_renderer->end(0);

			auto rb = renderContext->allocNamed< render::LambdaRenderBlock >(L"UI");
			rb->lambda = [this](render::IRenderView* renderView) {
				m_renderer->render(renderView, 0);
			};
			renderContext->draw(rb);
		});
		m_renderGraph->addPass(uiPass);
	}
	else
	{
		if (!m_uiPage)
			log::warning << L"UIPagePreviewControl: No UIPage set" << Endl;
		else if (!m_uiPage->getRoot())
			log::warning << L"UIPagePreviewControl: UIPage has no root element" << Endl;
	}

	// Validate and build render graph.
	if (!m_renderGraph->validate())
		return;

	m_renderContext->flush();
	m_renderGraph->build(m_renderContext, sz.cx, sz.cy);

	// Render frame.
	if (m_renderView->beginFrame())
	{
		m_renderContext->render(m_renderView);
		m_renderView->endFrame();
		m_renderView->present();
	}

	event->consume();
}

void UIPagePreviewControl::eventIdle(ui::IdleEvent* event)
{
	if (isVisible(true))
		event->requestMore();
}

}
