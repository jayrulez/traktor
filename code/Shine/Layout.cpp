/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Layout.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Layout", Layout, UIElement)

Layout::Layout(const std::wstring& name)
:	UIElement(name)
{
}

Layout::~Layout()
{
}

void Layout::update(float deltaTime)
{
	// Update layout if dirty and auto-layout is enabled
	if (m_layoutDirty && m_autoLayout)
	{
		updateLayout();
		m_layoutDirty = false;
	}

	// Call base class to update children
	UIElement::update(deltaTime);
}

void Layout::updateLayout()
{
	// Base implementation does nothing
	// Override in derived classes to implement layout logic
}

Vector2 Layout::calculateMinSize() const
{
	// Base implementation returns current size
	// Override in derived classes for proper calculation
	return getSize();
}

Vector2 Layout::calculatePreferredSize() const
{
	// Base implementation returns current size
	// Override in derived classes for proper calculation
	return getSize();
}

void Layout::setPadding(const Padding& padding)
{
	if (m_padding.left != padding.left ||
		m_padding.right != padding.right ||
		m_padding.top != padding.top ||
		m_padding.bottom != padding.bottom)
	{
		m_padding = padding;
		markLayoutDirty();
	}
}

void Layout::setSpacing(float spacing)
{
	if (m_spacing != spacing)
	{
		m_spacing = spacing;
		markLayoutDirty();
	}
}

void Layout::setIgnoreInvisibleChildren(bool ignore)
{
	if (m_ignoreInvisibleChildren != ignore)
	{
		m_ignoreInvisibleChildren = ignore;
		markLayoutDirty();
	}
}

void Layout::setAutoLayout(bool autoLayout)
{
	if (m_autoLayout != autoLayout)
	{
		m_autoLayout = autoLayout;

		// Immediately update layout if turning on
		if (autoLayout && m_layoutDirty)
		{
			updateLayout();
			m_layoutDirty = false;
		}
	}
}

void Layout::sizeChanged()
{
	UIElement::sizeChanged();
	markLayoutDirty();
}

void Layout::markLayoutDirty()
{
	m_layoutDirty = true;
}

Vector2 Layout::getContentSize() const
{
	Vector2 size = getSize();
	return Vector2(
		size.x - m_padding.left - m_padding.right,
		size.y - m_padding.top - m_padding.bottom
	);
}

Vector2 Layout::getContentOffset() const
{
	return Vector2(m_padding.left, m_padding.top);
}

AlignedVector<UIElement*> Layout::getLayoutChildren() const
{
	AlignedVector<UIElement*> layoutChildren;

	const AlignedVector<Ref<UIElement>>& children = getChildren();
	for (const auto& child : children)
	{
		if (!child)
			continue;

		// Skip invisible children if configured
		if (m_ignoreInvisibleChildren && !child->isVisible())
			continue;

		layoutChildren.push_back(child.ptr());
	}

	return layoutChildren;
}

}
