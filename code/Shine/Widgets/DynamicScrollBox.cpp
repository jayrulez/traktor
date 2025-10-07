/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/DynamicScrollBox.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.DynamicScrollBox", DynamicScrollBox, ScrollView)

DynamicScrollBox::DynamicScrollBox(const std::wstring& name)
:	ScrollView(name)
{
}

DynamicScrollBox::~DynamicScrollBox()
{
	clearVisibleItems();
}

void DynamicScrollBox::setPrototype(UIElement* prototype)
{
	m_prototype = prototype;
}

void DynamicScrollBox::setItemCount(int count)
{
	if (m_itemCount != count)
	{
		m_itemCount = count;
		rebuild();
	}
}

void DynamicScrollBox::setItemHeight(float height)
{
	if (m_itemHeight != height)
	{
		m_itemHeight = height;
		rebuild();
	}
}

void DynamicScrollBox::rebuild()
{
	if (!m_prototype)
		return;

	// Set content size based on total items
	float totalHeight = m_itemCount * m_itemHeight;
	if (m_contentArea)
		m_contentArea->setSize(Vector2(getSize().x, totalHeight));

	// Update visible items
	updateVisibleItems();
}

void DynamicScrollBox::scrollToItem(int index)
{
	if (index < 0 || index >= m_itemCount)
		return;

	float targetY = index * m_itemHeight;
	setScrollOffset(Vector2(0.0f, targetY));
}

void DynamicScrollBox::update(float deltaTime)
{
	ScrollView::update(deltaTime);

	// Update visible items when scrolling
	updateVisibleItems();
}

void DynamicScrollBox::updateVisibleItems()
{
	if (!m_prototype || m_itemCount == 0 || m_itemHeight <= 0.0f)
		return;

	// Calculate visible range
	float scrollY = getScrollOffset().y;
	float viewportHeight = getSize().y;

	int firstVisible = (int)(scrollY / m_itemHeight);
	int lastVisible = (int)((scrollY + viewportHeight) / m_itemHeight);

	// Clamp to valid range
	firstVisible = clamp(firstVisible, 0, m_itemCount - 1);
	lastVisible = clamp(lastVisible, 0, m_itemCount - 1);

	// Check if range changed
	if (firstVisible == m_firstVisibleIndex && lastVisible == m_lastVisibleIndex)
		return;

	m_firstVisibleIndex = firstVisible;
	m_lastVisibleIndex = lastVisible;

	// Clear old items
	clearVisibleItems();

	// Create visible items
	for (int i = firstVisible; i <= lastVisible; ++i)
	{
		UIElement* item = createItem(i);
		if (item)
		{
			m_visibleItems.push_back(item);

			// Position item
			item->setPosition(Vector2(0.0f, i * m_itemHeight));

			// Add to content area
			if (m_contentArea)
				m_contentArea->addChild(item);

			if (onItemCreated)
				onItemCreated(item, i);

			if (onItemVisible)
				onItemVisible(item, i);
		}
	}
}

void DynamicScrollBox::clearVisibleItems()
{
	if (m_contentArea)
	{
		for (auto& item : m_visibleItems)
			m_contentArea->removeChild(item);
	}

	m_visibleItems.clear();
}

UIElement* DynamicScrollBox::createItem(int index)
{
	// Note: This is a placeholder implementation
	// A real implementation would need proper cloning
	// TODO: Implement proper UI element cloning
	return nullptr;
}

}
