/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/TooltipDisplay.h"
#include "Shine/Widgets/Tooltip.h"
#include "Shine/Interactable.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.TooltipDisplay", TooltipDisplay, UIElement)

TooltipDisplay::TooltipDisplay(const std::wstring& name)
:	UIElement(name)
{
}

TooltipDisplay::~TooltipDisplay()
{
}

void TooltipDisplay::showTooltip(Tooltip* tooltip, const Vector2& position)
{
	if (!tooltip)
		return;

	// Hide current tooltip if different
	if (m_currentTooltip != tooltip)
		hideTooltip();

	m_currentTooltip = tooltip;
	m_tooltipVisible = true;

	// Add as child if not already
	if (tooltip->getParent() != this)
		addChild(tooltip);

	// Position tooltip
	updateTooltipPosition(position);

	// Make visible
	tooltip->setVisible(true);
}

void TooltipDisplay::hideTooltip()
{
	if (m_currentTooltip)
	{
		m_currentTooltip->setVisible(false);
		m_currentTooltip = nullptr;
	}

	m_tooltipVisible = false;
	m_hoverTime = 0.0f;
}

void TooltipDisplay::setTooltipForElement(Interactable* element, Tooltip* tooltip)
{
	if (!element || !tooltip)
		return;

	// Setup hover callbacks
	element->onHoverStart = [this, tooltip]() {
		m_hoveredElement = nullptr; // Will be set by element
		m_hoverTime = 0.0f;
	};

	element->onHoverEnd = [this]() {
		hideTooltip();
		m_hoveredElement = nullptr;
	};

	// Store tooltip reference (would need element->tooltip member)
	// For now, this is a simplified implementation
}

void TooltipDisplay::update(float deltaTime)
{
	UIElement::update(deltaTime);

	// Handle delayed tooltip display
	if (m_hoveredElement && !m_tooltipVisible)
	{
		m_hoverTime += deltaTime;

		// Show tooltip after delay
		// if (m_hoverTime >= tooltip->getShowDelay())
		// {
		//     showTooltip(tooltip, mousePosition);
		// }
	}
}

void TooltipDisplay::updateTooltipPosition(const Vector2& position)
{
	if (!m_currentTooltip)
		return;

	// Position tooltip near cursor with offset
	Vector2 tooltipPos = position + m_currentTooltip->getOffset();

	// TODO: Clamp to screen bounds
	// Vector2 canvasSize = getCanvas()->getSize();
	// if (tooltipPos.x + tooltipSize.x > canvasSize.x)
	//     tooltipPos.x = canvasSize.x - tooltipSize.x;
	// if (tooltipPos.y + tooltipSize.y > canvasSize.y)
	//     tooltipPos.y = position.y - tooltipSize.y - 10;

	m_currentTooltip->setPosition(tooltipPos);
}

}
