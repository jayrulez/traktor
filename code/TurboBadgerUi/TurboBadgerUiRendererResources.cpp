/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/TurboBadgerUiRendererResources.h"

#include "Resource/IResourceManager.h"

namespace traktor::turbobadgerui
{
	namespace
	{
		const resource::Id< render::Shader > c_idShader(Guid(L"{DC42B494-A516-E044-B088-DA73EF65571A}")); // System/TurboBadgerUi/Shaders/TurboBadgerUi_ColorTexture
	}

	T_IMPLEMENT_RTTI_CLASS(L"traktor.turbobadgerui.TurboBadgerUiRendererResources", TurboBadgerUiRendererResources, Object)

		bool TurboBadgerUiRendererResources::create(resource::IResourceManager* resourceManager)
	{
		if (!resourceManager->bind(c_idShader, m_shader))
			return false;

		return true;
	}

	void TurboBadgerUiRendererResources::destroy()
	{
		m_shader.clear();
	}
}