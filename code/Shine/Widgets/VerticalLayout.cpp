/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/VerticalLayout.h"
#include <algorithm>

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.VerticalLayout", VerticalLayout, Layout)

VerticalLayout::VerticalLayout(const std::wstring& name)
:	Layout(name)
{
}

VerticalLayout::~VerticalLayout()
{
}

void VerticalLayout::setChildAlignment(ChildAlignment alignment)
{
	if (m_childAlignment != alignment)
	{
		m_childAlignment = alignment;
		markLayoutDirty();
	}
}

void VerticalLayout::updateLayout()
{
	AlignedVector<UIElement*> children = getLayoutChildren();
	if (children.empty())
		return;

	Vector2 contentSize = getContentSize();
	Vector2 contentOffset = getContentOffset();
	float spacing = getSpacing();

	// Calculate total height needed and max width
	float totalHeight = 0.0f;
	float maxWidth = 0.0f;

	for (UIElement* child : children)
	{
		if (!child)
			continue;

		Vector2 childSize = child->getSize();
		totalHeight += childSize.y;
		maxWidth = std::max(maxWidth, childSize.x);
	}

	// Add spacing between children
	if (children.size() > 1)
		totalHeight += spacing * (children.size() - 1);

	// Position children vertically
	float currentY = contentOffset.y;

	for (UIElement* child : children)
	{
		if (!child)
			continue;

		Vector2 childSize = child->getSize();

		// Calculate X position based on alignment
		float childX = contentOffset.x;

		switch (m_childAlignment)
		{
		case ChildAlignment::Start:
			// Left-aligned
			childX = contentOffset.x;
			break;

		case ChildAlignment::Center:
			// Center horizontally
			childX = contentOffset.x + (contentSize.x - childSize.x) * 0.5f;
			break;

		case ChildAlignment::End:
			// Right-aligned
			childX = contentOffset.x + contentSize.x - childSize.x;
			break;

		case ChildAlignment::Stretch:
			// Stretch to fill width
			childX = contentOffset.x;
			childSize.x = contentSize.x;
			child->setSize(childSize);
			break;
		}

		// Set child position
		child->setPosition(Vector2(childX, currentY));

		// Advance Y position
		currentY += childSize.y + spacing;
	}
}

Vector2 VerticalLayout::calculateMinSize() const
{
	AlignedVector<UIElement*> children = getLayoutChildren();

	float totalHeight = 0.0f;
	float maxWidth = 0.0f;

	for (UIElement* child : children)
	{
		if (!child)
			continue;

		Vector2 childSize = child->getSize();
		totalHeight += childSize.y;
		maxWidth = std::max(maxWidth, childSize.x);
	}

	// Add spacing
	if (children.size() > 1)
		totalHeight += getSpacing() * (children.size() - 1);

	// Add padding
	Padding padding = getPadding();
	totalHeight += padding.top + padding.bottom;
	maxWidth += padding.left + padding.right;

	return Vector2(maxWidth, totalHeight);
}

Vector2 VerticalLayout::calculatePreferredSize() const
{
	// For now, preferred size = min size
	return calculateMinSize();
}

}
