/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Paper/Ui/Layouts/StackPanel.h"

#include "Core/Serialization/ISerializer.h"
#include "Core/Serialization/Member.h"
#include "Core/Serialization/MemberEnum.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.StackPanel", 0, StackPanel, Panel)

Vector2 StackPanel::measure(const Vector2& availableSize)
{
	Vector2 desiredSize = Vector2::zero();

	if (m_orientation == Orientation::Vertical)
	{
		// Vertical stack: height accumulates, width is max
		for (auto child : m_children)
		{
			if (child)
			{
				Vector2 childSize = child->measure(Vector2(availableSize.x, std::numeric_limits< float >::infinity()));
				desiredSize.x = max(desiredSize.x, childSize.x);
				desiredSize.y += childSize.y;
			}
		}
	}
	else
	{
		// Horizontal stack: width accumulates, height is max
		for (auto child : m_children)
		{
			if (child)
			{
				Vector2 childSize = child->measure(Vector2(std::numeric_limits< float >::infinity(), availableSize.y));
				desiredSize.x += childSize.x;
				desiredSize.y = max(desiredSize.y, childSize.y);
			}
		}
	}

	m_desiredSize = desiredSize;
	return m_desiredSize;
}

void StackPanel::arrange(const Vector2& position, const Vector2& size)
{
	UIElement::arrange(position, size);

	Vector2 currentPos = position;

	if (m_orientation == Orientation::Vertical)
	{
		// Arrange children vertically
		for (auto child : m_children)
		{
			if (child)
			{
				Vector2 childSize = child->getDesiredSize();
				child->arrange(currentPos, Vector2(size.x, childSize.y));
				currentPos.y += childSize.y;
			}
		}
	}
	else
	{
		// Arrange children horizontally
		for (auto child : m_children)
		{
			if (child)
			{
				Vector2 childSize = child->getDesiredSize();
				child->arrange(currentPos, Vector2(childSize.x, size.y));
				currentPos.x += childSize.x;
			}
		}
	}
}

void StackPanel::serialize(ISerializer& s)
{
	const MemberEnum< Orientation >::Key c_orientation_Keys[] = {
		{ L"Vertical", Orientation::Vertical },
		{ L"Horizontal", Orientation::Horizontal },
		{ 0 }
	};

	Panel::serialize(s);

	if (s.getVersion() >= 0)
		s >> MemberEnum< Orientation >(L"orientation", m_orientation, c_orientation_Keys);
}

}
