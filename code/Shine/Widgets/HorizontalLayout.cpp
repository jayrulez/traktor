/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/HorizontalLayout.h"
#include <algorithm>

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.HorizontalLayout", HorizontalLayout, Layout)

HorizontalLayout::HorizontalLayout(const std::wstring& name)
:	Layout(name)
{
}

HorizontalLayout::~HorizontalLayout()
{
}

void HorizontalLayout::setChildAlignment(ChildAlignment alignment)
{
	if (m_childAlignment != alignment)
	{
		m_childAlignment = alignment;
		markLayoutDirty();
	}
}

void HorizontalLayout::updateLayout()
{
	AlignedVector<UIElement*> children = getLayoutChildren();
	if (children.empty())
		return;

	Vector2 contentSize = getContentSize();
	Vector2 contentOffset = getContentOffset();
	float spacing = getSpacing();

	// Calculate total width needed and max height
	float totalWidth = 0.0f;
	float maxHeight = 0.0f;

	for (UIElement* child : children)
	{
		if (!child)
			continue;

		Vector2 childSize = child->getSize();
		totalWidth += childSize.x;
		maxHeight = std::max(maxHeight, childSize.y);
	}

	// Add spacing between children
	if (children.size() > 1)
		totalWidth += spacing * (children.size() - 1);

	// Position children horizontally
	float currentX = contentOffset.x;

	for (UIElement* child : children)
	{
		if (!child)
			continue;

		Vector2 childSize = child->getSize();

		// Calculate Y position based on alignment
		float childY = contentOffset.y;

		switch (m_childAlignment)
		{
		case ChildAlignment::Start:
			// Top-aligned
			childY = contentOffset.y;
			break;

		case ChildAlignment::Center:
			// Center vertically
			childY = contentOffset.y + (contentSize.y - childSize.y) * 0.5f;
			break;

		case ChildAlignment::End:
			// Bottom-aligned
			childY = contentOffset.y + contentSize.y - childSize.y;
			break;

		case ChildAlignment::Stretch:
			// Stretch to fill height
			childY = contentOffset.y;
			childSize.y = contentSize.y;
			child->setSize(childSize);
			break;
		}

		// Set child position
		child->setPosition(Vector2(currentX, childY));

		// Advance X position
		currentX += childSize.x + spacing;
	}
}

Vector2 HorizontalLayout::calculateMinSize() const
{
	AlignedVector<UIElement*> children = getLayoutChildren();

	float totalWidth = 0.0f;
	float maxHeight = 0.0f;

	for (UIElement* child : children)
	{
		if (!child)
			continue;

		Vector2 childSize = child->getSize();
		totalWidth += childSize.x;
		maxHeight = std::max(maxHeight, childSize.y);
	}

	// Add spacing
	if (children.size() > 1)
		totalWidth += getSpacing() * (children.size() - 1);

	// Add padding
	Padding padding = getPadding();
	totalWidth += padding.left + padding.right;
	maxHeight += padding.top + padding.bottom;

	return Vector2(totalWidth, maxHeight);
}

Vector2 HorizontalLayout::calculatePreferredSize() const
{
	// For now, preferred size = min size
	return calculateMinSize();
}

}
