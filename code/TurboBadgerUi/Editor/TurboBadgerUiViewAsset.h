/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "TurboBadgerUi/Editor/Types.h"

#include "Editor/Asset.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_TURBOBADGERUI_EDITOR_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::turbobadgerui
{
	class T_DLLCLASS TurboBadgerUiViewAsset : public editor::Asset
	{
		T_RTTI_CLASS;

	public:
		TurboBadgerUiViewAsset() = default;

		int32_t getWidth() const;
		int32_t getHeight() const;

		virtual void serialize(ISerializer& s) override final;

	private:
		friend class TurboBadgerUiPipeline;

		int32_t m_width = 0;
		int32_t m_height = 0;
	};
}