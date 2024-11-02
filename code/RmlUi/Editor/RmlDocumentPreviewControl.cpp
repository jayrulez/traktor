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
	namespace
	{
		static const std::map<ui::VirtualKey, Rml::Input::KeyIdentifier> s_uiToRmlUiKeyMap = {
				{ui::VkNull, Rml::Input::KeyIdentifier::KI_UNKNOWN},

				{ui::VkTab, Rml::Input::KeyIdentifier::KI_TAB},
				{ui::VkClear, Rml::Input::KeyIdentifier::KI_CLEAR},
				{ui::VkReturn, Rml::Input::KeyIdentifier::KI_RETURN},
				{ui::VkShift, Rml::Input::KeyIdentifier::KI_LSHIFT},
				{ui::VkControl, Rml::Input::KeyIdentifier::KI_LCONTROL},
				{ui::VkMenu, Rml::Input::KeyIdentifier::KI_LMENU},
				{ui::VkPause, Rml::Input::KeyIdentifier::KI_PAUSE},
				{ui::VkCapital, Rml::Input::KeyIdentifier::KI_CAPITAL},
				{ui::VkEscape, Rml::Input::KeyIdentifier::KI_ESCAPE},
				{ui::VkSpace, Rml::Input::KeyIdentifier::KI_SPACE},
				{ui::VkEnd, Rml::Input::KeyIdentifier::KI_END},
				{ui::VkHome, Rml::Input::KeyIdentifier::KI_HOME},
				{ui::VkPageUp, Rml::Input::KeyIdentifier::KI_PRIOR},
				{ui::VkPageDown, Rml::Input::KeyIdentifier::KI_NEXT},
				{ui::VkLeft, Rml::Input::KeyIdentifier::KI_LEFT},
				{ui::VkUp, Rml::Input::KeyIdentifier::KI_UP},
				{ui::VkRight, Rml::Input::KeyIdentifier::KI_RIGHT},
				{ui::VkDown, Rml::Input::KeyIdentifier::KI_DOWN},
				{ui::VkSelect, Rml::Input::KeyIdentifier::KI_SELECT},
				{ui::VkPrint, Rml::Input::KeyIdentifier::KI_PRINT},
				{ui::VkExecute, Rml::Input::KeyIdentifier::KI_EXECUTE},
				{ui::VkSnapshot, Rml::Input::KeyIdentifier::KI_SNAPSHOT},
				{ui::VkInsert, Rml::Input::KeyIdentifier::KI_INSERT},
				{ui::VkDelete, Rml::Input::KeyIdentifier::KI_DELETE},
				{ui::VkBackSpace, Rml::Input::KeyIdentifier::KI_BACK},
				{ui::VkHelp, Rml::Input::KeyIdentifier::KI_HELP},

				{ui::VkNumPad0, Rml::Input::KeyIdentifier::KI_NUMPAD0},
				{ui::VkNumPad1, Rml::Input::KeyIdentifier::KI_NUMPAD1},
				{ui::VkNumPad2, Rml::Input::KeyIdentifier::KI_NUMPAD2},
				{ui::VkNumPad3, Rml::Input::KeyIdentifier::KI_NUMPAD3},
				{ui::VkNumPad4, Rml::Input::KeyIdentifier::KI_NUMPAD4},
				{ui::VkNumPad5, Rml::Input::KeyIdentifier::KI_NUMPAD5},
				{ui::VkNumPad6, Rml::Input::KeyIdentifier::KI_NUMPAD6},
				{ui::VkNumPad7, Rml::Input::KeyIdentifier::KI_NUMPAD7},
				{ui::VkNumPad8, Rml::Input::KeyIdentifier::KI_NUMPAD8},
				{ui::VkNumPad9, Rml::Input::KeyIdentifier::KI_NUMPAD9},

				{ui::VkMultiply, Rml::Input::KeyIdentifier::KI_MULTIPLY},
				{ui::VkAdd, Rml::Input::KeyIdentifier::KI_ADD},
				{ui::VkSeparator, Rml::Input::KeyIdentifier::KI_SEPARATOR},
				{ui::VkSubtract, Rml::Input::KeyIdentifier::KI_SUBTRACT},
				{ui::VkDecimal, Rml::Input::KeyIdentifier::KI_DECIMAL},
				{ui::VkDivide, Rml::Input::KeyIdentifier::KI_DIVIDE},

				{ui::VkF1, Rml::Input::KeyIdentifier::KI_F1},
				{ui::VkF2, Rml::Input::KeyIdentifier::KI_F2},
				{ui::VkF3, Rml::Input::KeyIdentifier::KI_F3},
				{ui::VkF4, Rml::Input::KeyIdentifier::KI_F4},
				{ui::VkF5, Rml::Input::KeyIdentifier::KI_F5},
				{ui::VkF6, Rml::Input::KeyIdentifier::KI_F6},
				{ui::VkF7, Rml::Input::KeyIdentifier::KI_F7},
				{ui::VkF8, Rml::Input::KeyIdentifier::KI_F8},
				{ui::VkF9, Rml::Input::KeyIdentifier::KI_F9},
				{ui::VkF10, Rml::Input::KeyIdentifier::KI_F10},
				{ui::VkF11, Rml::Input::KeyIdentifier::KI_F11},
				{ui::VkF12, Rml::Input::KeyIdentifier::KI_F12},

				{ui::VkNumLock, Rml::Input::KeyIdentifier::KI_NUMLOCK},
				{ui::VkScroll, Rml::Input::KeyIdentifier::KI_SCROLL},

				{ui::VkComma, Rml::Input::KeyIdentifier::KI_OEM_COMMA},
				{ui::VkPeriod, Rml::Input::KeyIdentifier::KI_OEM_PERIOD},

				{ui::Vk0, Rml::Input::KeyIdentifier::KI_0},
				{ui::Vk1, Rml::Input::KeyIdentifier::KI_1},
				{ui::Vk2, Rml::Input::KeyIdentifier::KI_2},
				{ui::Vk3, Rml::Input::KeyIdentifier::KI_3},
				{ui::Vk4, Rml::Input::KeyIdentifier::KI_4},
				{ui::Vk5, Rml::Input::KeyIdentifier::KI_5},
				{ui::Vk6, Rml::Input::KeyIdentifier::KI_6},
				{ui::Vk7, Rml::Input::KeyIdentifier::KI_7},
				{ui::Vk8, Rml::Input::KeyIdentifier::KI_8},
				{ui::Vk9, Rml::Input::KeyIdentifier::KI_9},

				{ui::VkA, Rml::Input::KeyIdentifier::KI_A},
				{ui::VkB, Rml::Input::KeyIdentifier::KI_B},
				{ui::VkC, Rml::Input::KeyIdentifier::KI_C},
				{ui::VkD, Rml::Input::KeyIdentifier::KI_D},
				{ui::VkE, Rml::Input::KeyIdentifier::KI_E},
				{ui::VkF, Rml::Input::KeyIdentifier::KI_F},
				{ui::VkG, Rml::Input::KeyIdentifier::KI_G},
				{ui::VkH, Rml::Input::KeyIdentifier::KI_H},
				{ui::VkI, Rml::Input::KeyIdentifier::KI_I},
				{ui::VkJ, Rml::Input::KeyIdentifier::KI_J},
				{ui::VkK, Rml::Input::KeyIdentifier::KI_K},
				{ui::VkL, Rml::Input::KeyIdentifier::KI_L},
				{ui::VkM, Rml::Input::KeyIdentifier::KI_M},
				{ui::VkN, Rml::Input::KeyIdentifier::KI_N},
				{ui::VkO, Rml::Input::KeyIdentifier::KI_O},
				{ui::VkP, Rml::Input::KeyIdentifier::KI_P},
				{ui::VkQ, Rml::Input::KeyIdentifier::KI_Q},
				{ui::VkR, Rml::Input::KeyIdentifier::KI_R},
				{ui::VkS, Rml::Input::KeyIdentifier::KI_S},
				{ui::VkT, Rml::Input::KeyIdentifier::KI_T},
				{ui::VkU, Rml::Input::KeyIdentifier::KI_U},
				{ui::VkV, Rml::Input::KeyIdentifier::KI_V},
				{ui::VkW, Rml::Input::KeyIdentifier::KI_W},
				{ui::VkX, Rml::Input::KeyIdentifier::KI_X},
				{ui::VkY, Rml::Input::KeyIdentifier::KI_Y},
				{ui::VkZ, Rml::Input::KeyIdentifier::KI_Z}
		};

		Rml::Input::KeyIdentifier getKey(ui::VirtualKey vk)
		{
			const auto& entry = s_uiToRmlUiKeyMap.find(vk);
			if (entry != s_uiToRmlUiKeyMap.end())
			{
				return entry->second;
			}

			return Rml::Input::KeyIdentifier::KI_UNKNOWN;
		}

		int32_t getModifierState(int32_t keyState)
		{
			const bool ctrlPressed = (keyState & ui::KsControl) != 0;
			const bool shiftPressed = (keyState & ui::KsShift) != 0;
			const bool altPressed = (keyState & ui::KsCommand) != 0;

			int modifierState = 0;

			modifierState |= ctrlPressed ? Rml::Input::KM_CTRL : 0;
			modifierState |= shiftPressed ? Rml::Input::KM_SHIFT : 0;
			modifierState |= altPressed ? Rml::Input::KM_ALT : 0;

			return modifierState;
		}

		// 0 for the left button, 1 for right, and 2 for middle button
		int32_t getMouseButtonIndex(int32_t button)
		{
			switch (button)
			{
			case ui::MbtLeft:
				return 0;

			case ui::MbtMiddle:
				return 2;

			case ui::MbtRight:
				return 1;

			case ui::MbtNone:
			default:
				return -1;
			}
		}
	}



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

		// todo: get name from rml document
		m_rmlContext = RmlUi::getInstance().createContext(L"Test", Vector2i(m_renderView->getWidth(), m_renderView->getHeight()));
		if (!m_rmlContext)
			return false;

		m_rmlContext->EnableMouseCursor(true);

		addEventHandler< ui::SizeEvent >(this, &RmlDocumentPreviewControl::eventSize);
		addEventHandler< ui::PaintEvent >(this, &RmlDocumentPreviewControl::eventPaint);
		addEventHandler< ui::KeyEvent >(this, &RmlDocumentPreviewControl::eventKey);
		addEventHandler< ui::KeyDownEvent >(this, &RmlDocumentPreviewControl::eventKeyDown);
		addEventHandler< ui::KeyUpEvent >(this, &RmlDocumentPreviewControl::eventKeyUp);
		addEventHandler< ui::MouseButtonDownEvent >(this, &RmlDocumentPreviewControl::eventButtonDown);
		addEventHandler< ui::MouseButtonUpEvent >(this, &RmlDocumentPreviewControl::eventButtonUp);
		addEventHandler< ui::MouseMoveEvent >(this, &RmlDocumentPreviewControl::eventMouseMove);
		addEventHandler< ui::MouseWheelEvent >(this, &RmlDocumentPreviewControl::eventMouseWheel);

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
		const ui::Size sz = getInnerRect().getSize();
		m_rmlDocument = rmlDocument;

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
			// todo:log error
		}

		// todo: remove
		// temporary testing
		//Rml::ElementDocument* document = m_rmlContext->LoadDocumentFromMemory(R"()");
		Rml::ElementDocument* document = m_rmlContext->LoadDocument("assets/demo.rml");

		document->Show();
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
		const float scale = std::max(dpi() / 96.0f, 1.0f);

		if (m_renderView)
		{
			m_renderView->reset(sz.cx, sz.cy);
		}

		if (m_rmlContext)
		{
			m_rmlContext->SetDimensions(Rml::Vector2i(sz.cx, sz.cy));
			//m_rmlContext->SetDimensions(Rml::Vector2i((int32_t)(sz.cx / scale), (int32_t)(sz.cy / scale)));
			m_rmlContext->SetDensityIndependentPixelRatio(scale);
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

		update();

		event->requestMore();
	}

	void RmlDocumentPreviewControl::eventKey(ui::KeyEvent* event)
	{
		if (!m_rmlContext)
		{
			return;
		}

		m_rmlContext->ProcessTextInput((Rml::Character)event->getCharacter());
	}

	void RmlDocumentPreviewControl::eventKeyDown(ui::KeyDownEvent* event)
	{
		if (!m_rmlContext)
		{
			return;
		}

		Rml::Input::KeyIdentifier key = getKey(event->getVirtualKey());

		m_rmlContext->ProcessKeyDown(
			key,
			getModifierState(event->getKeyState()));
	}

	void RmlDocumentPreviewControl::eventKeyUp(ui::KeyUpEvent* event)
	{
		if (!m_rmlContext)
		{
			return;
		}

		Rml::Input::KeyIdentifier key = getKey(event->getVirtualKey());

		m_rmlContext->ProcessKeyUp(
			key,
			getModifierState(event->getKeyState()));
	}

	void RmlDocumentPreviewControl::eventButtonDown(ui::MouseButtonDownEvent* event)
	{
		if (!m_rmlContext)
		{
			return;
		}

		int32_t buttonIndex = getMouseButtonIndex(event->getButton());

		m_rmlContext->ProcessMouseButtonUp(
			buttonIndex,
			getModifierState(event->getKeyState()));
	}

	void RmlDocumentPreviewControl::eventButtonUp(ui::MouseButtonUpEvent* event)
	{
		if (!m_rmlContext)
		{
			return;
		}

		int32_t buttonIndex = getMouseButtonIndex(event->getButton());

		m_rmlContext->ProcessMouseButtonUp(
			buttonIndex,
			getModifierState(event->getKeyState()));
	}

	void RmlDocumentPreviewControl::eventMouseMove(ui::MouseMoveEvent* event)
	{
		if (!m_rmlContext)
		{
			return;
		}
		const ui::Point mousePosition = event->getPosition();
		const float scale = std::max(dpi() / 96.0f, 1.0f);

		m_rmlContext->ProcessMouseMove(
			(int32_t)(mousePosition.x /*/ scale*/),
			(int32_t)(mousePosition.y /*/ scale*/),
			getModifierState(event->getKeyState()));
	}

	void RmlDocumentPreviewControl::eventMouseWheel(ui::MouseWheelEvent* event)
	{
		if (!m_rmlContext)
		{
			return;
		}

		m_rmlContext->ProcessMouseWheel(
			(float)event->getRotation(),
			getModifierState(event->getKeyState()));
	}

}