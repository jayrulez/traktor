/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/MarkupButton.h"
#include "Shine/RenderContext.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.MarkupButton", MarkupButton, Interactable)

MarkupButton::MarkupButton(const std::wstring& name)
:	Interactable(name)
{
	createDefaultWidgets();
	updateTextColor();
}

MarkupButton::~MarkupButton()
{
}

void MarkupButton::createDefaultWidgets()
{
	m_textWidget = new Text(L"Text");
	addChild(m_textWidget);
}

const std::wstring& MarkupButton::getText() const
{
	static const std::wstring empty;
	return m_textWidget ? m_textWidget->getText() : empty;
}

void MarkupButton::setText(const std::wstring& text)
{
	if (m_textWidget)
		m_textWidget->setText(text);
}

void MarkupButton::setLinkColor(const Color4f& color)
{
	m_linkColor = color;
	updateTextColor();
}

bool MarkupButton::onMouseButtonDown(MouseButton button, const Vector2& position)
{
	if (button == MouseButton::Left)
	{
		m_isPressed = true;
		updateTextColor();
		return true;
	}

	return false;
}

bool MarkupButton::onMouseButtonUp(MouseButton button, const Vector2& position)
{
	if (button == MouseButton::Left && m_isPressed)
	{
		m_isPressed = false;
		updateTextColor();

		// Trigger click events
		if (onClick)
			onClick();

		if (onLinkClick && !m_link.empty())
			onLinkClick(m_link);

		return true;
	}

	return false;
}

void MarkupButton::onMouseEnter()
{
	m_isHovered = true;
	updateTextColor();
}

void MarkupButton::onMouseLeave()
{
	m_isHovered = false;
	m_isPressed = false;
	updateTextColor();
}

void MarkupButton::render(RenderContext* rc, float parentOpacity)
{
	// Render children (including text widget)
	Interactable::render(rc, parentOpacity);

	// Render underline if enabled
	if (m_underlined && m_textWidget && !m_textWidget->getText().empty())
	{
		Vector2 worldPos = getWorldPosition();
		Vector2 size = getSize();

		// Determine underline color based on state
		Color4f underlineColor = m_linkColor;
		if (m_isPressed)
			underlineColor = m_pressedColor;
		else if (m_isHovered)
			underlineColor = m_hoverColor;

		underlineColor.setAlpha(Scalar(underlineColor.getAlpha() * getOpacity() * parentOpacity));

		// Draw underline
		float underlineY = worldPos.y + size.y - 2.0f;
		rc->drawLine(
			Vector2(worldPos.x, underlineY),
			Vector2(worldPos.x + size.x, underlineY),
			underlineColor,
			1.0f
		);
	}
}

void MarkupButton::updateTextColor()
{
	if (!m_textWidget)
		return;

	if (m_isPressed)
		m_textWidget->setColor(m_pressedColor);
	else if (m_isHovered)
		m_textWidget->setColor(m_hoverColor);
	else
		m_textWidget->setColor(m_linkColor);
}

}
