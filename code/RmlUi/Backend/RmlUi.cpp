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
#include "Core/Misc/TString.h"
#include "RmlUi/Backend/RmlUi.h"
#include "RmlUi/Core/Context.h"

namespace traktor::rmlui
{
	namespace
	{
		const Rml::String DefaultContextName = "Default";

		const resource::Id< render::Shader > c_idShaderRmlUiShader(Guid(L"{20046EBD-5DC4-494B-AF59-8F89AFFCC107}"));
		const resource::Id< render::Shader > c_idShaderRmlUiShaderWithTexture(Guid(L"{2664F5C5-16C6-B042-ACF5-27EC491EBCB6}"));
	}

	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.rmlui.RmlUi", 0, RmlUi, Object)

		RmlUi::BackendData::BackendData(
			const resource::Proxy< render::Shader >& rmlUiShader,
			const resource::Proxy< render::Shader >& rmlUiShaderWithTexture,
			render::IRenderSystem* renderSystem)
		: m_renderInterface(rmlUiShader, rmlUiShaderWithTexture, renderSystem)
	{

	}

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

	bool RmlUi::initialize(resource::IResourceManager* resourceManager, render::IRenderSystem* renderSystem)
	{
		if (m_initialized)
			return true;

		if (!resourceManager->bind(c_idShaderRmlUiShader, m_rmlUiShader))
			return false;

		if (!resourceManager->bind(c_idShaderRmlUiShaderWithTexture, m_rmlUiShaderWithTexture))
			return false;

		m_backendData = new BackendData(m_rmlUiShader, m_rmlUiShaderWithTexture, renderSystem);

		Rml::SetFileInterface(&m_backendData->m_fileInterface);
		Rml::SetSystemInterface(&m_backendData->m_systemInterface);
		Rml::SetRenderInterface(&m_backendData->m_renderInterface);

		m_initialized = Rml::Initialise();

		return m_initialized;
	}

	bool RmlUi::isInitialized() const
	{
		return m_initialized;
	}

	void RmlUi::Shutdown()
	{
		if (!m_initialized)
			return;

		Rml::Shutdown();

		delete m_backendData;

		m_rmlUiShader.clear();
		m_rmlUiShaderWithTexture.clear();

		m_initialized = false;
	}

	SystemInterface* RmlUi::GetSystemInterface() const
	{
		if (!m_initialized)
			return nullptr;

		return &m_backendData->m_systemInterface;
	}

	RenderInterface* RmlUi::GetRenderInterface() const
	{
		if (!m_initialized)
			return nullptr;

		return &m_backendData->m_renderInterface;
	}

	FileInterface* RmlUi::GetFileInterface() const
	{
		if (!m_initialized)
			return nullptr;

		return &m_backendData->m_fileInterface;
	}

	Rml::Context* RmlUi::CreateContext(const std::wstring& name, traktor::Vector2i size)
	{
		if (!m_initialized)
			return nullptr;

		Rml::Context* context = Rml::CreateContext(wstombs(name), Rml::Vector2i(size.x, size.y));
		m_rmlContexts.push_back(context);
		return context;
	}

	void RmlUi::DestroyContext(Rml::Context* context)
	{
		if (!m_initialized)
			return;

		auto it = std::find(m_rmlContexts.begin(), m_rmlContexts.end(), context);
		if (it != m_rmlContexts.end())
		{
			m_rmlContexts.erase(it);
		}
		Rml::RemoveContext(context->GetName());
	}
}