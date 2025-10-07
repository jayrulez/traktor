/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/LayoutFitter.h"
#include <algorithm>

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.LayoutFitter", LayoutFitter, Layout)

LayoutFitter::LayoutFitter(const std::wstring& name)
:	Layout(name)
{
}

LayoutFitter::~LayoutFitter()
{
}

void LayoutFitter::setHorizontalFit(FitMode mode)
{
	if (m_horizontalFit != mode)
	{
		m_horizontalFit = mode;
		markLayoutDirty();
	}
}

void LayoutFitter::setVerticalFit(FitMode mode)
{
	if (m_verticalFit != mode)
	{
		m_verticalFit = mode;
		markLayoutDirty();
	}
}

void LayoutFitter::setFitWidth(bool fit)
{
	setHorizontalFit(fit ? FitMode::PreferredSize : FitMode::Unconstrained);
}

void LayoutFitter::setFitHeight(bool fit)
{
	setVerticalFit(fit ? FitMode::PreferredSize : FitMode::Unconstrained);
}

void LayoutFitter::updateLayout()
{
	// Calculate fitted size
	Vector2 fittedSize = calculateFittedSize();

	// Apply fitted size
	if (fittedSize.x > 0 || fittedSize.y > 0)
	{
		Vector2 currentSize = getSize();

		if (m_horizontalFit != FitMode::Unconstrained)
			currentSize.x = fittedSize.x;

		if (m_verticalFit != FitMode::Unconstrained)
			currentSize.y = fittedSize.y;

		setSize(currentSize);
	}

	// Let base class position children
	Layout::updateLayout();
}

Vector2 LayoutFitter::calculateMinSize() const
{
	AlignedVector<UIElement*> children = getLayoutChildren();

	if (children.empty())
		return Vector2(0, 0);

	float maxWidth = 0.0f;
	float maxHeight = 0.0f;

	for (UIElement* child : children)
	{
		if (!child)
			continue;

		Vector2 childSize = child->getSize();
		Vector2 childPos = child->getPosition();

		// Calculate bounds including position
		float right = childPos.x + childSize.x;
		float bottom = childPos.y + childSize.y;

		maxWidth = std::max(maxWidth, right);
		maxHeight = std::max(maxHeight, bottom);
	}

	// Add padding
	Padding padding = getPadding();
	maxWidth += padding.left + padding.right;
	maxHeight += padding.top + padding.bottom;

	return Vector2(maxWidth, maxHeight);
}

Vector2 LayoutFitter::calculatePreferredSize() const
{
	// For now, preferred size = min size
	// Could be extended to query children for their preferred size
	return calculateMinSize();
}

Vector2 LayoutFitter::calculateFittedSize() const
{
	Vector2 size(0, 0);

	switch (m_horizontalFit)
	{
	case FitMode::MinSize:
		size.x = calculateMinSize().x;
		break;

	case FitMode::PreferredSize:
		size.x = calculatePreferredSize().x;
		break;

	case FitMode::Unconstrained:
		size.x = getSize().x;
		break;
	}

	switch (m_verticalFit)
	{
	case FitMode::MinSize:
		size.y = calculateMinSize().y;
		break;

	case FitMode::PreferredSize:
		size.y = calculatePreferredSize().y;
		break;

	case FitMode::Unconstrained:
		size.y = getSize().y;
		break;
	}

	return size;
}

}
