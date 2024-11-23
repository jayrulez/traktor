/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "TurboBadgerUi/Types.h"

#include "Core/Serialization/ISerializable.h"
#include "Core/Io/Path.h"

 // import/export mechanism.
#undef T_DLLCLASS
#if defined(T_TURBOBADGERUI_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::turbobadgerui
{
	/*! TurboBadgerUiViewResource
	* \ingroup TurboBadgerUi
	*/
	class T_DLLCLASS TurboBadgerUiViewResource : public ISerializable
	{
		T_RTTI_CLASS;

	public:
		TurboBadgerUiViewResource() = default;

		TurboBadgerUiViewResource(const Path& filePath);

		const Path& getFilePath() const;

		int32_t getWidth() const;

		int32_t getHeight() const;

		virtual void serialize(ISerializer& s) override final;

	private:
		Path m_viewFilePath = {};
		int32_t m_width = 0;
		int32_t m_height = 0;
	};
}