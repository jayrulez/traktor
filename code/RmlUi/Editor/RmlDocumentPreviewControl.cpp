/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <cstring>
#include "Core/Log/Log.h"
#include "Core/Math/Const.h"
#include "Core/Misc/SafeDestroy.h"
#include "Core/Settings/PropertyFloat.h"
#include "Core/Settings/PropertyGroup.h"
#include "Core/Settings/PropertyInteger.h"
#include "Editor/IEditor.h"
#include "Render/IRenderSystem.h"
#include "Render/IRenderTargetSet.h"
#include "Render/IRenderView.h"
#include "Render/ScreenRenderer.h"
#include "Render/Shader.h"
#include "Render/Context/RenderContext.h"
#include "Render/VertexElement.h"
#include "Render/Frame/RenderGraph.h"
#include "RmlUi/RmlDocumentResource.h"
#include "RmlUi/RmlUiRenderer.h"
#include "RmlUi/RmlUi.h"
#include "RmlUi/Editor/RmlDocumentPreviewControl.h"
#include "Ui/Itf/IWidget.h"
#include "Ui/Application.h"
#include "Resource/IResourceManager.h"
#include "Render/IProgram.h"

namespace traktor::rmlui
{
	T_IMPLEMENT_RTTI_CLASS(L"traktor.rmlui.RmlDocumentPreviewControl", RmlDocumentPreviewControl, ui::Widget)

		RmlDocumentPreviewControl::RmlDocumentPreviewControl(editor::IEditor* editor,
			db::Database* database,
			resource::IResourceManager* resourceManager,
			render::IRenderSystem* renderSystem)
		: m_editor(editor)
		, m_database(database)
		, m_resourceManager(resourceManager)
		, m_renderSystem(renderSystem)
	{
	}

	bool RmlDocumentPreviewControl::create(ui::Widget* parent)
	{
		if (!RmlUi::getInstance().isInitialized())
		{
			// todo: print error in log
			return false;
		}

		if (!Widget::create(parent, ui::WsFocus | ui::WsNoCanvas))
			return false;

		render::RenderViewEmbeddedDesc desc;
		desc.depthBits = 16;
		desc.stencilBits = 8;
		desc.multiSample = m_editor->getSettings()->getProperty< int32_t >(L"Editor.MultiSample", 4);
		desc.multiSampleShading = m_editor->getSettings()->getProperty< float >(L"Editor.MultiSampleShading", 0.0f);
		desc.waitVBlanks = 1;
		desc.syswin = getIWidget()->getSystemWindow();

		m_renderView = m_renderSystem->createRenderView(desc);
		if (!m_renderView)
			return false;

		m_renderContext = new render::RenderContext(4 * 1024 * 1024);
		m_renderGraph = new render::RenderGraph(m_renderSystem, desc.multiSample);

		// Create RmlUi renderer.
		m_rmlUiRenderer = new rmlui::RmlUiRenderer();
		if (!m_rmlUiRenderer->create(
			m_resourceManager,
			m_renderSystem,
			1,
			true
		))
			return false;

		// todo: get fonts from document
		if (!RmlUi::getInstance().loadFonts({ 
			{"assets/Atop-R99O3.ttf", false},
			{"assets/LatoLatin-Regular.ttf", false},
			{"assets/LatoLatin-Bold.ttf", false},
			{"assets/LatoLatin-BoldItalic.ttf", false},
			{"assets/LatoLatin-Italic.ttf", false},
			{"assets/NotoEmoji-Regular.ttf", true}
			}))
		{
			return false;
		}

		// todo: get name from rml document
		m_rmlContext = RmlUi::getInstance().createContext(L"Test", Vector2i(m_renderView->getWidth(), m_renderView->getHeight()));
		if (!m_rmlContext)
			return false;

		// todo: remove
		// temporary testing
		//Rml::ElementDocument* document = m_rmlContext->LoadDocumentFromMemory(R"()");
		Rml::ElementDocument* document = m_rmlContext->LoadDocument("assets/demo.rml");

		document->Show();


		addEventHandler< ui::SizeEvent >(this, &RmlDocumentPreviewControl::eventSize);
		addEventHandler< ui::PaintEvent >(this, &RmlDocumentPreviewControl::eventPaint);

		// Register our idle event handler.
		m_idleEventHandler = ui::Application::getInstance()->addEventHandler< ui::IdleEvent >(this, &RmlDocumentPreviewControl::eventIdle);

		return true;
	}

	void RmlDocumentPreviewControl::destroy()
	{
		ui::Application::getInstance()->removeEventHandler(m_idleEventHandler);

		m_rmlContext->UnloadAllDocuments();

		RmlUi::getInstance().destroyContext(m_rmlContext);

		m_rmlContext = nullptr;

		safeDestroy(m_rmlUiRenderer);
		//safeDestroy(m_renderContext);
		safeDestroy(m_renderGraph);
		safeClose(m_renderView);

		Widget::destroy();
	}

	void RmlDocumentPreviewControl::setRmlDocument(RmlDocumentResource* rmlDocument)
	{
		m_rmlDocument = rmlDocument;

		const ui::Size sz = getInnerRect().getSize();
	}

	ui::Size RmlDocumentPreviewControl::getPreferredSize(const ui::Size& hint) const
	{
		if (!m_rmlDocument)
			return ui::Size(600, 600);

		//Aabb2 bounds = {};//m_document->getFrameBounds();

		int width = 0;// int(bounds.mx.x / 20.0f);
		int height = 0;// int(bounds.mx.y / 20.0f);

		return ui::Size(width, height);
	}

	void RmlDocumentPreviewControl::eventSize(ui::SizeEvent* event)
	{
		ui::Size sz = event->getSize();

		sz = getInnerRect().getSize();
		const float scale = 1.0f;// std::max(dpi() / 96.0f, 1.0f);

		if (m_renderView)
		{
			m_renderView->reset(sz.cx, sz.cy);
		}

		if (m_rmlContext)
		{
			m_rmlContext->SetDimensions(Rml::Vector2i(sz.cx, sz.cy));
			//m_rmlContext->SetDimensions(Rml::Vector2i((int32_t)(sz.cx / scale), (int32_t)(sz.cy / scale)));
			//m_rmlContext->SetDensityIndependentPixelRatio(scale);
		}
	}

	void RmlDocumentPreviewControl::eventPaint(ui::PaintEvent* event)
	{
		if (!m_renderView)
			return;

		ui::Size sz = getInnerRect().getSize();
		const float scale = std::max(dpi() / 96.0f, 1.0f);

		// Render view events; reset view if it has become lost.
		render::RenderEvent re;
		while (m_renderView->nextEvent(re))
		{
			if (re.type == render::RenderEventType::Lost)
			{
				m_renderView->reset(sz.cx, sz.cy);
			}
		}

		// Add passes to render graph.
		m_rmlUiRenderer->beginSetup(m_renderGraph);
		m_rmlUiRenderer->render(m_rmlContext, sz.cx, sz.cy);
		m_rmlUiRenderer->endSetup();

		// Validate render graph.
		if (!m_renderGraph->validate())
			return;

		// Build render context.
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

	void RmlDocumentPreviewControl::eventIdle(ui::IdleEvent* event)
	{
		if (!m_rmlContext)
			return;

		m_rmlContext->Update();

		event->requestMore();
	}

}