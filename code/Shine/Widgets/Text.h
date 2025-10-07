/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Guid.h"
#include "Shine/Visual.h"
#include <string>

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

class FontManager;
class IFont;

/*!
 * \brief Text display widget.
 *
 * Text widget displays text with support for:
 * - Font size and color
 * - Horizontal and vertical alignment
 * - Text wrapping
 * - Overflow handling
 * - Auto-sizing
 */
class T_DLLCLASS Text : public Visual
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Horizontal text alignment
	 */
	enum class HAlign
	{
		Left,
		Center,
		Right
	};

	/*!
	 * \brief Vertical text alignment
	 */
	enum class VAlign
	{
		Top,
		Middle,
		Bottom
	};

	/*!
	 * \brief Text overflow mode
	 */
	enum class OverflowMode
	{
		Overflow,      // Text extends beyond bounds
		Clip,          // Clip to bounds
		Ellipsis       // Show "..." when clipped
	};

	/*!
	 * \brief Text wrapping mode
	 */
	enum class WrapMode
	{
		NoWrap,        // Single line
		WordWrap,      // Wrap at word boundaries
		CharacterWrap  // Wrap at any character
	};

	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit Text(const std::wstring& name = L"Text");

	/*!
	 * \brief Destructor
	 */
	virtual ~Text();

	// === Text Content ===

	/*!
	 * \brief Get text content
	 */
	const std::wstring& getText() const { return m_text; }

	/*!
	 * \brief Set text content
	 */
	void setText(const std::wstring& text);

	// === Font Properties ===

	/*!
	 * \brief Get font size in points
	 */
	float getFontSize() const { return m_fontSize; }

	/*!
	 * \brief Set font size
	 */
	void setFontSize(float size);

	/*!
	 * \brief Get text color (uses Visual::getColor())
	 */
	Color4f getTextColor() const { return getColor(); }

	/*!
	 * \brief Set text color
	 */
	void setTextColor(const Color4f& color) { setColor(color); }

	// === Alignment ===

	/*!
	 * \brief Get horizontal alignment
	 */
	HAlign getHorizontalAlign() const { return m_hAlign; }

	/*!
	 * \brief Set horizontal alignment
	 */
	void setHorizontalAlign(HAlign align);

	/*!
	 * \brief Get vertical alignment
	 */
	VAlign getVerticalAlign() const { return m_vAlign; }

	/*!
	 * \brief Set vertical alignment
	 */
	void setVerticalAlign(VAlign align);

	// === Wrapping and Overflow ===

	/*!
	 * \brief Get wrap mode
	 */
	WrapMode getWrapMode() const { return m_wrapMode; }

	/*!
	 * \brief Set wrap mode
	 */
	void setWrapMode(WrapMode mode);

	/*!
	 * \brief Get overflow mode
	 */
	OverflowMode getOverflowMode() const { return m_overflowMode; }

	/*!
	 * \brief Set overflow mode
	 */
	void setOverflowMode(OverflowMode mode);

	// === Auto-sizing ===

	/*!
	 * \brief Get whether to auto-size to fit text
	 */
	bool getAutoSize() const { return m_autoSize; }

	/*!
	 * \brief Set auto-size mode
	 */
	void setAutoSize(bool autoSize);

	/*!
	 * \brief Calculate text size
	 */
	Vector2 calculateTextSize() const;

	// === Font System ===

	/*!
	 * \brief Set font manager
	 */
	void setFontManager(FontManager* manager);

	/*!
	 * \brief Set font resource
	 * \param fontId Font resource GUID
	 */
	void setFontResource(const Guid& fontId);

	/*!
	 * \brief Get font resource ID
	 */
	const Guid& getFontResource() const { return m_fontId; }

	// === Rendering ===

	virtual void render(RenderContext* rc, float parentOpacity) override;

private:
	std::wstring m_text;
	float m_fontSize = 16.0f;
	HAlign m_hAlign = HAlign::Left;
	VAlign m_vAlign = VAlign::Top;
	WrapMode m_wrapMode = WrapMode::NoWrap;
	OverflowMode m_overflowMode = OverflowMode::Overflow;
	bool m_autoSize = false;

	// Font system
	Guid m_fontId;                      // Font resource ID
	Ref<IFont> m_font;                  // Loaded font
	Ref<FontManager> m_fontManager;     // Font manager (injected)

	// Cached text size (updated when text changes)
	mutable bool m_textSizeDirty = true;
	mutable Vector2 m_cachedTextSize;

	// Helper to update cached text size
	void updateTextSize() const;

	// Helper to calculate aligned position
	Vector2 calculateAlignedPosition(const Vector2& textSize, const Vector2& bounds) const;
};

}
