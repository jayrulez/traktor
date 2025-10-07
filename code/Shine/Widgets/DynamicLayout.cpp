/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/DynamicLayout.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.DynamicLayout", DynamicLayout, Layout)

DynamicLayout::DynamicLayout(const std::wstring& name)
:	Layout(name)
{
}

DynamicLayout::~DynamicLayout()
{
	clearItems();
}

void DynamicLayout::setPrototype(UIElement* prototype)
{
	m_prototype = prototype;
}

void DynamicLayout::setItemCount(int count)
{
	if (m_itemCount != count)
	{
		m_itemCount = count;
		rebuild();
	}
}

void DynamicLayout::rebuild()
{
	clearItems();

	if (!m_prototype)
		return;

	// Create items
	for (int i = 0; i < m_itemCount; ++i)
	{
		UIElement* item = createItem(i);
		if (item)
		{
			m_items.push_back(item);
			addChild(item);

			if (onItemCreated)
				onItemCreated(item, i);
		}
	}

	// Update layout
	updateLayout();
}

void DynamicLayout::setOrientation(Orientation orientation)
{
	if (m_orientation != orientation)
	{
		m_orientation = orientation;
		updateLayout();
	}
}

void DynamicLayout::setColumns(int columns)
{
	if (m_columns != columns)
	{
		m_columns = std::max(1, columns);
		if (m_orientation == Orientation::Grid)
			updateLayout();
	}
}

void DynamicLayout::updateLayout()
{
	switch (m_orientation)
	{
	case Orientation::Horizontal:
		layoutHorizontal();
		break;

	case Orientation::Vertical:
		layoutVertical();
		break;

	case Orientation::Grid:
		layoutGrid();
		break;
	}
}

void DynamicLayout::clearItems()
{
	for (auto& item : m_items)
		removeChild(item);

	m_items.clear();
}

UIElement* DynamicLayout::createItem(int index)
{
	// Note: This is a placeholder implementation
	// A real implementation would need proper cloning
	// TODO: Implement proper UI element cloning
	return nullptr;
}

void DynamicLayout::layoutHorizontal()
{
	float xOffset = m_padding.left;

	for (auto& item : m_items)
	{
		item->setPosition(Vector2(xOffset, m_padding.top));
		xOffset += item->getSize().x + m_spacing;
	}
}

void DynamicLayout::layoutVertical()
{
	float yOffset = m_padding.top;

	for (auto& item : m_items)
	{
		item->setPosition(Vector2(m_padding.left, yOffset));
		yOffset += item->getSize().y + m_spacing;
	}
}

void DynamicLayout::layoutGrid()
{
	float xOffset = m_padding.left;
	float yOffset = m_padding.top;
	int column = 0;
	float rowHeight = 0.0f;

	for (auto& item : m_items)
	{
		// Position item
		item->setPosition(Vector2(xOffset, yOffset));

		// Track max height in row
		rowHeight = std::max(rowHeight, item->getSize().y);

		// Advance column
		column++;
		xOffset += item->getSize().x + m_spacing;

		// Wrap to next row
		if (column >= m_columns)
		{
			column = 0;
			xOffset = m_padding.left;
			yOffset += rowHeight + m_spacing;
			rowHeight = 0.0f;
		}
	}
}

}
