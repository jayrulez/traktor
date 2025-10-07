/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/DropdownOption.h"
#include "Shine/Widgets/Dropdown.h"
#include "Shine/Widgets/Image.h"
#include "Shine/Widgets/Text.h"
#include "Shine/RenderContext.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.DropdownOption", DropdownOption, Interactable)

DropdownOption::DropdownOption(const std::wstring& name)
:	Interactable(name)
{
	createDefaultWidgets();
	updateVisualState();
}

DropdownOption::~DropdownOption()
{
}

const std::wstring& DropdownOption::getText() const
{
	if (m_textWidget)
		return m_textWidget->getText();

	static std::wstring empty;
	return empty;
}

void DropdownOption::setText(const std::wstring& text)
{
	if (m_textWidget)
		m_textWidget->setText(text);
}

bool DropdownOption::onMouseButtonDown(MouseButton button, const Vector2& position)
{
	if (!Interactable::onMouseButtonDown(button, position))
		return false;

	if (button == MouseButton::Left)
	{
		// Notify dropdown of selection
		if (m_dropdown)
		{
			m_dropdown->setSelectedIndex(m_optionIndex);
			m_dropdown->close();
		}

		return true;
	}

	return false;
}

void DropdownOption::onInteractionStateChanged()
{
	updateVisualState();
}

void DropdownOption::render(RenderContext* rc, float parentOpacity)
{
	// Let base class handle rendering (will render children)
	Interactable::render(rc, parentOpacity);
}

void DropdownOption::createDefaultWidgets()
{
	Vector2 size = getSize();
	if (size.x < 20) size.x = 180;
	if (size.y < 20) size.y = 25;
	setSize(size);

	// Create background
	Ref<Image> background = new Image(L"OptionBackground");
	background->setPosition(Vector2(0, 0));
	background->setSize(size);
	background->setColor(Color4f(0.2f, 0.2f, 0.2f, 1));
	addChild(background);
	m_backgroundImage = background;

	// Background anchoring
	background->setAnchorMin(Vector2(0, 0));
	background->setAnchorMax(Vector2(1, 1));
	background->setOffsetMin(Vector2(0, 0));
	background->setOffsetMax(Vector2(0, 0));

	// Create text
	Ref<Text> text = new Text(L"OptionText");
	text->setPosition(Vector2(5, 0));
	text->setSize(Vector2(size.x - 10, size.y));
	text->setText(L"Option");
	text->setHorizontalAlign(Text::HAlign::Left);
	text->setVerticalAlign(Text::VAlign::Middle);
	text->setColor(Color4f(1, 1, 1, 1));
	addChild(text);
	m_textWidget = text;

	// Text anchoring
	text->setAnchorMin(Vector2(0, 0));
	text->setAnchorMax(Vector2(1, 1));
	text->setOffsetMin(Vector2(5, 0));
	text->setOffsetMax(Vector2(-5, 0));
}

void DropdownOption::updateVisualState()
{
	if (!m_backgroundImage)
		return;

	// Update background color based on state
	if (isHovered())
		m_backgroundImage->setColor(Color4f(0.4f, 0.4f, 0.4f, 1));
	else
		m_backgroundImage->setColor(Color4f(0.2f, 0.2f, 0.2f, 1));
}

}
