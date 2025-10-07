/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Visual.h"
#include "Shine/RenderContext.h"
#include <algorithm>

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Visual", Visual, UIElement)

Visual::Visual(const std::wstring& name)
:	UIElement(name)
{
}

Visual::~Visual()
{
}

void Visual::render(RenderContext* rc, float parentOpacity)
{
	// Base implementation renders children only
	// Derived classes override this to add their own rendering

	float finalOpacity = parentOpacity * m_color.getAlpha();

	// Render all children
	const AlignedVector<Ref<UIElement>>& children = getChildren();
	for (const auto& child : children)
	{
		if (!child)
			continue;

		Visual* visualChild = dynamic_type_cast<Visual*>(child.ptr());
		if (visualChild && visualChild->isVisibleInHierarchy())
		{
			visualChild->render(rc, finalOpacity);
		}
	}
}

void Visual::renderHierarchy(RenderContext* rc)
{
	if (!isVisibleInHierarchy())
		return;

	// Setup clipping if enabled
	if (m_clipChildren)
	{
		Aabb2 bounds = getWorldBounds();
		rc->pushClipRect(bounds);
	}

	// Call virtual render method
	onBeforeRender(rc);
	render(rc, 1.0f);
	onAfterRender(rc);

	// Restore clipping
	if (m_clipChildren)
	{
		rc->popClipRect();
	}
}

void Visual::setColor(const Color4f& color)
{
	if (m_color != color)
	{
		m_color = color;
		onVisualChanged();

		if (onColorChanged)
			onColorChanged();
	}
}

void Visual::setOpacity(float opacity)
{
	Color4f newColor = m_color;
	newColor.setAlpha(Scalar(opacity));
	setColor(newColor);
}

void Visual::setRenderOrder(int order)
{
	if (m_renderOrder != order)
	{
		m_renderOrder = order;
		onVisualChanged();
	}
}

void Visual::setClipChildren(bool clip)
{
	if (m_clipChildren != clip)
	{
		m_clipChildren = clip;
	}
}

void Visual::onBeforeRender(RenderContext* rc)
{
	// Override in derived classes if needed
}

void Visual::onAfterRender(RenderContext* rc)
{
	// Override in derived classes if needed
}

void Visual::onVisualChanged()
{
	// Override in derived classes if needed
}

}
