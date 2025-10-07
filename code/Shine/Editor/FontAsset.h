/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Serialization/ISerializable.h"
#include "Editor/Asset.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EDITOR_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

/*!
 * \brief Editor font asset
 * \ingroup Shine
 *
 * Source asset for font creation. References a TrueType/OpenType font file
 * and specifies rasterization settings.
 *
 * The FontPipeline converts this asset to a runtime Font resource by:
 * 1. Loading the font file with FreeType
 * 2. Rasterizing glyphs to a texture atlas
 * 3. Extracting metrics and kerning data
 * 4. Creating optimized Font resource
 */
class T_DLLCLASS FontAsset : public editor::Asset
{
	T_RTTI_CLASS;

public:
	FontAsset() = default;

	// === Font File ===

	/*!
	 * \brief Set path to font file (TTF, OTF)
	 */
	void setFontFile(const Path& fontFile) { m_fontFile = fontFile; }

	/*!
	 * \brief Get path to font file
	 */
	const Path& getFontFile() const { return m_fontFile; }

	// === Rasterization Settings ===

	/*!
	 * \brief Set font size to rasterize at
	 */
	void setFontSize(float size) { m_fontSize = size; }

	/*!
	 * \brief Get font size
	 */
	float getFontSize() const { return m_fontSize; }

	/*!
	 * \brief Set character set to include
	 *
	 * If empty, includes default set (ASCII + common Unicode).
	 * Otherwise, only includes specified characters.
	 */
	void setCharacterSet(const std::wstring& chars) { m_characterSet = chars; }

	/*!
	 * \brief Get character set
	 */
	const std::wstring& getCharacterSet() const { return m_characterSet; }

	// === Atlas Settings ===

	/*!
	 * \brief Set atlas texture size (width and height)
	 */
	void setAtlasSize(int32_t size) { m_atlasSize = size; }

	/*!
	 * \brief Get atlas size
	 */
	int32_t getAtlasSize() const { return m_atlasSize; }

	/*!
	 * \brief Set atlas padding (pixels between glyphs)
	 */
	void setAtlasPadding(int32_t padding) { m_atlasPadding = padding; }

	/*!
	 * \brief Get atlas padding
	 */
	int32_t getAtlasPadding() const { return m_atlasPadding; }

	// === Advanced Settings ===

	/*!
	 * \brief Enable signed distance field rendering
	 *
	 * SDF allows high-quality scaling and effects but requires
	 * more processing during rasterization.
	 */
	void setEnableSDF(bool enable) { m_enableSDF = enable; }

	/*!
	 * \brief Get SDF enabled flag
	 */
	bool getEnableSDF() const { return m_enableSDF; }

	/*!
	 * \brief Set SDF spread (pixels)
	 *
	 * Only used if SDF is enabled.
	 */
	void setSDFSpread(int32_t spread) { m_sdfSpread = spread; }

	/*!
	 * \brief Get SDF spread
	 */
	int32_t getSDFSpread() const { return m_sdfSpread; }

	/*!
	 * \brief Enable antialiasing
	 */
	void setEnableAntialiasing(bool enable) { m_enableAntialiasing = enable; }

	/*!
	 * \brief Get antialiasing enabled flag
	 */
	bool getEnableAntialiasing() const { return m_enableAntialiasing; }

	/*!
	 * \brief Enable kerning extraction
	 */
	void setEnableKerning(bool enable) { m_enableKerning = enable; }

	/*!
	 * \brief Get kerning enabled flag
	 */
	bool getEnableKerning() const { return m_enableKerning; }

	// === Serialization ===

	virtual void serialize(ISerializer& s) override final;

private:
	Path m_fontFile;                          // Path to TTF/OTF file
	float m_fontSize = 32.0f;                 // Size to rasterize at
	std::wstring m_characterSet;              // Characters to include (empty = default)

	int32_t m_atlasSize = 1024;               // Atlas texture size (square)
	int32_t m_atlasPadding = 2;               // Padding between glyphs

	bool m_enableSDF = false;                 // Signed distance field rendering
	int32_t m_sdfSpread = 8;                  // SDF spread in pixels

	bool m_enableAntialiasing = true;         // Antialias glyphs
	bool m_enableKerning = true;              // Extract kerning pairs
};

}
