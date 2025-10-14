/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Serialization/ISerializer.h"
#include "Paper/Ui/UIElement.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.paper.UIElement", UIElement, ISerializable)

Vector2 UIElement::measure(const Vector2& availableSize)
{
	m_desiredSize = Vector2::zero();
	return m_desiredSize;
}

void UIElement::arrange(const Vector2& position, const Vector2& size)
{
	m_actualPosition = position;
	m_actualSize = size;
}

void UIElement::render(Draw2D* renderer)
{
	// Base implementation does nothing
}

void UIElement::serialize(ISerializer& s)
{
	// Base serialization - derived classes can add their own members
}

}
