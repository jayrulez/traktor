/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Shine/Visual.h"
#include "Core/Timer/Timer.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

/*!
 * \brief Mouse button enumeration
 */
enum class MouseButton
{
	Left = 0,
	Right = 1,
	Middle = 2
};

/*!
 * \brief Base class for all interactive UI elements that handle input.
 *
 * Interactable extends Visual with input handling capabilities:
 * - Mouse events (hover, click, drag)
 * - Keyboard events (when focused)
 * - Interaction state (hovered, pressed, focused, disabled)
 * - Hit testing
 *
 * This is an abstract base class. Use derived classes like Button, Slider, etc.
 */
class T_DLLCLASS Interactable : public Visual
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit Interactable(const std::wstring& name = L"");

	/*!
	 * \brief Destructor
	 */
	virtual ~Interactable();

	// === Input Handling ===

	/*!
	 * \brief Handle mouse button press
	 * \param button Mouse button
	 * \param position Mouse position in world space
	 * \return true if event was handled
	 */
	virtual bool onMouseButtonDown(MouseButton button, const Vector2& position);

	/*!
	 * \brief Handle mouse button release
	 * \param button Mouse button
	 * \param position Mouse position in world space
	 * \return true if event was handled
	 */
	virtual bool onMouseButtonUp(MouseButton button, const Vector2& position);

	/*!
	 * \brief Handle mouse move
	 * \param position Mouse position in world space
	 * \return true if event was handled
	 */
	virtual bool onMouseMove(const Vector2& position);

	/*!
	 * \brief Handle mouse enter (cursor enters bounds)
	 */
	virtual void onMouseEnter();

	/*!
	 * \brief Handle mouse leave (cursor exits bounds)
	 */
	virtual void onMouseLeave();

	/*!
	 * \brief Handle mouse wheel scroll
	 * \param delta Scroll delta (positive = up, negative = down)
	 * \return true if event was handled
	 */
	virtual bool onMouseWheel(float delta);

	/*!
	 * \brief Handle key press
	 * \param key Key code
	 * \return true if event was handled
	 */
	virtual bool onKeyDown(int key);

	/*!
	 * \brief Handle key release
	 * \param key Key code
	 * \return true if event was handled
	 */
	virtual bool onKeyUp(int key);

	/*!
	 * \brief Handle text input (character typed)
	 * \param character Unicode character
	 * \return true if event was handled
	 */
	virtual bool onTextInput(wchar_t character);

	/*!
	 * \brief Handle focus gained
	 */
	virtual void onFocusGained();

	/*!
	 * \brief Handle focus lost
	 */
	virtual void onFocusLost();

	// === Hit Testing ===

	/*!
	 * \brief Test if point is inside this element
	 * \param worldPosition Point in world space
	 * \return true if point is inside
	 */
	virtual bool hitTest(const Vector2& worldPosition) const;

	// === Interaction State ===

	/*!
	 * \brief Get whether element is currently hovered
	 */
	bool isHovered() const { return m_hovered; }

	/*!
	 * \brief Get whether element is currently pressed
	 */
	bool isPressed() const { return m_pressed; }

	/*!
	 * \brief Get whether element has focus
	 */
	bool isFocused() const { return m_focused; }

	/*!
	 * \brief Get whether element is interactable (not disabled)
	 */
	bool isInteractable() const;

	/*!
	 * \brief Set focus to this element
	 */
	void focus();

	/*!
	 * \brief Remove focus from this element
	 */
	void unfocus();

	// === Events ===

	std::function<void()> onClick;
	std::function<void()> onDoubleClick;
	std::function<void()> onHoverStart;
	std::function<void()> onHoverEnd;
	std::function<void()> onPressStart;
	std::function<void()> onPressEnd;
	std::function<void()> onFocusChanged;
	std::function<void(const Vector2&)> onDragStart;
	std::function<void(const Vector2&)> onDrag;
	std::function<void(const Vector2&)> onDragEnd;

protected:
	/*!
	 * \brief Set hovered state (internal use)
	 */
	void setHovered(bool hovered);

	/*!
	 * \brief Set pressed state (internal use)
	 */
	void setPressed(bool pressed);

	/*!
	 * \brief Set focused state (internal use)
	 */
	void setFocused(bool focused);

	/*!
	 * \brief Called when interaction state changes
	 */
	virtual void onInteractionStateChanged();

private:
	bool m_hovered = false;
	bool m_pressed = false;
	bool m_focused = false;

	// Double-click detection
	Timer m_clickTimer;
	double m_lastClickTime = 0.0;
	const double m_doubleClickThreshold = 0.3; // 300ms

	// Drag detection
	bool m_dragging = false;
	Vector2 m_dragStartPosition;
	const float m_dragThreshold = 5.0f; // 5 pixels
};

}
