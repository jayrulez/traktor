/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Config.h"
#include "Core/Object.h"
#include "Core/Math//Vector2i.h"
#include "Core/Singleton/ISingleton.h"
#include "RmlUi/Core/Core.h"
#include "RmlUi/Core/Input.h"
#include "RmlUi/Backend/FileInterface.h"
#include "RmlUi/Backend/RenderInterface.h"
#include "RmlUi/Backend/SystemInterface.h"
#include "Render/IRenderSystem.h"
#include "Render/IRenderView.h"
#include "Resource/IResourceManager.h"

 // import/export mechanism.
#undef T_DLLCLASS
#if defined(T_RMLUI_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

using KeyDownCallback = bool (*)(Rml::Context* context, Rml::Input::KeyIdentifier key, int key_modifier, float native_dp_ratio, bool priority);

namespace traktor::rmlui
{
	class T_DLLCLASS RmlUi
		: public Object
		, public ISingleton
	{
		T_RTTI_CLASS;

	public:
		RmlUi() = default;

		static RmlUi& getInstance();

		void destroy() override;

		bool initialize(resource::IResourceManager* resourceManager, render::IRenderSystem* renderSystem);

		bool isInitialized() const;

		void Shutdown();

		SystemInterface* GetSystemInterface() const;

		RenderInterface* GetRenderInterface() const;

		FileInterface* GetFileInterface() const;

		//Rml::Context* GetContext() const;

		Rml::Context* CreateContext(const std::wstring& name, traktor::Vector2i size);

		void DestroyContext(Rml::Context* context);

	private:
		struct BackendData
		{
			BackendData(
				const resource::Proxy< render::Shader >& rmlUiShader,
				const resource::Proxy< render::Shader >& rmlUiShaderWithTexture, 
				render::IRenderSystem* renderSystem);

			FileInterface m_fileInterface;
			RenderInterface m_renderInterface;
			SystemInterface m_systemInterface;
		};

		bool m_initialized = false;
		resource::Proxy< render::Shader > m_rmlUiShader;
		resource::Proxy< render::Shader > m_rmlUiShaderWithTexture;
		BackendData* m_backendData = nullptr;
		AlignedVector<Rml::Context*> m_rmlContexts;
	};

}