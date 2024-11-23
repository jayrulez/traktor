/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/TurboBadgerUiRendererResources.h"

#include "Core/Misc/SafeDestroy.h"
#include "Resource/IResourceManager.h"
#include "Render/IRenderSystem.h"

namespace traktor::turbobadgerui
{
	namespace
	{
		const resource::Id< render::Shader > c_idShader(Guid(L"{DC42B494-A516-E044-B088-DA73EF65571A}")); // System/TurboBadgerUi/Shaders/TurboBadgerUi_ColorTexture
	}

	T_IMPLEMENT_RTTI_CLASS(L"traktor.turbobadgerui.TurboBadgerUiRendererResources", TurboBadgerUiRendererResources, Object)

		bool TurboBadgerUiRendererResources::create(resource::IResourceManager* resourceManager, render::IRenderSystem* renderSystem)
	{
		if (!resourceManager->bind(c_idShader, m_shader))
			return false;

		render::SimpleTextureCreateDesc createInfo = {
			.width = 2,
			.height = 2,
			.mipCount = 1,
			.format = render::TfR8G8B8A8,
			.sRGB = false,
			.immutable = true,
			.shaderStorage = false,
		};

		static const uint8_t textureData[2 * 2 * 4] = {
			255, 255, 255, 255, // Pixel (0,0) - White
			255, 255, 255, 255, // Pixel (0,1) - White
			255, 255, 255, 255, // Pixel (1,0) - White
			255, 255, 255, 255  // Pixel (1,1) - White
		};

		createInfo.initialData[0] = {
			.data = &textureData,
			.pitch = 4 * 2,
			.slicePitch = 4 * 2 * 2
		};

		m_defaultTexture = renderSystem->createSimpleTexture(createInfo, L"TurboBadgerUi_DefaultTexture");

		if (!m_defaultTexture)
		{
			return false;
		}

		return true;
	}

	void TurboBadgerUiRendererResources::destroy()
	{
		safeDestroy(m_defaultTexture);
		m_shader.clear();
	}
}