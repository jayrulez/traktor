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
		/*const resource::Id< render::Shader > c_idShaderColor(Guid(L"{5E18600A-1FCC-5140-AB80-E75615B5D01F}"));
		const resource::Id< render::Shader > c_idShaderTexture(Guid(L"{AFC34A55-B4CF-774F-A86F-B303A7317CF0}"));*/
		const resource::Id< render::Shader > c_idShaderColor(Guid(L"{FC6749BF-3F4F-0A44-A422-943E7B055825}"));
		const resource::Id< render::Shader > c_idShaderTexture(Guid(L"{40BC3CDC-189B-2147-BDC6-076C28EFE2D3}"));
		const resource::Id< render::Shader > c_idShaderColorStencil(Guid(L"{CF117AE2-CB27-AD4B-8A0B-ECFE3351788D}"));
		const resource::Id< render::Shader > c_idShaderTextureStencil(Guid(L"{A4A53552-DA43-D748-8DBC-F0472EE2C2A0}"));
		const resource::Id< render::Shader > c_idShaderStencilGeometry(Guid(L"{B0694000-F743-6A49-B951-D8B621C78E44}"));
	}

	T_IMPLEMENT_RTTI_CLASS(L"traktor.rmlui.RmlUiResources", RmlUiResources, Object)

		bool RmlUiResources::create(resource::IResourceManager* resourceManager)
	{
		if (!resourceManager->bind(c_idShaderColor, m_shaderColor))
			return false;
		if (!resourceManager->bind(c_idShaderTexture, m_shaderTexture))
			return false;
		if (!resourceManager->bind(c_idShaderColorStencil, m_shaderColorStencil))
			return false;
		if (!resourceManager->bind(c_idShaderTextureStencil, m_shaderTextureStencil))
			return false;
		if (!resourceManager->bind(c_idShaderStencilGeometry, m_shaderStencilGeometry))
			return false;

		return true;
	}

	void RmlUiResources::destroy()
	{
		m_shaderColor.clear();
		m_shaderTexture.clear();
		m_shaderColorStencil.clear();
	}

}