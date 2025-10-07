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
#include "Shine/Layout.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

/*!
 * \brief LayoutCell provides additional layout properties for grid children.
 *
 * LayoutCell is a lightweight wrapper that provides grid-specific properties
 * for elements in a GridLayout, such as column span, row span, and alignment.
 *
 * Usage:
 *   Ref<LayoutCell> cell = new LayoutCell(L"Cell");
 *   cell->setColumnSpan(2);  // Span 2 columns
 *   cell->setRowSpan(1);     // Span 1 row
 *
 *   // Add actual content as child
 *   cell->addChild(new Button(L"WideButton"));
 *
 *   grid->addChild(cell);
 */
class T_DLLCLASS LayoutCell : public UIElement
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Cell alignment
	 */
	enum class Alignment
	{
		TopLeft,
		TopCenter,
		TopRight,
		MiddleLeft,
		MiddleCenter,
		MiddleRight,
		BottomLeft,
		BottomCenter,
		BottomRight,
		Stretch  // Stretch to fill cell
	};

	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit LayoutCell(const std::wstring& name = L"LayoutCell");

	/*!
	 * \brief Destructor
	 */
	virtual ~LayoutCell();

	// === Grid Properties ===

	/*!
	 * \brief Get column index (0-based)
	 */
	int getColumn() const { return m_column; }

	/*!
	 * \brief Set column index
	 */
	void setColumn(int column) { m_column = std::max(0, column); }

	/*!
	 * \brief Get row index (0-based)
	 */
	int getRow() const { return m_row; }

	/*!
	 * \brief Set row index
	 */
	void setRow(int row) { m_row = std::max(0, row); }

	/*!
	 * \brief Get column span
	 */
	int getColumnSpan() const { return m_columnSpan; }

	/*!
	 * \brief Set column span
	 */
	void setColumnSpan(int span) { m_columnSpan = std::max(1, span); }

	/*!
	 * \brief Get row span
	 */
	int getRowSpan() const { return m_rowSpan; }

	/*!
	 * \brief Set row span
	 */
	void setRowSpan(int span) { m_rowSpan = std::max(1, span); }

	// === Alignment ===

	/*!
	 * \brief Get cell alignment
	 */
	Alignment getAlignment() const { return m_alignment; }

	/*!
	 * \brief Set cell alignment
	 */
	void setAlignment(Alignment alignment) { m_alignment = alignment; }

	// === Margins ===

	/*!
	 * \brief Get margins (spacing around content within cell)
	 */
	Padding getMargins() const { return m_margins; }

	/*!
	 * \brief Set margins
	 */
	void setMargins(const Padding& margins) { m_margins = margins; }

	// === Min/Max Size ===

	/*!
	 * \brief Get minimum size
	 */
	Vector2 getMinSize() const { return m_minSize; }

	/*!
	 * \brief Set minimum size
	 */
	void setMinSize(const Vector2& size) { m_minSize = size; }

	/*!
	 * \brief Get maximum size (0 = no limit)
	 */
	Vector2 getMaxSize() const { return m_maxSize; }

	/*!
	 * \brief Set maximum size
	 */
	void setMaxSize(const Vector2& size) { m_maxSize = size; }

private:
	int m_column = 0;
	int m_row = 0;
	int m_columnSpan = 1;
	int m_rowSpan = 1;
	Alignment m_alignment = Alignment::Stretch;
	Padding m_margins;
	Vector2 m_minSize = Vector2(0, 0);
	Vector2 m_maxSize = Vector2(0, 0); // 0 = no limit
};

}
