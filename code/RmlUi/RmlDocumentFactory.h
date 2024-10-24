/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Object.h"
#include "Core/Io/Path.h"
#include "Database/Instance.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_RMLUI_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::rmlui
{

class RmlDocument;

/*! Rml document factory.
 * \ingroup RmlUi
 */
class T_DLLCLASS RmlDocumentFactory : public Object
{
	T_RTTI_CLASS;

public:
	RmlDocumentFactory();

	Ref< RmlDocument > createRmlDocument(const traktor::Path& assetPath, const db::Instance* sourceInstance, IStream* sourceStream) const;
};

}
