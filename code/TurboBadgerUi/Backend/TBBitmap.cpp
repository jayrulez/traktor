/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/Backend/TBBitmap.h"

#include "TurboBadgerUi/Backend/TBRenderer.h"

#include "Render/ITexture.h"
#include "Render/IRenderSystem.h"

namespace traktor::turbobadgerui
{
	TBBitmap::TBBitmap(TBRenderer* renderer, render::IRenderSystem* renderSystem)
		: m_renderer(renderer),
		m_renderSystem(renderSystem),
		m_texture(nullptr)
	{
	}

	TBBitmap::~TBBitmap()
	{
		m_renderer->FlushBitmap(this);
	}

	bool
		TBBitmap::Init(int width, int height, unsigned int* data)
	{
		m_width = width;
		m_height = height;

		render::SimpleTextureCreateDesc desc;
		desc.width = m_width;
		desc.height = m_height;
		desc.format = render::TextureFormat::TfR8G8B8A8;
		desc.immutable = true;
		desc.sRGB = false;
		render::TextureInitialData initialData;
		initialData.data = data;
		initialData.pitch = 4 * desc.width;
		initialData.slicePitch = 4 * desc.width * desc.height;
		desc.initialData[0] = initialData;
		desc.mipCount = 1;
		m_texture = m_renderSystem->createSimpleTexture(desc, T_FILE_LINE_W);

		//SetData(data);
		return true;
	}

	int TBBitmap::Width()
	{
		return m_width;
	}

	int TBBitmap::Height()
	{
		return m_height;
	}

	void TBBitmap::SetData(unsigned int* data)
	{
		render::ITexture::Lock lock;
		m_texture->lock(0, 0, lock);
		std::memcpy(lock.bits, data, m_width * m_height * 4);
		m_texture->unlock(0, 0);
	}
}