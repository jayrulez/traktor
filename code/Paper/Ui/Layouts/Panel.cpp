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

void Panel::serialize(ISerializer& s)
{
	UIElement::serialize(s);
	s >> MemberRefArray< UIElement >(L"children", m_children);
}

}
