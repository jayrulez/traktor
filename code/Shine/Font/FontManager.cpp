/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Log/Log.h"
#include "Resource/IResourceManager.h"
#include "Shine/Font/BitmapFontResource.h"
#include "Shine/Font/BitmapFont.h"
#include "Shine/Font/FontManager.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.FontManager", FontManager, Object)

FontManager::FontManager(resource::IResourceManager* resourceManager)
:	m_resourceManager(resourceManager)
{
}

FontManager::~FontManager()
{
}

Ref<IFont> FontManager::getFont(const Guid& fontId)
{
	auto it = m_fontCache.find(fontId);
	if (it != m_fontCache.end())
		return it->second;

	resource::Proxy<BitmapFontResource> fontResource;
	if (!m_resourceManager->bind(resource::Id<BitmapFontResource>(fontId), fontResource))
	{
		log::error << L"FontManager: Failed to load font " << fontId.format() << Endl;
		return nullptr;
	}

	Ref<BitmapFont> font = new BitmapFont();
	if (!font->create(fontResource))
	{
		log::error << L"FontManager: Failed to create font " << fontId.format() << Endl;
		return nullptr;
	}

	m_fontCache[fontId] = font;
	return font;
}

void FontManager::clearCache()
{
	m_fontCache.clear();
}

}
