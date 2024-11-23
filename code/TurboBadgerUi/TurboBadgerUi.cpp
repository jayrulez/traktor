/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/TurboBadgerUi.h"
#include "TurboBadgerUi/TurboBadgerUiView.h"

#include "Core/Singleton/SingletonManager.h"

#include "tb_core.h"
#include "tb_widgets.h"
#include "tb_msg.h"
#include "animation/tb_animation.h"

namespace traktor::turbobadgerui
{
	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.turbobadgerui.TurboBadgerUi", 0, TurboBadgerUi, Object)

		TurboBadgerUi::BackendData::BackendData(render::IRenderSystem* renderSystem)
		: renderer(renderSystem)
	{

	}

	TurboBadgerUi& TurboBadgerUi::getInstance()
	{
		static TurboBadgerUi* s_instance = nullptr;
		if (!s_instance)
		{
			s_instance = new TurboBadgerUi();
			s_instance->addRef(nullptr);
			SingletonManager::getInstance().add(s_instance);
		}
		return *s_instance;
	}

	void TurboBadgerUi::destroy()
	{
		T_SAFE_RELEASE(this);
	}

	bool TurboBadgerUi::initialize(render::IRenderSystem* renderSystem)
	{
		if (m_initialized)
			return true;

		m_backendData = new BackendData(renderSystem);

		m_initialized = tb::tb_core_init(&m_backendData->renderer);

		if (!m_initialized)
		{
			delete m_backendData;
		}

		return m_initialized;
	}

	void TurboBadgerUi::shutdown()
	{
		if (!m_initialized)
			return;

		tb::tb_core_shutdown();

		delete m_backendData;

		m_initialized = false;
	}

	bool TurboBadgerUi::isInitialized() const
	{
		return m_initialized;
	}

	TurboBadgerUiView* TurboBadgerUi::createView()
	{
		if (!m_initialized)
			return nullptr;

		TurboBadgerUiView* view = new TurboBadgerUiView();
		m_views.push_back(view);

		return view;
	}

	void TurboBadgerUi::destroyView(TurboBadgerUiView* view)
	{
		if (view == nullptr)
			return;

		auto it = std::find(m_views.begin(), m_views.end(), view);
		if (it != m_views.end())
		{
			m_views.erase(it);
		}

		delete view;
	}

	void TurboBadgerUi::update()
	{
		tb::TBMessageHandler::ProcessMessages();
		tb::TBAnimationManager::Update();
	}

	void TurboBadgerUi::updateView(TurboBadgerUiView* view)
	{
		view->InvokeProcessStates();
		view->InvokeProcess();
	}

	void TurboBadgerUi::reloadRenderResources()
	{
		m_backendData->renderer.InvokeContextLost(); // Forget all bitmaps
		m_backendData->renderer.InvokeContextRestored(); // Reload all bitmaps
	}

	void TurboBadgerUi::renderView(TurboBadgerUiView* view, uint32_t width, uint32_t height, AlignedVector<TurboBadgerUiBatch>& batches)
	{
		batches = m_backendData->renderer.renderView(view, width, height);
	}
}