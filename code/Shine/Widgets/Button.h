/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Shine/Interactable.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

class Image;
class Text;

/*!
 * \brief Button widget for clickable buttons.
 *
 * Button is a composite widget containing:
 * - Background image (optional)
 * - Text label (optional)
 * - Visual state transitions (normal, hovered, pressed, disabled)
 *
 * Usage:
 *   Ref<Button> button = new Button(L"PlayButton");
 *   button->setText(L"Play");
 *   button->onClick = []() { startGame(); };
 */
class T_DLLCLASS Button : public Interactable
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit Button(const std::wstring& name = L"Button");

	/*!
	 * \brief Destructor
	 */
	virtual ~Button();

	// === Text ===

	/*!
	 * \brief Get button text
	 */
	const std::wstring& getText() const;

	/*!
	 * \brief Set button text
	 */
	void setText(const std::wstring& text);

	/*!
	 * \brief Get text widget (for advanced customization)
	 */
	Text* getTextWidget() const { return m_textWidget; }

	// === Background ===

	/*!
	 * \brief Get background image widget
	 */
	Image* getBackgroundImage() const { return m_backgroundImage; }

	// === State Colors ===

	/*!
	 * \brief Get normal state color
	 */
	Color4f getNormalColor() const { return m_normalColor; }

	/*!
	 * \brief Set normal state color
	 */
	void setNormalColor(const Color4f& color) { m_normalColor = color; updateVisualState(); }

	/*!
	 * \brief Get hovered state color
	 */
	Color4f getHoveredColor() const { return m_hoveredColor; }

	/*!
	 * \brief Set hovered state color
	 */
	void setHoveredColor(const Color4f& color) { m_hoveredColor = color; updateVisualState(); }

	/*!
	 * \brief Get pressed state color
	 */
	Color4f getPressedColor() const { return m_pressedColor; }

	/*!
	 * \brief Set pressed state color
	 */
	void setPressedColor(const Color4f& color) { m_pressedColor = color; updateVisualState(); }

	/*!
	 * \brief Get disabled state color
	 */
	Color4f getDisabledColor() const { return m_disabledColor; }

	/*!
	 * \brief Set disabled state color
	 */
	void setDisabledColor(const Color4f& color) { m_disabledColor = color; updateVisualState(); }

	// === Interaction ===

	virtual void onInteractionStateChanged() override;

	virtual void render(RenderContext* rc, float parentOpacity) override;

private:
	// Child widgets
	Image* m_backgroundImage = nullptr;
	Text* m_textWidget = nullptr;

	// State colors
	Color4f m_normalColor = Color4f(0.8f, 0.8f, 0.8f, 1.0f);
	Color4f m_hoveredColor = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
	Color4f m_pressedColor = Color4f(0.6f, 0.6f, 0.6f, 1.0f);
	Color4f m_disabledColor = Color4f(0.5f, 0.5f, 0.5f, 0.5f);

	// Helper to update visual state based on interaction state
	void updateVisualState();

	// Helper to create default child widgets
	void createDefaultWidgets();
};

}
