/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/Button.h"
#include "Shine/Widgets/Image.h"
#include "Shine/Widgets/Text.h"
#include "Shine/RenderContext.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Button", Button, Interactable)

Button::Button(const std::wstring& name)
:	Interactable(name)
{
	createDefaultWidgets();
	updateVisualState();
}

Button::~Button()
{
}

const std::wstring& Button::getText() const
{
	if (m_textWidget)
		return m_textWidget->getText();

	static std::wstring empty;
	return empty;
}

void Button::setText(const std::wstring& text)
{
	if (m_textWidget)
		m_textWidget->setText(text);
}

void Button::onInteractionStateChanged()
{
	updateVisualState();
}

void Button::render(RenderContext* rc, float parentOpacity)
{
	// Let base class handle rendering (will render children)
	Interactable::render(rc, parentOpacity);
}

void Button::updateVisualState()
{
	if (!m_backgroundImage)
		return;

	// Update background color based on state
	Color4f targetColor = m_normalColor;

	if (!isEnabled())
	{
		targetColor = m_disabledColor;
	}
	else if (isPressed())
	{
		targetColor = m_pressedColor;
	}
	else if (isHovered())
	{
		targetColor = m_hoveredColor;
	}

	m_backgroundImage->setColor(targetColor);
}

void Button::createDefaultWidgets()
{
	// Create background image
	Ref<Image> background = new Image(L"ButtonBackground");
	background->setPosition(Vector2(0, 0));
	background->setSize(getSize());
	background->setColor(m_normalColor);
	// No texture - will render as colored rectangle (via Panel-like behavior)
	addChild(background);
	m_backgroundImage = background;

	// Background anchoring - stretch to fill button
	background->setAnchorMin(Vector2(0, 0));
	background->setAnchorMax(Vector2(1, 1));
	background->setOffsetMin(Vector2(0, 0));
	background->setOffsetMax(Vector2(0, 0));

	// Create text label
	Ref<Text> text = new Text(L"ButtonText");
	text->setPosition(Vector2(0, 0));
	text->setSize(getSize());
	text->setText(L"Button");
	text->setHorizontalAlign(Text::HAlign::Center);
	text->setVerticalAlign(Text::VAlign::Middle);
	text->setColor(Color4f(0, 0, 0, 1)); // Black text
	addChild(text);
	m_textWidget = text;

	// Text anchoring - stretch to fill button
	text->setAnchorMin(Vector2(0, 0));
	text->setAnchorMax(Vector2(1, 1));
	text->setOffsetMin(Vector2(0, 0));
	text->setOffsetMax(Vector2(0, 0));
}

}
