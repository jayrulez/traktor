/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Shine/Widgets/Image.h"

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
 * \brief Cursor widget for custom UI cursor display.
 *
 * Cursor displays a custom cursor image that follows the mouse position
 * or gamepad-controlled position. Supports multiple cursor states (normal,
 * hover, click) with different visuals.
 *
 * Usage:
 *   Ref<Cursor> cursor = new Cursor(L"CustomCursor");
 *   cursor->setNormalImage(normalTexture);
 *   cursor->setHoverImage(hoverTexture);
 *   cursor->setClickImage(clickTexture);
 *   cursor->setOffset(Vector2(-16, -16)); // Hotspot offset
 *   canvas->addChild(cursor);
 */
class T_DLLCLASS Cursor : public Image
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Cursor state
	 */
	enum class State
	{
		Normal,  // Default cursor
		Hover,   // Over interactable element
		Click,   // Mouse button pressed
		Disabled // Cursor disabled
	};

	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit Cursor(const std::wstring& name = L"Cursor");

	/*!
	 * \brief Destructor
	 */
	virtual ~Cursor();

	// === Cursor State ===

	/*!
	 * \brief Get cursor state
	 */
	State getCursorState() const { return m_cursorState; }

	/*!
	 * \brief Set cursor state
	 */
	void setCursorState(State state);

	// === Cursor Images ===

	/*!
	 * \brief Get normal state image
	 */
	render::ITexture* getNormalImage() const { return m_normalImage; }

	/*!
	 * \brief Set normal state image
	 */
	void setNormalImage(render::ITexture* texture);

	/*!
	 * \brief Get hover state image
	 */
	render::ITexture* getHoverImage() const { return m_hoverImage; }

	/*!
	 * \brief Set hover state image
	 */
	void setHoverImage(render::ITexture* texture);

	/*!
	 * \brief Get click state image
	 */
	render::ITexture* getClickImage() const { return m_clickImage; }

	/*!
	 * \brief Set click state image
	 */
	void setClickImage(render::ITexture* texture);

	// === Cursor Position ===

	/*!
	 * \brief Get cursor position offset (hotspot)
	 */
	Vector2 getOffset() const { return m_offset; }

	/*!
	 * \brief Set cursor position offset
	 */
	void setOffset(const Vector2& offset) { m_offset = offset; }

	/*!
	 * \brief Update cursor position to mouse position
	 */
	void updateToMousePosition(const Vector2& mousePosition);

	// === Visibility ===

	/*!
	 * \brief Get whether hardware cursor is hidden
	 */
	bool getHideHardwareCursor() const { return m_hideHardwareCursor; }

	/*!
	 * \brief Set whether to hide hardware cursor
	 */
	void setHideHardwareCursor(bool hide) { m_hideHardwareCursor = hide; }

	// === Events ===

	std::function<void(State)> onStateChanged;

private:
	State m_cursorState = State::Normal;
	Vector2 m_offset = Vector2(0.0f, 0.0f);
	bool m_hideHardwareCursor = true;

	// State images
	Ref<render::ITexture> m_normalImage;
	Ref<render::ITexture> m_hoverImage;
	Ref<render::ITexture> m_clickImage;

	// Helper methods
	void updateCursorImage();
};

}
