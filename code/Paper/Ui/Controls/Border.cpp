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
#include "Paper/Ui/UIStyle.h"
#include "Paper/Draw2D.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.Border", 0, Border, UIElement)

void Border::applyStyle(const UIStyle* style)
{
	UIElement::applyStyle(style);

	if (style)
	{
		// Apply background color from style
		Color4f background;
		if (style->tryGetColor(L"Background", background))
			m_background = background;

		// Apply border brush from style
		Color4f borderBrush;
		if (style->tryGetColor(L"BorderBrush", borderBrush))
			m_borderBrush = borderBrush;

		// Apply border thickness from style
		float borderThickness;
		if (style->tryGetDimension(L"BorderThickness", borderThickness))
			m_borderThickness = borderThickness;

		// Apply padding from style
		Vector2 padding;
		if (style->tryGetVector(L"Padding", padding))
			m_padding = padding;
	}
}

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
		// Set parent pointer
		m_child->setParent(this);

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
	if (context)
	{
		Draw2D* renderer = context->getRenderer();
		if (renderer)
		{
			// Render background
			if (m_background.getAlpha() > 0.0f)
			{
				renderer->drawQuad(m_actualPosition, m_actualSize, m_background);
			}

			// Render border
			if (m_borderThickness > 0.0f && m_borderBrush.getAlpha() > 0.0f)
			{
				// Top border
				renderer->drawQuad(m_actualPosition, Vector2(m_actualSize.x, m_borderThickness), m_borderBrush);
				// Bottom border
				renderer->drawQuad(Vector2(m_actualPosition.x, m_actualPosition.y + m_actualSize.y - m_borderThickness), Vector2(m_actualSize.x, m_borderThickness), m_borderBrush);
				// Left border
				renderer->drawQuad(m_actualPosition, Vector2(m_borderThickness, m_actualSize.y), m_borderBrush);
				// Right border
				renderer->drawQuad(Vector2(m_actualPosition.x + m_actualSize.x - m_borderThickness, m_actualPosition.y), Vector2(m_borderThickness, m_actualSize.y), m_borderBrush);
			}
		}
	}

	// Render child on top
	if (m_child)
		m_child->render(context);
}

void Border::renderDebug(UIContext* context)
{
	// Render debug visualization from base class
	UIElement::renderDebug(context);

	// Render the actual border background and borders (without child)
	if (context)
	{
		Draw2D* renderer = context->getRenderer();
		if (renderer)
		{
			// Render background
			if (m_background.getAlpha() > 0.0f)
			{
				renderer->drawQuad(m_actualPosition, m_actualSize, m_background);
			}

			// Render border
			if (m_borderThickness > 0.0f && m_borderBrush.getAlpha() > 0.0f)
			{
				// Top border
				renderer->drawQuad(m_actualPosition, Vector2(m_actualSize.x, m_borderThickness), m_borderBrush);
				// Bottom border
				renderer->drawQuad(Vector2(m_actualPosition.x, m_actualPosition.y + m_actualSize.y - m_borderThickness), Vector2(m_actualSize.x, m_borderThickness), m_borderBrush);
				// Left border
				renderer->drawQuad(m_actualPosition, Vector2(m_borderThickness, m_actualSize.y), m_borderBrush);
				// Right border
				renderer->drawQuad(Vector2(m_actualPosition.x + m_actualSize.x - m_borderThickness, m_actualPosition.y), Vector2(m_borderThickness, m_actualSize.y), m_borderBrush);
			}
		}
	}

	// Render child in debug mode
	if (m_child)
		m_child->renderDebug(context);
}

UIElement* Border::hitTest(const Vector2& position)
{
	// First check if position is within this border's bounds
	if (!containsPoint(position))
		return nullptr;

	// If we have a child, check child first (children are on top)
	if (m_child)
	{
		UIElement* childHit = m_child->hitTest(position);
		if (childHit)
			return childHit;
	}

	// No child hit, return this border
	return this;
}

void Border::onMouseEnter(MouseEvent& event)
{
	UIElement::onMouseEnter(event);

	// Save original border color and brighten it on hover
	m_originalBorderBrush = m_borderBrush;

	// Brighten the border color (multiply by 1.5 and clamp to 1.0)
	m_borderBrush = Color4f(
		min(m_borderBrush.getRed() * 1.5f, 1.0f),
		min(m_borderBrush.getGreen() * 1.5f, 1.0f),
		min(m_borderBrush.getBlue() * 1.5f, 1.0f),
		m_borderBrush.getAlpha()
	);
}

void Border::onMouseLeave(MouseEvent& event)
{
	UIElement::onMouseLeave(event);

	// Restore original border color
	m_borderBrush = m_originalBorderBrush;
}

void Border::serialize(ISerializer& s)
{
	UIElement::serialize(s);

	s >> MemberRef< UIElement >(L"child", m_child);
	s >> Member< Color4f >(L"background", m_background);
	s >> Member< Color4f >(L"borderBrush", m_borderBrush);
	s >> Member< float >(L"borderThickness", m_borderThickness);
	s >> Member< Vector2 >(L"padding", m_padding);

	// After deserialization, set parent pointer
	if (s.getDirection() == ISerializer::Direction::Read && m_child)
	{
		m_child->setParent(this);
	}
}

}
