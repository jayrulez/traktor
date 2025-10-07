/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

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
 * \brief Grid layout widget.
 *
 * GridLayout arranges children in a grid with:
 * - Fixed or automatic columns
 * - Fixed or automatic rows
 * - Spacing between cells
 * - Cell size control
 * - Child alignment within cells
 *
 * Usage:
 *   Ref<GridLayout> grid = new GridLayout();
 *   grid->setColumns(3);  // 3 columns
 *   grid->setSpacing(5);
 *   for (int i = 0; i < 9; ++i)
 *       grid->addChild(new Button(L"Button"));
 */
class T_DLLCLASS GridLayout : public Layout
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Cell size mode
	 */
	enum class CellSizeMode
	{
		Fixed,      // Fixed cell size
		Flexible    // Cells expand to fill space
	};

	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit GridLayout(const std::wstring& name = L"GridLayout");

	/*!
	 * \brief Destructor
	 */
	virtual ~GridLayout();

	// === Grid Properties ===

	/*!
	 * \brief Get number of columns (0 = automatic)
	 */
	int getColumns() const { return m_columns; }

	/*!
	 * \brief Set number of columns
	 */
	void setColumns(int columns);

	/*!
	 * \brief Get number of rows (0 = automatic)
	 */
	int getRows() const { return m_rows; }

	/*!
	 * \brief Set number of rows
	 */
	void setRows(int rows);

	// === Cell Size ===

	/*!
	 * \brief Get cell size mode
	 */
	CellSizeMode getCellSizeMode() const { return m_cellSizeMode; }

	/*!
	 * \brief Set cell size mode
	 */
	void setCellSizeMode(CellSizeMode mode);

	/*!
	 * \brief Get fixed cell size (for Fixed mode)
	 */
	Vector2 getCellSize() const { return m_cellSize; }

	/*!
	 * \brief Set fixed cell size
	 */
	void setCellSize(const Vector2& size);

	// === Layout ===

	virtual void updateLayout() override;

	virtual Vector2 calculateMinSize() const override;

	virtual Vector2 calculatePreferredSize() const override;

private:
	int m_columns = 1;
	int m_rows = 0; // 0 = automatic based on children
	CellSizeMode m_cellSizeMode = CellSizeMode::Fixed;
	Vector2 m_cellSize = Vector2(100, 100);

	// Helper to calculate grid dimensions
	void calculateGridDimensions(int childCount, int& outCols, int& outRows) const;
};

}
