/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Serialization/ISerializer.h"
#include "Core/Serialization/MemberRefArray.h"
#include "Paper/Ui/Layouts/Panel.h"
#include "Paper/Ui/UIContext.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.paper.Panel", Panel, UIElement)

void Panel::addChild(UIElement* child)
{
	if (child)
		m_children.push_back(child);
}

void Panel::removeChild(UIElement* child)
{
	m_children.remove(child);
}

void Panel::render(UIContext* context)
{
	// Render all children
	for (auto child : m_children)
	{
		if (child)
			child->render(context);
	}
}

void Panel::renderDebug(UIContext* context)
{
	// Render debug for this panel
	UIElement::renderDebug(context);

	// Render debug for all children
	for (auto child : m_children)
	{
		if (child)
			child->renderDebug(context);
	}
}

UIElement* Panel::hitTest(const Vector2& position)
{
	// First check if position is within this panel's bounds
	if (!containsPoint(position))
		return nullptr;

	// Check children in reverse order (last child is on top)
	for (int32_t i = (int32_t)m_children.size() - 1; i >= 0; --i)
	{
		if (m_children[i])
		{
			UIElement* childHit = m_children[i]->hitTest(position);
			if (childHit)
				return childHit;
		}
	}

	// No child hit, return this panel
	return this;
}

void Panel::serialize(ISerializer& s)
{
	UIElement::serialize(s);
	s >> MemberRefArray< UIElement >(L"children", m_children);
}

}
