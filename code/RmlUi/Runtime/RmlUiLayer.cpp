/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Class/Any.h"
#include "Core/Io/StringOutputStream.h"
#include "Core/Log/Log.h"
#include "Core/Misc/SafeDestroy.h"
#include "Core/Settings/PropertyBoolean.h"
#include "Core/Settings/PropertyGroup.h"
#include "Core/Timer/Profiler.h"
#include "Input/IInputDevice.h"
#include "Input/InputSystem.h"
#include "Net/BidirectionalObjectTransport.h"
#include "Net/SocketAddressIPv4.h"
#include "Net/TcpSocket.h"
#include "Render/IRenderSystem.h"
#include "Render/IRenderTargetSet.h"
#include "Render/IRenderView.h"
#include "Render/Frame/RenderGraph.h"
#include "Runtime/IEnvironment.h"
#include "Runtime/UpdateInfo.h"
#include "Runtime/Engine/Stage.h"
#include "RmlUi/RmlDocument.h"
#include "RmlUi/Runtime/RmlUiLayer.h"

namespace traktor::rmlui
{
	//namespace
	//{

	//	const struct InputKeyCode
	//	{
	//		input::DefaultControl inputKeyCode;
	//		uint32_t asKeyCode;
	//	};

	//	//bool isWhiteSpace(wchar_t ch)
	//	//{
	//	//	return ch == 0 || ch == L' ' || ch == L'\t' || ch == L'\n' || ch == L'\r';
	//	//}

	//	//uint32_t translateInputKeyCode(uint32_t inputKeyCode)
	//	//{
	//	//	for (uint32_t i = 0; i < sizeof_array(c_inputKeyCodes); ++i)
	//	//	{
	//	//		if (c_inputKeyCodes[i].inputKeyCode == (input::DefaultControl)inputKeyCode)
	//	//			return c_inputKeyCodes[i].asKeyCode;
	//	//	}
	//	//	return 0;
	//	//}

	//}

	T_IMPLEMENT_RTTI_CLASS(L"traktor.rmlui.RmlUiLayer", RmlUiLayer, runtime::Layer)

		RmlUiLayer::RmlUiLayer(
			runtime::Stage* stage,
			const std::wstring& name,
			bool permitTransition,
			runtime::IEnvironment* environment,
			const resource::Proxy< RmlDocument >& rmlDocument,
			bool clearBackground
		)
		: Layer(stage, name, permitTransition)
		, m_environment(environment)
		, m_rmlDocument(rmlDocument)
		, m_clearBackground(clearBackground)
		, m_visible(true)
		, m_offset(0.0f, 0.0f)
		, m_scale(1.0f)
		, m_lastUpValue(false)
		, m_lastDownValue(false)
		, m_lastConfirmValue(false)
		, m_lastEscapeValue(false)
		, m_lastMouseX(-1)
		, m_lastMouseY(-1)
	{
	}

	void RmlUiLayer::destroy()
	{
		m_environment = nullptr;
		m_rmlDocument.clear();

		Layer::destroy();
	}

	void RmlUiLayer::transition(Layer* fromLayer)
	{
		RmlUiLayer* fromRmlUiLayer = checked_type_cast<RmlUiLayer*, false>(fromLayer);

		// Pass document as well, if it's the same document and we're allowed.
		const bool permit = fromLayer->isTransitionPermitted() && isTransitionPermitted();
		if (m_rmlDocument == fromRmlUiLayer->m_rmlDocument)
		{
			if (permit)
			{
				m_rmlDocument.consume();
			}
		}
	}

	void RmlUiLayer::preUpdate(const runtime::UpdateInfo& info)
	{
		T_PROFILER_SCOPE(L"RmlUiLayer pre-update");

		if (m_rmlDocument.changed())
		{
			m_rmlDocument.consume();
		}
	}

