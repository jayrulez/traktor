/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/Runtime/TurboBadgerUiLayer.h"
#include "TurboBadgerUi/TurboBadgerUiResource.h"

#include "Core/Timer/Profiler.h"
#include "Runtime/IEnvironment.h"
#include "Runtime/UpdateInfo.h"
#include "Runtime/Engine/Stage.h"

namespace traktor::turbobadgerui
{
	T_IMPLEMENT_RTTI_CLASS(L"traktor.turbobadgerui.TurboBadgerUiLayer", TurboBadgerUiLayer, runtime::Layer)

		TurboBadgerUiLayer::TurboBadgerUiLayer(
			runtime::Stage* stage,
			const std::wstring& name,
			bool permitTransition,
			runtime::IEnvironment* environment,
			const resource::Proxy< TurboBadgerUiResource >& uiResource
		)
		: Layer(stage, name, permitTransition)
		, m_environment(environment)
		, m_uiResource(uiResource)
	{
	}

	void TurboBadgerUiLayer::destroy()
	{
		m_environment = nullptr;
		m_uiResource.clear();

		Layer::destroy();
	}

	void TurboBadgerUiLayer::transition(Layer* fromLayer)
	{
		//todo
	}

	void TurboBadgerUiLayer::preUpdate(const runtime::UpdateInfo& info)
	{
		T_PROFILER_SCOPE(L"TurboBadgerUiLayer pre-update");

		if (m_uiResource.changed())
		{
			m_uiResource.consume();
		}

		// todo
	}

	void TurboBadgerUiLayer::update(const runtime::UpdateInfo& info)
	{
		T_PROFILER_SCOPE(L"TurboBadgerUiLayer update");
		render::IRenderView* renderView = m_environment->getRender()->getRenderView();
		input::InputSystem* inputSystem = m_environment->getInput()->getInputSystem();
		std::string command, args;

		// todo
	}

	void TurboBadgerUiLayer::postUpdate(const runtime::UpdateInfo& info)
	{
	}

	void TurboBadgerUiLayer::preSetup(const runtime::UpdateInfo& info)
	{
		if (m_uiResource.changed())
		{
			m_uiResource.consume();
		}

		// todo
	}

	void TurboBadgerUiLayer::setup(const runtime::UpdateInfo& info, render::RenderGraph& renderGraph)
	{
		// todo
	}

	void TurboBadgerUiLayer::preReconfigured()
	{
	}

	void TurboBadgerUiLayer::postReconfigured()
	{
		// todo
	}

	void TurboBadgerUiLayer::suspend()
	{
	}

	void TurboBadgerUiLayer::resume()
	{
	}

	void TurboBadgerUiLayer::hotReload()
	{
	}
}