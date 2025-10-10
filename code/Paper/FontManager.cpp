/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Paper/FontManager.h"
#include "Paper/IFont.h"
#include "Resource/IResourceManager.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.paper.FontManager", FontManager, Object)

FontManager::FontManager(resource::IResourceManager* resourceManager)
:	m_resourceManager(resourceManager)
{
}

void FontManager::registerFont(const std::wstring& name, const Guid& fontId)
{
	m_fontRegistry[name] = fontId;
}

Ref< const IFont > FontManager::getFont(const std::wstring& name) const
{
	auto it = m_fontRegistry.find(name);
	if (it == m_fontRegistry.end())
		return nullptr;

	return getFont(it->second);
}

Ref< const IFont > FontManager::getFont(const Guid& fontId) const
{
	// Check cache first
	auto it = m_fontCache.find(fontId);
	if (it != m_fontCache.end())
		return it->second;

	// Load font from resource manager
	resource::Proxy< IFont > fontProxy;
	if (!m_resourceManager->bind(resource::Id< IFont >(fontId), fontProxy))
		return nullptr;

	Ref< const IFont > font = fontProxy.getResource();
	if (!font)
		return nullptr;

	// Cache the font
	m_fontCache[fontId] = font;
	return font;
}

}
