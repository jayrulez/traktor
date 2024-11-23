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
#include "TurboBadgerUi/TurboBadgerUiViewResource.h"
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
#include "Ui/Enums.h"

#include "tb_widgets.h"

namespace
{
	using namespace traktor;

	int32_t getKey(ui::VirtualKey vk)
	{
		return (int32_t)vk;
	}

	tb::SPECIAL_KEY getSpecialKey(ui::VirtualKey vk)
	{
		switch (vk) {
		case ui::VirtualKey::VkNull: return tb::TB_KEY_UNDEFINED;
		case ui::VirtualKey::VkUp: return tb::TB_KEY_UP;
		case ui::VirtualKey::VkDown: return tb::TB_KEY_DOWN;
		case ui::VirtualKey::VkLeft: return tb::TB_KEY_LEFT;
		case ui::VirtualKey::VkRight: return tb::TB_KEY_RIGHT;
		case ui::VirtualKey::VkPageUp: return tb::TB_KEY_PAGE_UP;
		case ui::VirtualKey::VkPageDown: return tb::TB_KEY_PAGE_DOWN;
		case ui::VirtualKey::VkHome: return tb::TB_KEY_HOME;
		case ui::VirtualKey::VkEnd: return tb::TB_KEY_END;
		case ui::VirtualKey::VkTab: return tb::TB_KEY_TAB;
		case ui::VirtualKey::VkBackSpace: return tb::TB_KEY_BACKSPACE;
		case ui::VirtualKey::VkInsert: return tb::TB_KEY_INSERT;
		case ui::VirtualKey::VkDelete: return tb::TB_KEY_DELETE;
		case ui::VirtualKey::VkReturn: return tb::TB_KEY_ENTER;
		case ui::VirtualKey::VkEscape: return tb::TB_KEY_ESC;
		case ui::VirtualKey::VkF1: return tb::TB_KEY_F1;
		case ui::VirtualKey::VkF2: return tb::TB_KEY_F2;
		case ui::VirtualKey::VkF3: return tb::TB_KEY_F3;
		case ui::VirtualKey::VkF4: return tb::TB_KEY_F4;
		case ui::VirtualKey::VkF5: return tb::TB_KEY_F5;
		case ui::VirtualKey::VkF6: return tb::TB_KEY_F6;
		case ui::VirtualKey::VkF7: return tb::TB_KEY_F7;
		case ui::VirtualKey::VkF8: return tb::TB_KEY_F8;
		case ui::VirtualKey::VkF9: return tb::TB_KEY_F9;
		case ui::VirtualKey::VkF10: return tb::TB_KEY_F10;
		case ui::VirtualKey::VkF11: return tb::TB_KEY_F11;
		case ui::VirtualKey::VkF12: return tb::TB_KEY_F12;
		default: return tb::TB_KEY_UNDEFINED;
		}
	}

