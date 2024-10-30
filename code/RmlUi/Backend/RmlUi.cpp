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
	}

	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.rmlui.RmlUi", 0, RmlUi, Object)

		RmlUi::BackendData::BackendData(
			resource::IResourceManager* resourceManager,
			render::IRenderSystem* renderSystem)
		: renderInterface(resourceManager, renderSystem)
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

		m_backendData = new BackendData(resourceManager, renderSystem);

		Rml::SetFileInterface(&m_backendData->fileInterface);
		Rml::SetSystemInterface(&m_backendData->systemInterface);
		Rml::SetRenderInterface(&m_backendData->renderInterface);

		m_initialized = Rml::Initialise();

		Rml::LoadFontFace("assets/Atop-R99O3.ttf");

		m_backendData->renderInterface.loadResources();

		return m_initialized;
	}

	bool RmlUi::isInitialized() const
	{
		return m_initialized;
	}

	void RmlUi::shutdown()
	{
		if (!m_initialized)
			return;

		Rml::Shutdown();

		m_backendData->renderInterface.unloadResources();

		delete m_backendData;

		m_initialized = false;
	}

	SystemInterface* RmlUi::getSystemInterface() const
	{
		if (!m_initialized)
			return nullptr;

		return &m_backendData->systemInterface;
	}

	RenderInterface* RmlUi::getRenderInterface() const
	{
		if (!m_initialized)
			return nullptr;

		return &m_backendData->renderInterface;
	}

	FileInterface* RmlUi::getFileInterface() const
	{
		if (!m_initialized)
			return nullptr;

		return &m_backendData->fileInterface;
	}

	Rml::Context* RmlUi::createContext(const std::wstring& name, traktor::Vector2i size)
	{
		if (!m_initialized)
			return nullptr;

		Rml::Context* context = Rml::CreateContext(wstombs(name), Rml::Vector2i(size.x, size.y));
		m_rmlContexts.push_back(context);
		return context;
	}

	void RmlUi::destroyContext(Rml::Context* context)
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

	AlignedVector<RenderInterface::Batch> RmlUi::renderContext(Rml::Context* context)
	{
		m_backendData->renderMutex.wait();
		AlignedVector<RenderInterface::Batch> batches = {};
		if (context != nullptr)
		{
			m_backendData->renderInterface.beginRendering();

			context->Render();

			batches = m_backendData->renderInterface.getBatches();

			m_backendData->renderInterface.endRendering();
		}

		m_backendData->renderMutex.release();

		return batches;
	}

	void RmlUi::reloadResources()
	{
		m_backendData->renderInterface.reloadResources();
	}
}