/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Core/Rtti/ITypedObject.h"
#include "Core/Singleton/SingletonManager.h"
#include "RmlUi/Backend/RmlUi.h"

namespace traktor::rmlui
{
	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.rmlui.RmlUi", 0, RmlUi, Object)

		RmlUi& RmlUi::getInstance()
	{
		static RmlUi* s_instance = nullptr;
		if (!s_instance)
		{
			s_instance = new RmlUi();
			s_instance->addRef(nullptr);
			SingletonManager::getInstance().add(s_instance);
		}
		return *s_instance;
	}

	void RmlUi::destroy()
	{
		T_SAFE_RELEASE(this);
	}

	bool RmlUi::Initialize()
	{
		if (m_initialized)
			return true;

		m_backendData = new BackendData();

		m_initialized = true;

		return m_initialized;
	}

	void RmlUi::Shutdown()
	{
		if (!m_initialized)
			return;

		delete m_backendData;

		m_initialized = false;
	}

	Rml::SystemInterface* RmlUi::GetSystemInterface()
	{
		if (!m_initialized)
			return nullptr;

		return m_backendData->m_systemInterface;
	}

	Rml::RenderInterface* RmlUi::GetRenderInterface()
	{
		if (!m_initialized)
			return nullptr;

		return m_backendData->m_renderInterface;
	}

	Rml::FileInterface* RmlUi::GetFileInterface()
	{
		if (!m_initialized)
			return nullptr;

		return m_backendData->m_fileInterface;
	}
}