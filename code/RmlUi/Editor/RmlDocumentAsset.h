/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Containers/AlignedVector.h"
#include "Editor/Asset.h"

 // import/export mechanism.
#undef T_DLLCLASS
#if defined(T_RMLUI_EDITOR_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::rmlui
{

	class T_DLLCLASS RmlDocumentAsset : public editor::Asset
	{
		T_RTTI_CLASS;

	public:

		virtual void serialize(ISerializer& s) override final;

	private:
		friend class Pipeline;

	};

}