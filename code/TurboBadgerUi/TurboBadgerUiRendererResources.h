/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "TurboBadgerUi/Types.h"

#include "Core/Object.h"
#include "Render/Shader.h"
#include "Render/ITexture.h"
#include "Resource/Proxy.h"

 // import/export mechanism.
#undef T_DLLCLASS
#if defined(T_TURBOBADGERUI_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::resource
{
	class IResourceManager;
}

namespace traktor::render
{
	class IRenderSystem;
}

namespace traktor::turbobadgerui
{
	class TurboBadgerUiRendererResources : public Object
	{
		T_RTTI_CLASS;

	public:
		bool create(resource::IResourceManager* resourceManager, render::IRenderSystem* renderSystem);

		void destroy();

	private:
		friend class TurboBadgerUiRenderer;

		resource::Proxy< render::Shader > m_shader;
		Ref< render::ITexture > m_defaultTexture;
	};
}