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
#include "Core/Ref.h"
#include "Core/Guid.h"
#include "Core/Math/Color4f.h"
#include "Paper/Ui/UIElement.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_PAPER_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::paper
{

class UIContext;
class IFont;
class IFontRenderer;

/*! Single-line text input control.
 * \ingroup Paper
 *
 * TextBox allows the user to enter and edit text.
 * Supports cursor positioning, text selection, and basic editing operations.
 */
class T_DLLCLASS TextBox : public UIElement
{
	T_RTTI_CLASS;

public:
	TextBox();

	void setText(const std::wstring& text);

	const std::wstring& getText() const { return m_text; }

	void setFontId(const Guid& fontId) { m_fontId = fontId; }

	const Guid& getFontId() const { return m_fontId; }

	void setForeground(const Color4f& color) { m_foreground = color; }

	const Color4f& getForeground() const { return m_foreground; }

	void setBackground(const Color4f& color) { m_background = color; }

	const Color4f& getBackground() const { return m_background; }

	void setBorderBrush(const Color4f& color) { m_borderBrush = color; }

	const Color4f& getBorderBrush() const { return m_borderBrush; }

	void setBorderThickness(float thickness) { m_borderThickness = thickness; }

	float getBorderThickness() const { return m_borderThickness; }

	void setPadding(const Vector2& padding) { m_padding = padding; }

	const Vector2& getPadding() const { return m_padding; }

	void setPlaceholder(const std::wstring& placeholder) { m_placeholder = placeholder; }

	const std::wstring& getPlaceholder() const { return m_placeholder; }

	virtual void applyStyle(const UIStyle* style) override;

	virtual Vector2 measure(const Vector2& availableSize, UIContext* context) override;

	virtual void arrange(const Vector2& position, const Vector2& size) override;

	virtual void render(UIContext* context) override;

	virtual void renderDebug(UIContext* context) override;

	virtual void onMouseDown(MouseEvent& event) override;

	virtual void onMouseMove(MouseEvent& event) override;

	virtual void onMouseUp(MouseEvent& event) override;

	virtual void onKeyDown(KeyEvent& event) override;

	virtual void serialize(ISerializer& s) override;

private:
	std::wstring m_text;
	std::wstring m_placeholder;
	Guid m_fontId;
	Color4f m_foreground = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
	Color4f m_background = Color4f(0.1f, 0.1f, 0.1f, 1.0f);
	Color4f m_borderBrush = Color4f(0.3f, 0.3f, 0.3f, 1.0f);
	Color4f m_borderFocusBrush = Color4f(0.3f, 0.8f, 1.0f, 1.0f);
	float m_borderThickness = 1.0f;
	Vector2 m_padding = Vector2(8.0f, 4.0f);

	// Edit state
	int32_t m_cursorPosition = 0;     // Cursor position in text
	int32_t m_selectionStart = -1;    // Selection start (-1 = no selection)
	int32_t m_selectionEnd = -1;      // Selection end
	bool m_isDragging = false;        // True when dragging to select text
	double m_cursorBlinkTime = 0.0;   // For blinking cursor animation

	// Helper methods
	int32_t getCharacterIndexAtPosition(const Vector2& localPosition);
	void setCursorPosition(int32_t position);
	void clearSelection();
	bool hasSelection() const;

	// Cached for cursor positioning (raw pointers to avoid Ref template issues with forward declarations)
	mutable const IFont* m_cachedFont = nullptr;
	mutable IFontRenderer* m_cachedFontRenderer = nullptr;
};

}
