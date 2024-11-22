/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/Editor/TurboBadgerUiPreviewControl.h"
#include "TurboBadgerUi/TurboBadgerUi.h"
#include "TurboBadgerUi/TurboBadgerUiResource.h"
#include "TurboBadgerUi/TurboBadgerUiRenderer.h"

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

#include "Ui/Itf/IWidget.h"
#include "Ui/Application.h"

namespace traktor::turbobadgerui
{
	T_IMPLEMENT_RTTI_CLASS(L"traktor.turbobadgerui.TurboBadgerUiPreviewControl", TurboBadgerUiPreviewControl, ui::Widget)

		TurboBadgerUiPreviewControl::TurboBadgerUiPreviewControl(editor::IEditor* editor,
			db::Database* database,
			resource::IResourceManager* resourceManager,
			render::IRenderSystem* renderSystem)
		: m_editor(editor)
		, m_database(database)
		, m_resourceManager(resourceManager)
		, m_renderSystem(renderSystem)
	{
	}

	bool TurboBadgerUiPreviewControl::create(ui::Widget* parent, TurboBadgerUiResource* uiResource)
	{
		m_uiResource = uiResource;

		if (!m_uiResource)
		{
			return false;
		}

		//if (!TurboBadgerUi::getInstance().isInitialized())
		//{
		//	// todo: print error in log
		//	return false;
		//}

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

		// Create TurboBadgerUi renderer.
		m_renderer = new turbobadgerui::TurboBadgerUiRenderer();
		if (!m_renderer->create(
			m_resourceManager,
			m_renderSystem,
			1
		))
			return false;

		// todo: create view
		//m_view = ? ? ;

		addEventHandler< ui::SizeEvent >(this, &TurboBadgerUiPreviewControl::eventSize);
		addEventHandler< ui::PaintEvent >(this, &TurboBadgerUiPreviewControl::eventPaint);
		addEventHandler< ui::KeyEvent >(this, &TurboBadgerUiPreviewControl::eventKey);
		addEventHandler< ui::KeyDownEvent >(this, &TurboBadgerUiPreviewControl::eventKeyDown);
		addEventHandler< ui::KeyUpEvent >(this, &TurboBadgerUiPreviewControl::eventKeyUp);
		addEventHandler< ui::MouseButtonDownEvent >(this, &TurboBadgerUiPreviewControl::eventButtonDown);
		addEventHandler< ui::MouseButtonUpEvent >(this, &TurboBadgerUiPreviewControl::eventButtonUp);
		addEventHandler< ui::MouseMoveEvent >(this, &TurboBadgerUiPreviewControl::eventMouseMove);
		addEventHandler< ui::MouseWheelEvent >(this, &TurboBadgerUiPreviewControl::eventMouseWheel);

		// Register our idle event handler.
		m_idleEventHandler = ui::Application::getInstance()->addEventHandler< ui::IdleEvent >(this, &TurboBadgerUiPreviewControl::eventIdle);

		return true;
	}

	void TurboBadgerUiPreviewControl::destroy()
	{
		ui::Application::getInstance()->removeEventHandler(m_idleEventHandler);

		// todo: destroy view?//

		safeDestroy(m_renderer);
		delete m_renderContext;
		safeDestroy(m_renderGraph);
		safeClose(m_renderView);

		Widget::destroy();
	}

	ui::Size TurboBadgerUiPreviewControl::getPreferredSize(const ui::Size& hint) const
	{
		if (!m_uiResource)
			return ui::Size(600, 600);

		// todo: get dimensions from resource
		int width = 600;
		int height = 600;

		return ui::Size(width, height);
	}

	void TurboBadgerUiPreviewControl::setUiResource(TurboBadgerUiResource* uiResource)
	{
		m_uiResource = uiResource;
	}

	void TurboBadgerUiPreviewControl::eventSize(ui::SizeEvent* event)
	{
		ui::Size sz = event->getSize();

		sz = getInnerRect().getSize();
		const float scale = std::max(dpi() / 96.0f, 1.0f);

		if (m_renderView)
		{
			m_renderView->reset(sz.cx, sz.cy);
		}

		// todo: set dpi and resize view
	}

	void TurboBadgerUiPreviewControl::eventPaint(ui::PaintEvent* event)
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
		m_renderer->beginSetup(m_renderGraph);
		// todo: render?
		m_renderer->endSetup();

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

	void TurboBadgerUiPreviewControl::eventIdle(ui::IdleEvent* event)
	{
		// todo: update view states and input states

		update();

		event->requestMore();
	}

	void TurboBadgerUiPreviewControl::eventKey(ui::KeyEvent* event)
	{
		// todo: key event
	}

	void TurboBadgerUiPreviewControl::eventKeyDown(ui::KeyDownEvent* event)
	{
		// todo: keydown event
	}

	void TurboBadgerUiPreviewControl::eventKeyUp(ui::KeyUpEvent* event)
	{
		// todo: keyup event
	}

	void TurboBadgerUiPreviewControl::eventButtonDown(ui::MouseButtonDownEvent* event)
	{
		// todo: mouse button down event
	}

	void TurboBadgerUiPreviewControl::eventButtonUp(ui::MouseButtonUpEvent* event)
	{
		// todo: mouse button up event
	}

	void TurboBadgerUiPreviewControl::eventMouseMove(ui::MouseMoveEvent* event)
	{
		// todo: mouse move event
	}

	void TurboBadgerUiPreviewControl::eventMouseWheel(ui::MouseWheelEvent* event)
	{
		// todo: mouse wheel event
	}
}