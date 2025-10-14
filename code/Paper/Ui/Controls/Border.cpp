/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Serialization/ISerializer.h"
#include "Core/Serialization/Member.h"
#include "Core/Serialization/MemberRef.h"
#include "Paper/Ui/Controls/Border.h"
#include "Paper/Ui/UIContext.h"
#include "Paper/Draw2D.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.Border", 0, Border, UIElement)

Vector2 Border::measure(const Vector2& availableSize, UIContext* context)
{
	Vector2 childAvailable = availableSize;

	// Subtract padding and border from available space
	float totalThickness = m_borderThickness * 2.0f;
	childAvailable.x = max(0.0f, childAvailable.x - m_padding.x * 2.0f - totalThickness);
	childAvailable.y = max(0.0f, childAvailable.y - m_padding.y * 2.0f - totalThickness);

	Vector2 childSize = Vector2::zero();
	if (m_child)
		childSize = m_child->measure(childAvailable, context);

	// Add padding and border back to desired size
	m_desiredSize = childSize + m_padding * 2.0f + Vector2(totalThickness, totalThickness);
	return m_desiredSize;
}

void Border::arrange(const Vector2& position, const Vector2& size)
{
	UIElement::arrange(position, size);

	if (m_child)
	{
		// Calculate child position and size
		float totalThickness = m_borderThickness * 2.0f;
		Vector2 childPos = position + m_padding + Vector2(m_borderThickness, m_borderThickness);
		Vector2 childSize = size - m_padding * 2.0f - Vector2(totalThickness, totalThickness);

		childSize.x = max(0.0f, childSize.x);
		childSize.y = max(0.0f, childSize.y);

		m_child->arrange(childPos, childSize);
	}
}

void Border::render(UIContext* context)
{
	// TODO: Render background rectangle
	// TODO: Render border rectangle

	// Render child
	if (m_child)
		m_child->render(context);
}

void Border::serialize(ISerializer& s)
{
	UIElement::serialize(s);

	s >> MemberRef< UIElement >(L"child", m_child);
	s >> Member< Color4f >(L"background", m_background);
	s >> Member< Color4f >(L"borderBrush", m_borderBrush);
	s >> Member< float >(L"borderThickness", m_borderThickness);
	s >> Member< Vector2 >(L"padding", m_padding);
}

}
