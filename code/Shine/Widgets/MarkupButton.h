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
#include "Shine/Widgets/Text.h"

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
 * \brief MarkupButton widget for clickable text links.
 *
 * MarkupButton is a text element that can be clicked like a hyperlink,
 * useful for in-game links, help text, credits, etc.
 *
 * Usage:
 *   Ref<MarkupButton> link = new MarkupButton(L"HelpLink");
 *   link->setText(L"Click here for help");
 *   link->setLinkColor(Color4f(0.3f, 0.5f, 1.0f, 1.0f));
 *   link->onClick = []() { showHelp(); };
 */
class T_DLLCLASS MarkupButton : public Interactable
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit MarkupButton(const std::wstring& name = L"MarkupButton");

	/*!
	 * \brief Destructor
	 */
	virtual ~MarkupButton();

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

	// === Link Styling ===

	/*!
	 * \brief Get normal link color
	 */
	Color4f getLinkColor() const { return m_linkColor; }

	/*!
	 * \brief Set normal link color
	 */
	void setLinkColor(const Color4f& color);

	/*!
	 * \brief Get hover link color
	 */
	Color4f getHoverColor() const { return m_hoverColor; }

	/*!
	 * \brief Set hover link color
	 */
	void setHoverColor(const Color4f& color) { m_hoverColor = color; }

	/*!
	 * \brief Get pressed link color
	 */
	Color4f getPressedColor() const { return m_pressedColor; }

	/*!
	 * \brief Set pressed link color
	 */
	void setPressedColor(const Color4f& color) { m_pressedColor = color; }

	// === Underline ===

	/*!
	 * \brief Get whether link is underlined
	 */
	bool getUnderlined() const { return m_underlined; }

	/*!
	 * \brief Set whether link is underlined
	 */
	void setUnderlined(bool underlined) { m_underlined = underlined; }

	// === Link Data ===

	/*!
	 * \brief Get link URL or identifier
	 */
	const std::wstring& getLink() const { return m_link; }

	/*!
	 * \brief Set link URL or identifier
	 */
	void setLink(const std::wstring& link) { m_link = link; }

	// === Events ===

	std::function<void()> onClick;
	std::function<void(const std::wstring&)> onLinkClick;

	// === Input ===

	virtual bool onMouseButtonDown(MouseButton button, const Vector2& position) override;
	virtual bool onMouseButtonUp(MouseButton button, const Vector2& position) override;
	virtual void onMouseEnter() override;
	virtual void onMouseLeave() override;

	// === Render ===

	virtual void render(RenderContext* rc, float parentOpacity) override;

private:
	// Child widgets
	Text* m_textWidget = nullptr;

	Color4f m_linkColor = Color4f(0.3f, 0.5f, 1.0f, 1.0f);
	Color4f m_hoverColor = Color4f(0.5f, 0.7f, 1.0f, 1.0f);
	Color4f m_pressedColor = Color4f(0.2f, 0.4f, 0.9f, 1.0f);
	bool m_underlined = true;
	std::wstring m_link;

	// State
	bool m_isHovered = false;
	bool m_isPressed = false;

	// Helper methods
	void createDefaultWidgets();
	void updateTextColor();
};

}
