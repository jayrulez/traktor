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
#include "Paper/Ui/Controls/Rectangle.h"
#include "Paper/Draw2D.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.Rectangle", 0, Rectangle, UIElement)

Vector2 Rectangle::measure(const Vector2& availableSize)
{
	// Use explicit width/height if set, otherwise use available size
	m_desiredSize.x = (m_width > 0.0f) ? m_width : availableSize.x;
	m_desiredSize.y = (m_height > 0.0f) ? m_height : availableSize.y;

	return m_desiredSize;
}

void Rectangle::render(Draw2D* renderer)
{
	// TODO: Render filled rectangle using Draw2D
	// renderer->drawFilledQuad(m_actualPosition, m_actualSize, m_fill);
}

void Rectangle::serialize(ISerializer& s)
{
	UIElement::serialize(s);

	s >> Member< Color4f >(L"fill", m_fill);
	s >> Member< float >(L"width", m_width);
	s >> Member< float >(L"height", m_height);
}

}