	tb::MODIFIER_KEYS getModifierKeys(int32_t keyState)
	{
		const bool ctrlPressed = (keyState & ui::KsControl) != 0;
		const bool shiftPressed = (keyState & ui::KsShift) != 0;
		const bool altPressed = (keyState & ui::KsCommand) != 0;

		tb::MODIFIER_KEYS modifierKeys = tb::MODIFIER_KEYS::TB_MODIFIER_NONE;

		modifierKeys |= ctrlPressed ? tb::MODIFIER_KEYS::TB_CTRL : tb::MODIFIER_KEYS::TB_MODIFIER_NONE;
		modifierKeys |= shiftPressed ? tb::MODIFIER_KEYS::TB_SHIFT : tb::MODIFIER_KEYS::TB_MODIFIER_NONE;
		modifierKeys |= altPressed ? tb::MODIFIER_KEYS::TB_ALT : tb::MODIFIER_KEYS::TB_MODIFIER_NONE;

		return modifierKeys;
	}
}

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

	bool TurboBadgerUiPreviewControl::create(ui::Widget* parent, TurboBadgerUiViewResource* uiViewResource)
	{
		m_uiViewResource = uiViewResource;

		if (!m_uiViewResource)
		{
			return false;
		}

		if (!TurboBadgerUi::getInstance().isInitialized())
		{
			log::error << L"TurboBadgerUi is not initialized." << Endl;
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

		// Create TurboBadgerUi renderer.
		m_renderer = new turbobadgerui::TurboBadgerUiRenderer();
		if (!m_renderer->create(
			m_resourceManager,
			m_renderSystem,
			1
		))
			return false;

		m_view = TurboBadgerUi::getInstance().createView(
			m_uiViewResource->getFilePath(), 
			m_uiViewResource->getWidth(), 
			m_uiViewResource->getHeight());

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

		if (m_view)
		{
			TurboBadgerUi::getInstance().destroyView(m_view);
			m_view = nullptr;
		}

		delete m_renderer;
		delete m_renderContext;
		safeDestroy(m_renderGraph);
		safeClose(m_renderView);

		Widget::destroy();
	}

	ui::Size TurboBadgerUiPreviewControl::getPreferredSize(const ui::Size& hint) const
	{
		if (!m_uiViewResource)
			return ui::Size(600, 600);

		int width = m_uiViewResource->getWidth();
		int height = m_uiViewResource->getHeight();

		return ui::Size(width, height);
	}

	void TurboBadgerUiPreviewControl::setUiViewResource(TurboBadgerUiViewResource* uiViewResource)
	{
		if (m_view)
		{
			// if there is an existing view, then destroy it
			TurboBadgerUi::getInstance().destroyView(m_view);
			m_view = nullptr;
		}

		m_uiViewResource = uiViewResource;

		m_view = TurboBadgerUi::getInstance().createView(
			m_uiViewResource->getFilePath(),
			m_uiViewResource->getWidth(),
			m_uiViewResource->getHeight());
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

		if (m_view)
		{
			// todo: set dpi and resize view
			//m_view->SetSize(sz.cx, sz.cy); // not sure we should do this, let's see when it starts working

			TurboBadgerUi::getInstance().resizeView(m_view, sz.cx, sz.cy);
		}
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
				TurboBadgerUi::getInstance().reloadRenderResources();
			}
		}

		// Add passes to render graph.
		m_renderer->beginSetup(m_renderGraph);
		m_renderer->renderView(m_view, sz.cx, sz.cy);
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
		if (!m_view)
			return;

		TurboBadgerUi::getInstance().update();

		TurboBadgerUi::getInstance().updateView(m_view);

		update();

		event->requestMore();
	}

	void TurboBadgerUiPreviewControl::eventKey(ui::KeyEvent* event)
	{
		if (!m_view)
			return;

		int32_t key = getKey(event->getVirtualKey());

		tb::SPECIAL_KEY specialKey = getSpecialKey(event->getVirtualKey());

		//m_view->InvokeKey(key, specialKey, getModifierKeys(event->getKeyState()), true);
		//m_view->InvokeKey(key, specialKey, getModifierKeys(event->getKeyState()), false);
	}

	void TurboBadgerUiPreviewControl::eventKeyDown(ui::KeyDownEvent* event)
	{
		if (!m_view)
			return;

		int32_t key = getKey(event->getVirtualKey());

		tb::SPECIAL_KEY specialKey = getSpecialKey(event->getVirtualKey());

		//m_view->InvokeKey(key, specialKey, getModifierKeys(event->getKeyState()), true);
	}

	void TurboBadgerUiPreviewControl::eventKeyUp(ui::KeyUpEvent* event)
	{
		if (!m_view)
			return;

		int32_t key = getKey(event->getVirtualKey());

		tb::SPECIAL_KEY specialKey = getSpecialKey(event->getVirtualKey());

		//m_view->InvokeKey(key, specialKey, getModifierKeys(event->getKeyState()), false);
	}

	void TurboBadgerUiPreviewControl::eventButtonDown(ui::MouseButtonDownEvent* event)
	{
		if (!m_view)
			return;

		//m_view->InvokePointerDown(event->getPosition().x, event->getPosition().y, 1, getModifierKeys(event->getKeyState()), false);
	}

	void TurboBadgerUiPreviewControl::eventButtonUp(ui::MouseButtonUpEvent* event)
	{
		if (!m_view)
			return;

		//m_view->InvokePointerUp(event->getPosition().x, event->getPosition().y, getModifierKeys(event->getKeyState()), false);
	}

	void TurboBadgerUiPreviewControl::eventMouseMove(ui::MouseMoveEvent* event)
	{
		if (!m_view)
			return;

		//m_view->InvokePointerMove(event->getPosition().x, event->getPosition().y, getModifierKeys(event->getKeyState()), false);
	}

	void TurboBadgerUiPreviewControl::eventMouseWheel(ui::MouseWheelEvent* event)
	{
		if (!m_view)
			return;

		//m_view->InvokeWheel(event->getPosition().x, event->getPosition().y, 0, event->getRotation(), getModifierKeys(event->getKeyState()));
	}
}