/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Paper/Editor/BitmapFontPreviewControl.h"

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
#include "Paper/BitmapFont/BitmapFont.h"
#include "Paper/BitmapFont/BitmapFontRenderer.h"
#include "Render/Context/RenderContext.h"
#include "Render/Frame/RenderGraph.h"
#include "Render/IRenderSystem.h"
#include "Render/IRenderView.h"
#include "Render/PrimitiveRenderer.h"
#include "Render/Resource/ShaderFactory.h"
#include "Render/Resource/TextureFactory.h"
#include "Resource/ResourceManager.h"
#include "Ui/Application.h"
#include "Ui/Itf/IWidget.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.paper.BitmapFontPreviewControl", BitmapFontPreviewControl, ui::Widget)

BitmapFontPreviewControl::BitmapFontPreviewControl(editor::IEditor* editor)
:	m_editor(editor)
,	m_previewText(L"The quick brown fox jumps over the lazy dog.\nABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789")
{
}

bool BitmapFontPreviewControl::create(ui::Widget* parent)
{
	if (!ui::Widget::create(parent, ui::WsNoCanvas))
		return false;

	Ref< db::Database > database = m_editor->getOutputDatabase();
	if (!database)
		return false;

	Ref< ObjectStore > objectStore = m_editor->getObjectStore();
	m_renderSystem = objectStore->get< render::IRenderSystem >();
	if (!m_renderSystem)
		return false;

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

	// Create resource manager.
	m_resourceManager = new resource::ResourceManager(database, settings->getProperty< bool >(L"Resource.Verbose", false));
	m_resourceManager->addFactory(new render::ShaderFactory(m_renderSystem));
	m_resourceManager->addFactory(new render::TextureFactory(m_renderSystem, 0));

	// Create primitive renderer.
	m_primitiveRenderer = new render::PrimitiveRenderer();
	if (!m_primitiveRenderer->create(m_resourceManager, m_renderSystem, 1))
		return false;

	// Create font renderer.
	m_fontRenderer = new BitmapFontRenderer(m_resourceManager, m_renderSystem, m_primitiveRenderer);

	// Add widget event handlers.
	addEventHandler< ui::SizeEvent >(this, &BitmapFontPreviewControl::eventSize);
	addEventHandler< ui::PaintEvent >(this, &BitmapFontPreviewControl::eventPaint);

	// Register idle event handler.
	m_idleEventHandler = ui::Application::getInstance()->addEventHandler< ui::IdleEvent >(this, &BitmapFontPreviewControl::eventIdle);

	m_timer.reset();
	return true;
}

void BitmapFontPreviewControl::destroy()
{
	ui::Application::getInstance()->removeEventHandler(m_idleEventHandler);

	safeDestroy(m_primitiveRenderer);
	safeDestroy(m_renderGraph);
	safeClose(m_renderView);
	safeDestroy(m_resourceManager);

	m_fontRenderer = nullptr;
	m_renderSystem = nullptr;

	ui::Widget::destroy();
}

void BitmapFontPreviewControl::setBitmapFont(BitmapFont* font)
{
	m_font = font;
	if (m_font)
	{
		log::info << L"BitmapFontPreviewControl: Font set, " << m_font->getGlyphCount() << L" glyphs, line height: " << m_font->getLineHeight() << Endl;
		log::info << L"BitmapFontPreviewControl: Texture ID: " << m_font->getTextureId().format() << Endl;
	}
	else
		log::warning << L"BitmapFontPreviewControl: Font is null" << Endl;
	update();
}

void BitmapFontPreviewControl::setPreviewText(const std::wstring& text)
{
	m_previewText = text;
	update();
}

void BitmapFontPreviewControl::eventSize(ui::SizeEvent* event)
{
	const ui::Size sz = event->getSize();

	if (m_renderView)
		m_renderView->reset(sz.cx, sz.cy);
}

void BitmapFontPreviewControl::eventPaint(ui::PaintEvent* event)
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

	// Render text if we have a font.
	if (m_font && m_fontRenderer && !m_previewText.empty())
	{
		// Create orthographic projection for 2D rendering (Y increases downward in screen space).
		const Matrix44 projection = orthoLh(0.0f, (float)sz.cx, 0.0f, (float)sz.cy, 0.0f, 1.0f);

		Ref< render::RenderPass > textPass = new render::RenderPass(L"Text");
		textPass->setOutput(render::RGTargetSet::Output, render::TfAll, render::TfAll);
		textPass->addBuild([=, this](const render::RenderGraph&, render::RenderContext* renderContext) {
			m_primitiveRenderer->begin(0, projection);
			m_primitiveRenderer->pushView(Matrix44::identity());
			m_primitiveRenderer->pushDepthState(false, false, false);

			const float x = 20.0f;
			float y = 40.0f;

			log::info << L"BitmapFontPreviewControl: Rendering text, size: " << sz.cx << L"x" << sz.cy << Endl;

			std::wstring currentLine;
			for (wchar_t ch : m_previewText)
			{
				if (ch == L'\n')
				{
					if (!currentLine.empty())
					{
						log::info << L"BitmapFontPreviewControl: Drawing line at (" << x << L", " << y << L"): " << currentLine << Endl;
						m_fontRenderer->drawText(m_font, Vector2(x, y), currentLine, Color4f(1.0f, 1.0f, 1.0f, 1.0f));
						currentLine.clear();
					}
					y += m_font->getLineHeight();
				}
				else
					currentLine += ch;
			}

			// Draw remaining text.
			if (!currentLine.empty())
			{
				log::info << L"BitmapFontPreviewControl: Drawing final line at (" << x << L", " << y << L"): " << currentLine << Endl;
				m_fontRenderer->drawText(m_font, Vector2(x, y), currentLine, Color4f(1.0f, 1.0f, 1.0f, 1.0f));
			}

			m_primitiveRenderer->popDepthState();
			m_primitiveRenderer->popView();
			m_primitiveRenderer->end(0);

			auto rb = renderContext->allocNamed< render::LambdaRenderBlock >(L"Text");
			rb->lambda = [this](render::IRenderView* renderView) {
				m_primitiveRenderer->render(renderView, 0);
			};
			renderContext->draw(rb);
		});
		m_renderGraph->addPass(textPass);
	}
	else
	{
		if (!m_font)
			log::warning << L"BitmapFontPreviewControl: No font set" << Endl;
		if (!m_fontRenderer)
			log::warning << L"BitmapFontPreviewControl: No font renderer" << Endl;
		if (m_previewText.empty())
			log::warning << L"BitmapFontPreviewControl: Preview text is empty" << Endl;
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

void BitmapFontPreviewControl::eventIdle(ui::IdleEvent* event)
{
	if (isVisible(true))
		event->requestMore();
}

}
