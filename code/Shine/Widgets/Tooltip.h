/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Shine/Visual.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

class Panel;
class Text;

/*!
 * \brief Tooltip widget for displaying hover tooltips.
 *
 * Tooltip displays a small popup with text when the user hovers over an element.
 * Typically used with TooltipDisplay which manages showing/hiding tooltips.
 *
 * Usage:
 *   Ref<Tooltip> tooltip = new Tooltip(L"ItemTooltip");
 *   tooltip->setText(L"This is a helpful tooltip");
 *   button->setTooltip(tooltip);
 */
class T_DLLCLASS Tooltip : public Visual
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit Tooltip(const std::wstring& name = L"Tooltip");

	/*!
	 * \brief Destructor
	 */
	virtual ~Tooltip();

	// === Content ===

	/*!
	 * \brief Get tooltip text
	 */
	const std::wstring& getText() const;

	/*!
	 * \brief Set tooltip text
	 */
	void setText(const std::wstring& text);

	// === Configuration ===

	/*!
	 * \brief Get delay before showing (seconds)
	 */
	float getShowDelay() const { return m_showDelay; }

	/*!
	 * \brief Set show delay
	 */
	void setShowDelay(float delay) { m_showDelay = delay; }

	/*!
	 * \brief Get offset from cursor
	 */
	Vector2 getOffset() const { return m_offset; }

	/*!
	 * \brief Set offset from cursor
	 */
	void setOffset(const Vector2& offset) { m_offset = offset; }

	// === Visuals ===

	/*!
	 * \brief Get background panel
	 */
	Panel* getBackground() const { return m_background; }

	/*!
	 * \brief Get text widget
	 */
	Text* getTextWidget() const { return m_textWidget; }

	// === Auto-sizing ===

	/*!
	 * \brief Update size to fit text
	 */
	void updateSizeToFitText();

	virtual void render(RenderContext* rc, float parentOpacity) override;

private:
	float m_showDelay = 0.5f; // 500ms delay
	Vector2 m_offset = Vector2(10, 10); // Default offset from cursor

	// Child widgets
	Panel* m_background = nullptr;
	Text* m_textWidget = nullptr;

	// Helper methods
	void createDefaultWidgets();
};

}
