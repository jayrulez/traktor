/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

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

/*! ScrollBar orientation. */
enum class ScrollBarOrientation
{
	Vertical,
	Horizontal
};

/*! ScrollBar control.
 * \ingroup Paper
 *
 * A scrollbar control with a draggable thumb and clickable track.
 * Can be oriented vertically or horizontally.
 */
class T_DLLCLASS ScrollBar : public UIElement
{
	T_RTTI_CLASS;

public:
	ScrollBar();

	void setOrientation(ScrollBarOrientation orientation) { m_orientation = orientation; }

	ScrollBarOrientation getOrientation() const { return m_orientation; }

	/*! Set the scrollbar value (0.0 to 1.0). */
	void setValue(float value);

	float getValue() const { return m_value; }

	/*! Set the viewport size relative to content size (0.0 to 1.0).
	 * This determines the thumb size.
	 */
	void setViewportSize(float viewportSize);

	float getViewportSize() const { return m_viewportSize; }

	/*! Set callback when value changes. */
	void setValueChangedCallback(std::function<void(float)> callback) { m_valueChangedCallback = callback; }

	// Styling
	void setTrackColor(const Color4f& color) { m_trackColor = color; }
	void setThumbColor(const Color4f& color) { m_thumbColor = color; }
	void setThumbHoverColor(const Color4f& color) { m_thumbHoverColor = color; }
	void setThumbPressedColor(const Color4f& color) { m_thumbPressedColor = color; }

	virtual void applyStyle(const UIStyle* style) override;

	virtual Vector2 measure(const Vector2& availableSize, UIContext* context) override;

	virtual void arrange(const Vector2& position, const Vector2& size) override;

	virtual void render(UIContext* context) override;

	virtual void onMouseDown(MouseEvent& event) override;

	virtual void onMouseMove(MouseEvent& event) override;

	virtual void onMouseUp(MouseEvent& event) override;

	virtual void onMouseEnter(MouseEvent& event) override;

	virtual void onMouseLeave(MouseEvent& event) override;

	virtual void serialize(ISerializer& s) override;

private:
	ScrollBarOrientation m_orientation = ScrollBarOrientation::Vertical;
	float m_value = 0.0f;              // Current scroll position (0.0 to 1.0)
	float m_viewportSize = 0.2f;       // Size of viewport relative to content (0.0 to 1.0)

	// Styling
	Color4f m_trackColor = Color4f(0.15f, 0.15f, 0.15f, 1.0f);
	Color4f m_thumbColor = Color4f(0.4f, 0.4f, 0.4f, 1.0f);
	Color4f m_thumbHoverColor = Color4f(0.5f, 0.5f, 0.5f, 1.0f);
	Color4f m_thumbPressedColor = Color4f(0.6f, 0.6f, 0.6f, 1.0f);

	// Interaction state
	bool m_isDragging = false;
	bool m_isThumbHovered = false;
	Vector2 m_dragStartPos;
	float m_dragStartValue = 0.0f;

	// Callback
	std::function<void(float)> m_valueChangedCallback;

	// Helper methods
	Vector2 getThumbPosition() const;
	Vector2 getThumbSize() const;
	bool isPointInThumb(const Vector2& point) const;
	float getValueFromPosition(const Vector2& position) const;
};

}
