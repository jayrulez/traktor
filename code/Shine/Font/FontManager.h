/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Object.h"
#include "Core/Ref.h"
#include "Core/Guid.h"
#include "Core/Containers/SmallMap.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::resource
{
	class IResourceManager;
}

namespace traktor::shine
{

class IFont;

/*!
 * \brief Font manager
 * \ingroup Shine
 *
 * Manages font loading and caching via Traktor's resource system.
 * Loads BitmapFontResource and creates BitmapFont instances.
 */
class T_DLLCLASS FontManager : public Object
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param resourceManager Resource manager for loading font resources
	 */
	explicit FontManager(resource::IResourceManager* resourceManager);

	virtual ~FontManager();

	/*!
	 * \brief Get or load font by GUID
	 * \param fontId Font resource GUID
	 * \return Font instance, or nullptr if failed to load
	 */
	Ref<IFont> getFont(const Guid& fontId);

	/*!
	 * \brief Clear font cache
	 */
	void clearCache();

private:
	resource::IResourceManager* m_resourceManager;
	SmallMap<Guid, Ref<IFont>> m_fontCache;
};

}
