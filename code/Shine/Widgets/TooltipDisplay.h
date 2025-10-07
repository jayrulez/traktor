/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Shine/UIElement.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

class Tooltip;
class Interactable;

/*!
 * \brief TooltipDisplay manages showing and hiding tooltips.
 *
 * TooltipDisplay is typically added to the Canvas root and handles
 * displaying tooltips when the user hovers over elements.
 *
 * Usage:
 *   Ref<TooltipDisplay> tooltipDisplay = new TooltipDisplay(L"Tooltips");
 *   canvas->addChild(tooltipDisplay);
 *
 *   // Elements can then have tooltips
 *   button->setTooltipText(L"Click to start game");
 */
class T_DLLCLASS TooltipDisplay : public UIElement
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit TooltipDisplay(const std::wstring& name = L"TooltipDisplay");

	/*!
	 * \brief Destructor
	 */
	virtual ~TooltipDisplay();

	// === Tooltip Management ===

	/*!
	 * \brief Show tooltip at position
	 */
	void showTooltip(Tooltip* tooltip, const Vector2& position);

	/*!
	 * \brief Hide current tooltip
	 */
	void hideTooltip();

	/*!
	 * \brief Get current visible tooltip
	 */
	Tooltip* getCurrentTooltip() const { return m_currentTooltip; }

	/*!
	 * \brief Set tooltip for an interactable element
	 */
	void setTooltipForElement(Interactable* element, Tooltip* tooltip);

	// === Update ===

	virtual void update(float deltaTime) override;

private:
	Tooltip* m_currentTooltip = nullptr;
	Interactable* m_hoveredElement = nullptr;
	float m_hoverTime = 0.0f;
	bool m_tooltipVisible = false;

	// Helper methods
	void updateTooltipPosition(const Vector2& position);
};

}
