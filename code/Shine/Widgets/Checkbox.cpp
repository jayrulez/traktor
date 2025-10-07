/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/Checkbox.h"
#include "Shine/Widgets/Image.h"
#include "Shine/Widgets/Text.h"
#include "Shine/RenderContext.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Checkbox", Checkbox, Interactable)

Checkbox::Checkbox(const std::wstring& name)
:	Interactable(name)
{
	createDefaultWidgets();
	updateCheckmarkVisibility();
}

Checkbox::~Checkbox()
{
}

void Checkbox::setChecked(bool checked)
{
	if (m_checked != checked)
	{
		m_checked = checked;
		updateCheckmarkVisibility();

		if (onCheckedChanged)
			onCheckedChanged(m_checked);
	}
}

const std::wstring& Checkbox::getText() const
{
	if (m_textWidget)
		return m_textWidget->getText();

	static std::wstring empty;
	return empty;
}

void Checkbox::setText(const std::wstring& text)
{
	if (m_textWidget)
		m_textWidget->setText(text);
}

bool Checkbox::onMouseButtonDown(MouseButton button, const Vector2& position)
{
	// Call base class first
	if (!Interactable::onMouseButtonDown(button, position))
		return false;

	// Toggle on click
	if (button == MouseButton::Left)
	{
		toggle();
		return true;
	}

	return false;
}

void Checkbox::render(RenderContext* rc, float parentOpacity)
{
	// Let base class handle rendering (will render children)
	Interactable::render(rc, parentOpacity);
}

void Checkbox::createDefaultWidgets()
{
	// Default checkbox size
	Vector2 boxSize(20, 20);
	Vector2 totalSize = getSize();
	if (totalSize.x < boxSize.x) totalSize.x = 200; // Default width
	if (totalSize.y < boxSize.y) totalSize.y = 20;  // Default height
	setSize(totalSize);

	// Create box background
	Ref<Image> box = new Image(L"CheckboxBox");
	box->setPosition(Vector2(0, 0));
	box->setSize(boxSize);
	box->setColor(Color4f(1, 1, 1, 1));
	// No texture - renders as white rectangle
	addChild(box);
	m_boxImage = box;

	// Create checkmark (initially hidden)
	Ref<Image> checkmark = new Image(L"CheckboxCheckmark");
	checkmark->setPosition(Vector2(2, 2));
	checkmark->setSize(Vector2(boxSize.x - 4, boxSize.y - 4));
	checkmark->setColor(Color4f(0, 0.8f, 0, 1)); // Green checkmark
	checkmark->setVisible(false);
	// No texture - renders as green rectangle (placeholder for actual checkmark icon)
	box->addChild(checkmark);
	m_checkmarkImage = checkmark;

	// Create label text
	Ref<Text> text = new Text(L"CheckboxLabel");
	text->setPosition(Vector2(boxSize.x + 5, 0));
	text->setSize(Vector2(totalSize.x - boxSize.x - 5, boxSize.y));
	text->setText(L"Checkbox");
	text->setHorizontalAlign(Text::HAlign::Left);
	text->setVerticalAlign(Text::VAlign::Middle);
	text->setColor(Color4f(1, 1, 1, 1));
	addChild(text);
	m_textWidget = text;
}

void Checkbox::updateCheckmarkVisibility()
{
	if (m_checkmarkImage)
		m_checkmarkImage->setVisible(m_checked);
}

}
