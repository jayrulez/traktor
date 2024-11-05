/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Resource/IResourceManager.h"
#include "RmlUi/RmlUiResources.h"

namespace traktor::rmlui
{
	namespace
	{
		const resource::Id< render::Shader > c_idShaderColor(Guid(L"{5E18600A-1FCC-5140-AB80-E75615B5D01F}"));
		const resource::Id< render::Shader > c_idShaderTexture(Guid(L"{AFC34A55-B4CF-774F-A86F-B303A7317CF0}"));
		const resource::Id< render::Shader > c_idShaderStencil(Guid(L"{57314865-7390-A14B-8182-1E0DE19D4ABA}"));
	}

	T_IMPLEMENT_RTTI_CLASS(L"traktor.rmlui.RmlUiResources", RmlUiResources, Object)

		bool RmlUiResources::create(resource::IResourceManager* resourceManager)
	{
		if (!resourceManager->bind(c_idShaderColor, m_shaderColor))
			return false;
		if (!resourceManager->bind(c_idShaderTexture, m_shaderTexture))
			return false;
		if (!resourceManager->bind(c_idShaderStencil, m_shaderStencil))
			return false;

		return true;
	}

	void RmlUiResources::destroy()
	{
		m_shaderColor.clear();
		m_shaderTexture.clear();
		m_shaderStencil.clear();
	}

}