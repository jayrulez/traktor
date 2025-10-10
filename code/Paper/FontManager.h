/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <string>
#include "Core/Object.h"
#include "Core/Ref.h"
#include "Core/Containers/SmallMap.h"
#include "Core/Guid.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_PAPER_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::resource
{

class IResourceManager;

}

namespace traktor::paper
{

class IFont;

/*! Font manager.
 * \ingroup Paper
 */
class T_DLLCLASS FontManager : public Object
{
	T_RTTI_CLASS;

public:
	explicit FontManager(resource::IResourceManager* resourceManager);

	/*! Register font with manager.
	 *
	 * \param name Font name.
	 * \param fontId Font resource identifier.
	 */
	void registerFont(const std::wstring& name, const Guid& fontId);

	/*! Get font by name.
	 *
	 * \param name Font name.
	 * \return Font or null if not found.
	 */
	Ref< const IFont > getFont(const std::wstring& name) const;

	/*! Get font by resource identifier.
	 *
	 * \param fontId Font resource identifier.
	 * \return Font or null if not found.
	 */
	Ref< const IFont > getFont(const Guid& fontId) const;

private:
	Ref< resource::IResourceManager > m_resourceManager;
	SmallMap< std::wstring, Guid > m_fontRegistry;
	mutable SmallMap< Guid, Ref< const IFont > > m_fontCache;
};

}
