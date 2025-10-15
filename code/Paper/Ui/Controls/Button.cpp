/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Serialization/ISerializer.h"
#include "Core/Serialization/Member.h"
#include "Paper/Ui/Controls/Button.h"
#include "Paper/Ui/Controls/TextBlock.h"
#include "Paper/Ui/UIStyle.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.Button", 0, Button, Border)

Button::Button()
{
	// Create internal TextBlock
	m_textBlock = new TextBlock();
	setChild(m_textBlock);

	// Default colors
	m_normalBackground = Color4f(0.15f, 0.25f, 0.35f, 1.0f);
	m_hoverBackground = Color4f(0.2f, 0.35f, 0.5f, 1.0f);
	m_pressedBackground = Color4f(0.1f, 0.2f, 0.3f, 1.0f);

	setBackground(m_normalBackground);
}

void Button::setText(const std::wstring& text)
{
	if (m_textBlock)
		m_textBlock->setText(text);
}

const std::wstring& Button::getText() const
{
	static const std::wstring empty;
	return m_textBlock ? m_textBlock->getText() : empty;
}

void Button::setFontId(const Guid& fontId)
{
	if (m_textBlock)
		m_textBlock->setFontId(fontId);
}

const Guid& Button::getFontId() const
{
	static const Guid empty;
	return m_textBlock ? m_textBlock->getFontId() : empty;
}

void Button::applyStyle(const UIStyle* style)
{
	Border::applyStyle(style);

	if (style)
	{
		// Apply button-specific colors if available
		Color4f normalBg;
		if (style->tryGetColor(L"NormalBackground", normalBg))
		{
			m_normalBackground = normalBg;
			if (!m_isMouseOver && !m_isPressed)
				setBackground(m_normalBackground);
		}

		Color4f hoverBg;
		if (style->tryGetColor(L"HoverBackground", hoverBg))
			m_hoverBackground = hoverBg;

		Color4f pressedBg;
		if (style->tryGetColor(L"PressedBackground", pressedBg))
			m_pressedBackground = pressedBg;
	}

	// Apply style to text block
	if (m_textBlock && style)
		m_textBlock->applyStyle(style);
}

void Button::onMouseEnter(MouseEvent& event)
{
	Border::onMouseEnter(event);

	if (!m_isPressed)
		setBackground(m_hoverBackground);
}

void Button::onMouseLeave(MouseEvent& event)
{
	Border::onMouseLeave(event);

	if (!m_isPressed)
		setBackground(m_normalBackground);
}

void Button::onMouseDown(MouseEvent& event)
{
	Border::onMouseDown(event);

	m_isPressed = true;
	setBackground(m_pressedBackground);
}

void Button::onMouseUp(MouseEvent& event)
{
	Border::onMouseUp(event);

	m_isPressed = false;
	if (isMouseOver())
		setBackground(m_hoverBackground);
	else
		setBackground(m_normalBackground);
}

void Button::serialize(ISerializer& s)
{
	Border::serialize(s);

	s >> Member< Color4f >(L"normalBackground", m_normalBackground);
	s >> Member< Color4f >(L"hoverBackground", m_hoverBackground);
	s >> Member< Color4f >(L"pressedBackground", m_pressedBackground);
}

}
