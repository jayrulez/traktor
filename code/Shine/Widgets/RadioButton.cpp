/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/RadioButton.h"
#include "Shine/Widgets/Image.h"
#include "Shine/Widgets/Text.h"
#include "Shine/RenderContext.h"
#include <algorithm>

namespace traktor::shine
{

// ============================================================================
// RadioButton
// ============================================================================

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.RadioButton", RadioButton, Interactable)

RadioButton::RadioButton(const std::wstring& name)
:	Interactable(name)
{
	createDefaultWidgets();
	updateDotVisibility();
}

RadioButton::~RadioButton()
{
	// Remove from group
	if (m_group)
		m_group->removeButton(this);
}

void RadioButton::setSelected(bool selected)
{
	if (m_selected != selected)
	{
		m_selected = selected;
		updateDotVisibility();

		if (m_selected && onSelected)
			onSelected();
	}
}

void RadioButton::setGroup(RadioButtonGroup* group)
{
	if (m_group == group)
		return;

	// Remove from old group
	if (m_group)
		m_group->removeButton(this);

	m_group = group;

	// Add to new group
	if (m_group)
		m_group->addButton(this);
}

const std::wstring& RadioButton::getText() const
{
	if (m_textWidget)
		return m_textWidget->getText();

	static std::wstring empty;
	return empty;
}

void RadioButton::setText(const std::wstring& text)
{
	if (m_textWidget)
		m_textWidget->setText(text);
}

bool RadioButton::onMouseButtonDown(MouseButton button, const Vector2& position)
{
	// Call base class first
	if (!Interactable::onMouseButtonDown(button, position))
		return false;

	// Select this radio button
	if (button == MouseButton::Left)
	{
		if (m_group)
			m_group->setSelected(this);
		else
			setSelected(true);

		return true;
	}

	return false;
}

void RadioButton::render(RenderContext* rc, float parentOpacity)
{
	// Let base class handle rendering (will render children)
	Interactable::render(rc, parentOpacity);
}

void RadioButton::createDefaultWidgets()
{
	// Default radio button size
	Vector2 circleSize(20, 20);
	Vector2 totalSize = getSize();
	if (totalSize.x < circleSize.x) totalSize.x = 200;
	if (totalSize.y < circleSize.y) totalSize.y = 20;
	setSize(totalSize);

	// Create circle outline
	Ref<Image> circle = new Image(L"RadioCircle");
	circle->setPosition(Vector2(0, 0));
	circle->setSize(circleSize);
	circle->setColor(Color4f(1, 1, 1, 1));
	// No texture - renders as white circle (TODO: need circle rendering)
	addChild(circle);
	m_circleImage = circle;

	// Create dot (selected indicator)
	Ref<Image> dot = new Image(L"RadioDot");
	dot->setPosition(Vector2(5, 5));
	dot->setSize(Vector2(circleSize.x - 10, circleSize.y - 10));
	dot->setColor(Color4f(0, 0.8f, 1, 1)); // Cyan dot
	dot->setVisible(false);
	// No texture - renders as colored circle
	circle->addChild(dot);
	m_dotImage = dot;

	// Create label text
	Ref<Text> text = new Text(L"RadioLabel");
	text->setPosition(Vector2(circleSize.x + 5, 0));
	text->setSize(Vector2(totalSize.x - circleSize.x - 5, circleSize.y));
	text->setText(L"Radio Button");
	text->setHorizontalAlign(Text::HAlign::Left);
	text->setVerticalAlign(Text::VAlign::Middle);
	text->setColor(Color4f(1, 1, 1, 1));
	addChild(text);
	m_textWidget = text;
}

void RadioButton::updateDotVisibility()
{
	if (m_dotImage)
		m_dotImage->setVisible(m_selected);
}

// ============================================================================
// RadioButtonGroup
// ============================================================================

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.RadioButtonGroup", RadioButtonGroup, Object)

RadioButtonGroup::RadioButtonGroup()
{
}

RadioButtonGroup::~RadioButtonGroup()
{
	// Clear references from buttons
	for (RadioButton* button : m_buttons)
	{
		if (button)
			button->setGroup(nullptr);
	}
}

void RadioButtonGroup::setSelected(RadioButton* button)
{
	if (m_selected == button)
		return;

	// Deselect previous
	if (m_selected)
		m_selected->setSelected(false);

	m_selected = button;

	// Select new
	if (m_selected)
		m_selected->setSelected(true);

	// Fire event
	if (onSelectionChanged)
		onSelectionChanged(m_selected);
}

void RadioButtonGroup::addButton(RadioButton* button)
{
	if (!button)
		return;

	// Check if already in group
	auto it = std::find(m_buttons.begin(), m_buttons.end(), button);
	if (it != m_buttons.end())
		return;

	m_buttons.push_back(button);
}

void RadioButtonGroup::removeButton(RadioButton* button)
{
	if (!button)
		return;

	auto it = std::find(m_buttons.begin(), m_buttons.end(), button);
	if (it != m_buttons.end())
	{
		m_buttons.erase(it);

		// Clear selection if removing selected button
		if (m_selected == button)
			m_selected = nullptr;
	}
}

}
