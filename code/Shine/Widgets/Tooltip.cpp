/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/Tooltip.h"
#include "Shine/Widgets/Panel.h"
#include "Shine/Widgets/Text.h"
#include "Shine/RenderContext.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Tooltip", Tooltip, Visual)

Tooltip::Tooltip(const std::wstring& name)
:	Visual(name)
{
	createDefaultWidgets();
}

Tooltip::~Tooltip()
{
}

const std::wstring& Tooltip::getText() const
{
	if (m_textWidget)
		return m_textWidget->getText();

	static std::wstring empty;
	return empty;
}

void Tooltip::setText(const std::wstring& text)
{
	if (m_textWidget)
	{
		m_textWidget->setText(text);
		updateSizeToFitText();
	}
}

void Tooltip::render(RenderContext* rc, float parentOpacity)
{
	// Let base class handle rendering (will render children)
	Visual::render(rc, parentOpacity);
}

void Tooltip::createDefaultWidgets()
{
	// Create background panel
	Ref<Panel> background = new Panel(L"TooltipBackground");
	background->setPosition(Vector2(0, 0));
	background->setSize(Vector2(100, 30));
	background->setBackgroundColor(Color4f(0.1f, 0.1f, 0.1f, 0.95f));
	addChild(background);
	m_background = background;

	// Background anchoring
	background->setAnchorMin(Vector2(0, 0));
	background->setAnchorMax(Vector2(1, 1));
	background->setOffsetMin(Vector2(0, 0));
	background->setOffsetMax(Vector2(0, 0));

	// Create text
	Ref<Text> text = new Text(L"TooltipText");
	text->setPosition(Vector2(8, 8));
	text->setSize(Vector2(84, 14));
	text->setText(L"Tooltip");
	text->setFontSize(12);
	text->setColor(Color4f(1, 1, 1, 1));
	text->setHorizontalAlign(Text::HAlign::Left);
	text->setVerticalAlign(Text::VAlign::Top);
	background->addChild(text);
	m_textWidget = text;

	setSize(Vector2(100, 30));
}

void Tooltip::updateSizeToFitText()
{
	if (!m_textWidget)
		return;

	// Calculate text size
	Vector2 textSize = m_textWidget->calculateTextSize();

	// Add padding
	Vector2 tooltipSize = textSize + Vector2(16, 16);

	// Set tooltip size
	setSize(tooltipSize);

	// Update text widget size
	m_textWidget->setSize(textSize);
}

}
