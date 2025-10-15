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
#include "Paper/Ui/UIContext.h"
#include "Paper/Ui/UIStyle.h"
#include "Paper/Draw2D.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.Rectangle", 0, Rectangle, UIElement)

void Rectangle::applyStyle(const UIStyle* style)
{
	UIElement::applyStyle(style);

	if (style)
	{
		// Apply fill color from style
		Color4f fill;
		if (style->tryGetColor(L"Fill", fill))
			m_fill = fill;

		// Apply width from style
		float width;
		if (style->tryGetDimension(L"Width", width))
			m_width = width;

		// Apply height from style
		float height;
		if (style->tryGetDimension(L"Height", height))
			m_height = height;
	}
}

Vector2 Rectangle::measure(const Vector2& availableSize, UIContext* context)
{
	// Use explicit width/height if set, otherwise use available size
	m_desiredSize.x = (m_width > 0.0f) ? m_width : availableSize.x;
	m_desiredSize.y = (m_height > 0.0f) ? m_height : availableSize.y;

	return m_desiredSize;
}

void Rectangle::render(UIContext* context)
{
	if (context)
	{
		Draw2D* renderer = context->getRenderer();
		if (renderer)
		{
			renderer->drawQuad(m_actualPosition, m_actualSize, m_fill);
		}
	}
}

void Rectangle::renderDebug(UIContext* context)
{
	// Render debug visualization from base class
	UIElement::renderDebug(context);

	// Also render the actual rectangle
	render(context);
}

void Rectangle::serialize(ISerializer& s)
{
	UIElement::serialize(s);

	s >> Member< Color4f >(L"fill", m_fill);
	s >> Member< float >(L"width", m_width);
	s >> Member< float >(L"height", m_height);
}

}
