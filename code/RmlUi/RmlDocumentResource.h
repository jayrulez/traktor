/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Serialization/ISerializable.h"
#include "Core/Io/Path.h"

 // import/export mechanism.
#undef T_DLLCLASS
#if defined(T_RMLUI_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::rmlui
{
	/*! RmlDocumentResource
	 * \ingroup RmlUi
	 */
	class T_DLLCLASS RmlDocumentResource : public ISerializable
	{
		T_RTTI_CLASS;

	public:
		RmlDocumentResource() = default;

		RmlDocumentResource(const Path& filePath, 
			const AlignedVector<Path>& fonts, 
			const AlignedVector<Path>& fallbackFonts, 
			int32_t width, 
			int32_t height);

		const Path& getFilePath() const;
		const AlignedVector<Path>& getFonts() const;
		const AlignedVector<Path>& getFallbackFonts() const;
		int32_t getWidth() const;
		int32_t getHeight() const;

		virtual void serialize(ISerializer& s) override final;

	private:
		Path m_rmlFilePath = {};
		AlignedVector<Path> m_fontFilePaths = {};
		AlignedVector<Path> m_fallbackFontFilePaths = {};
		int32_t m_referenceWidth = 0;
		int32_t m_referenceHeight = 0;
	};

}