	void RmlUiLayer::update(const runtime::UpdateInfo& info)
	{
		T_PROFILER_SCOPE(L"RmlUiLayer update");
		render::IRenderView* renderView = m_environment->getRender()->getRenderView();
		input::InputSystem* inputSystem = m_environment->getInput()->getInputSystem();
		std::string command, args;

		// Do NOT propagate input in case user is fabricating input.
		if (!m_environment->getInput()->isFabricating())
		{
			// Propagate keyboard input to RmlUi.
			input::IInputDevice* keyboardDevice = inputSystem->getDevice(input::InputCategory::Keyboard, 0, true);
			if (keyboardDevice)
			{
				input::IInputDevice::KeyEvent ke;
				while (keyboardDevice->getKeyEvent(ke))
				{
					//if (ke.type == input::KeyEventType::Character)
					//	m_moviePlayer->postKey(ke.character);
					//else
					//{
					//	uint32_t keyCode = translateInputKeyCode(ke.keyCode);
					//	if (keyCode != 0)
					//	{
					//		if (ke.type == input::KeyEventType::Down)
					//			m_moviePlayer->postKeyDown(keyCode);
					//		else if (ke.type == input::KeyEventType::Up)
					//			m_moviePlayer->postKeyUp(keyCode);
					//	}
					//}
				}
			}

			// Propagate joystick input to RmlUi; synthesized as keyboard events.
			int32_t joystickDeviceCount = inputSystem->getDeviceCount(input::InputCategory::Joystick, true);
			for (int32_t i = 0; i < joystickDeviceCount; ++i)
			{
				input::IInputDevice* joystickDevice = inputSystem->getDevice(input::InputCategory::Joystick, i, true);
				T_ASSERT(joystickDevice);

				int32_t up = -1, down = -1;
				joystickDevice->getDefaultControl(input::DefaultControl::Up, false, up);
				joystickDevice->getDefaultControl(input::DefaultControl::Down, false, down);

				if (up != -1)
				{
					const bool upValue = bool(joystickDevice->getControlValue(up) > 0.5f);
					if (upValue != m_lastUpValue)
					{
						//if (upValue)
						//	m_moviePlayer->postKeyDown(Key::AkUp);
						//else
						//	m_moviePlayer->postKeyUp(Key::AkUp);
					}
					m_lastUpValue = upValue;
				}

				if (down != -1)
				{
					const bool downValue = bool(joystickDevice->getControlValue(down) > 0.5f);
					if (downValue != m_lastDownValue)
					{
						//if (downValue)
						//	m_moviePlayer->postKeyDown(Key::AkDown);
						//else
						//	m_moviePlayer->postKeyUp(Key::AkDown);
					}
					m_lastDownValue = downValue;
				}

				int32_t buttonConfirm = -1, buttonEscape = -1;
				joystickDevice->getDefaultControl(input::DefaultControl::Button1, false, buttonConfirm);
				joystickDevice->getDefaultControl(input::DefaultControl::Button2, false, buttonEscape);

				if (buttonConfirm != -1)
				{
					const bool confirmValue = bool(joystickDevice->getControlValue(buttonConfirm) > 0.5f);
					if (confirmValue != m_lastConfirmValue)
					{
						//if (confirmValue)
						//	m_moviePlayer->postKeyDown(Key::AkEnter);
						//else
						//	m_moviePlayer->postKeyUp(Key::AkEnter);
					}
					m_lastConfirmValue = confirmValue;
				}

				if (buttonEscape != -1)
				{
					const bool escapeValue = bool(joystickDevice->getControlValue(buttonEscape) > 0.5f);
					if (escapeValue != m_lastEscapeValue)
					{
						//if (escapeValue)
						//	m_moviePlayer->postKeyDown(Key::AkEscape);
						//else
						//	m_moviePlayer->postKeyUp(Key::AkEscape);
					}
					m_lastEscapeValue = escapeValue;
				}
			}

			// Propagate mouse input to RmlUi; don't send mouse events if mouse cursor isn't visible.
			if (renderView->isCursorVisible())
			{
				int32_t width = renderView->getWidth();
				int32_t height = renderView->getHeight();

				const float viewRatio = m_environment->getRender()->getViewAspectRatio();
				const float aspectRatio = m_environment->getRender()->getAspectRatio();

				width = int32_t(width * aspectRatio / viewRatio);

				int32_t mouseDeviceCount = inputSystem->getDeviceCount(input::InputCategory::Mouse, true);
				if (mouseDeviceCount >= sizeof_array(m_lastMouse))
					mouseDeviceCount = sizeof_array(m_lastMouse);

				for (int32_t i = 0; i < mouseDeviceCount; ++i)
				{
					input::IInputDevice* mouseDevice = inputSystem->getDevice(input::InputCategory::Mouse, i, true);
					T_ASSERT(mouseDevice);

					LastMouseState& last = m_lastMouse[i];

					int32_t positionX = -1, positionY = -1;
					mouseDevice->getDefaultControl(input::DefaultControl::PositionX, true, positionX);
					mouseDevice->getDefaultControl(input::DefaultControl::PositionY, true, positionY);

					int32_t button1 = -1, button2 = -1;
					mouseDevice->getDefaultControl(input::DefaultControl::Button1, false, button1);
					mouseDevice->getDefaultControl(input::DefaultControl::Button2, false, button2);

					int32_t axisZ = -1;
					mouseDevice->getDefaultControl(input::DefaultControl::AxisZ, true, axisZ);

					float minX, minY;
					float maxX, maxY;
					mouseDevice->getControlRange(positionX, minX, maxX);
					mouseDevice->getControlRange(positionY, minY, maxY);

					if (maxX > minX && maxY > minY)
					{
						float x = mouseDevice->getControlValue(positionX);
						float y = mouseDevice->getControlValue(positionY);

						x = (x - minX) / (maxX - minX);
						y = (y - minY) / (maxY - minY);

						const int32_t mx = int32_t(width * x);
						const int32_t my = int32_t(height * y);

						const int32_t mb =
							(mouseDevice->getControlValue(button1) > 0.5f ? 1 : 0) |
							(mouseDevice->getControlValue(button2) > 0.5f ? 2 : 0);

						if (mx != m_lastMouseX || my != m_lastMouseY)
						{
							//m_moviePlayer->postMouseMove(mx, my, mb);
							m_lastMouseX = mx;
							m_lastMouseY = my;
						}

						if (mb != last.button)
						{
							//if (mb)
							//	m_moviePlayer->postMouseDown(mx, my, mb);
							//else
							//	m_moviePlayer->postMouseUp(mx, my, mb);

							last.button = mb;
						}

						if (axisZ != -1)
						{
							const int32_t wheel = int32_t(mouseDevice->getControlValue(axisZ) * 3.0f);
							if (wheel != last.wheel)
							{
								//m_moviePlayer->postMouseWheel(mx, my, wheel);
								last.wheel = wheel;
							}
						}
					}
				}
			}
		}
	}

	void RmlUiLayer::postUpdate(const runtime::UpdateInfo& info)
	{
	}

	void RmlUiLayer::preSetup(const runtime::UpdateInfo& info)
	{
		if (m_rmlDocument.changed())
		{
			m_rmlDocument.consume();
		}
	}

	void RmlUiLayer::setup(const runtime::UpdateInfo& info, render::RenderGraph& renderGraph)
	{
	}

	void RmlUiLayer::preReconfigured()
	{
	}

	void RmlUiLayer::postReconfigured()
	{
		// Post resize to RmlUi; adjust width to keep aspect ratio.
		//if (m_moviePlayer)
		{
			render::IRenderView* renderView = m_environment->getRender()->getRenderView();
			float viewRatio = m_environment->getRender()->getViewAspectRatio();
			float aspectRatio = m_environment->getRender()->getAspectRatio();

			int32_t width = int32_t(renderView->getWidth() * aspectRatio / viewRatio);
			int32_t height = renderView->getHeight();

			//m_moviePlayer->postViewResize(width, height);
		}
	}

	void RmlUiLayer::suspend()
	{
	}

	void RmlUiLayer::resume()
	{
	}

	void RmlUiLayer::hotReload()
	{
	}
}