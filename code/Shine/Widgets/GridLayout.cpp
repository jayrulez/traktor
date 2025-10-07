/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/GridLayout.h"
#include <algorithm>
#include <cmath>

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.GridLayout", GridLayout, Layout)

GridLayout::GridLayout(const std::wstring& name)
:	Layout(name)
{
}

GridLayout::~GridLayout()
{
}

void GridLayout::setColumns(int columns)
{
	if (m_columns != columns)
	{
		m_columns = std::max(1, columns);
		markLayoutDirty();
	}
}

void GridLayout::setRows(int rows)
{
	if (m_rows != rows)
	{
		m_rows = std::max(0, rows);
		markLayoutDirty();
	}
}

void GridLayout::setCellSizeMode(CellSizeMode mode)
{
	if (m_cellSizeMode != mode)
	{
		m_cellSizeMode = mode;
		markLayoutDirty();
	}
}

void GridLayout::setCellSize(const Vector2& size)
{
	if (m_cellSize != size)
	{
		m_cellSize = size;
		markLayoutDirty();
	}
}

void GridLayout::updateLayout()
{
	AlignedVector<UIElement*> children = getLayoutChildren();
	if (children.empty())
		return;

	Vector2 contentSize = getContentSize();
	Vector2 contentOffset = getContentOffset();
	float spacing = getSpacing();

	// Calculate grid dimensions
	int cols, rows;
	calculateGridDimensions((int)children.size(), cols, rows);

	// Calculate cell size
	Vector2 cellSize = m_cellSize;

	if (m_cellSizeMode == CellSizeMode::Flexible)
	{
		// Calculate cell size to fill available space
		float totalSpacingX = spacing * (cols - 1);
		float totalSpacingY = spacing * (rows - 1);

		cellSize.x = (contentSize.x - totalSpacingX) / cols;
		cellSize.y = (contentSize.y - totalSpacingY) / rows;
	}

	// Position children in grid
	int childIndex = 0;

	for (int row = 0; row < rows && childIndex < (int)children.size(); ++row)
	{
		for (int col = 0; col < cols && childIndex < (int)children.size(); ++col)
		{
			UIElement* child = children[childIndex++];
			if (!child)
				continue;

			// Calculate cell position
			float x = contentOffset.x + col * (cellSize.x + spacing);
			float y = contentOffset.y + row * (cellSize.y + spacing);

			// Set child position and size
			child->setPosition(Vector2(x, y));
			child->setSize(cellSize);
		}
	}
}

Vector2 GridLayout::calculateMinSize() const
{
	AlignedVector<UIElement*> children = getLayoutChildren();

	if (children.empty())
		return Vector2(0, 0);

	// Calculate grid dimensions
	int cols, rows;
	calculateGridDimensions((int)children.size(), cols, rows);

	// Calculate total size
	float spacing = getSpacing();
	Vector2 cellSize = m_cellSize;

	float totalWidth = cols * cellSize.x + (cols - 1) * spacing;
	float totalHeight = rows * cellSize.y + (rows - 1) * spacing;

	// Add padding
	Padding padding = getPadding();
	totalWidth += padding.left + padding.right;
	totalHeight += padding.top + padding.bottom;

	return Vector2(totalWidth, totalHeight);
}

Vector2 GridLayout::calculatePreferredSize() const
{
	// For now, preferred size = min size
	return calculateMinSize();
}

void GridLayout::calculateGridDimensions(int childCount, int& outCols, int& outRows) const
{
	if (childCount == 0)
	{
		outCols = 0;
		outRows = 0;
		return;
	}

	outCols = m_columns;

	if (m_rows > 0)
	{
		// Fixed rows
		outRows = m_rows;
	}
	else
	{
		// Calculate rows based on children and columns
		outRows = (childCount + m_columns - 1) / m_columns; // Ceiling division
	}
}

}
