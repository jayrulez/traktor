/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Serialization/ISerializer.h"
#include "Core/Math/Color4f.h"
#include "Paper/Ui/UIElement.h"
#include "Paper/Ui/UIContext.h"
#include "Paper/Ui/UIStyle.h"
#include "Paper/Draw2D.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.paper.UIElement", UIElement, ISerializable)

void UIElement::applyStyle(const UIStyle* style)
{
	// Base implementation does nothing
	// Derived classes override to apply specific style properties
}

Vector2 UIElement::measure(const Vector2& availableSize, UIContext* context)
{
	m_desiredSize = Vector2::zero();
	return m_desiredSize;
}

void UIElement::arrange(const Vector2& position, const Vector2& size)
{
	m_actualPosition = position;
	m_actualSize = size;
}

void UIElement::render(UIContext* context)
{
	// Base implementation does nothing
}

void UIElement::renderDebug(UIContext* context)
{
	Draw2D* renderer = context->getRenderer();

	// Draw a semi-transparent colored rectangle to show the element's bounds
	const float borderThickness = 2.0f;

	// Draw filled background with transparency
	renderer->drawQuad(m_actualPosition, m_actualSize, Color4f(0.2f, 0.5f, 0.8f, 0.2f));

	// Draw border outline
	// Top border
	renderer->drawQuad(m_actualPosition, Vector2(m_actualSize.x, borderThickness), Color4f(0.2f, 0.8f, 1.0f, 0.8f));
	// Bottom border
	renderer->drawQuad(Vector2(m_actualPosition.x, m_actualPosition.y + m_actualSize.y - borderThickness), Vector2(m_actualSize.x, borderThickness), Color4f(0.2f, 0.8f, 1.0f, 0.8f));
	// Left border
	renderer->drawQuad(m_actualPosition, Vector2(borderThickness, m_actualSize.y), Color4f(0.2f, 0.8f, 1.0f, 0.8f));
	// Right border
	renderer->drawQuad(Vector2(m_actualPosition.x + m_actualSize.x - borderThickness, m_actualPosition.y), Vector2(borderThickness, m_actualSize.y), Color4f(0.2f, 0.8f, 1.0f, 0.8f));
}

UIElement* UIElement::hitTest(const Vector2& position)
{
	// Default implementation: return this element if point is within bounds
	if (containsPoint(position))
		return this;
	return nullptr;
}

bool UIElement::containsPoint(const Vector2& position) const
{
	// Check if position is within element's bounds
	return position.x >= m_actualPosition.x &&
	       position.y >= m_actualPosition.y &&
	       position.x < m_actualPosition.x + m_actualSize.x &&
	       position.y < m_actualPosition.y + m_actualSize.y;
}

void UIElement::onMouseMove(MouseEvent& event)
{
	// Base implementation does nothing
	// Derived classes can override to handle mouse movement
}

void UIElement::onMouseDown(MouseEvent& event)
{
	// Base implementation does nothing
	// Derived classes can override to handle mouse button press
}

void UIElement::onMouseUp(MouseEvent& event)
{
	// Base implementation does nothing
	// Derived classes can override to handle mouse button release
}

void UIElement::onMouseEnter(MouseEvent& event)
{
	m_isMouseOver = true;
	// Derived classes can override to handle mouse enter
}

void UIElement::onMouseLeave(MouseEvent& event)
{
	m_isMouseOver = false;
	// Derived classes can override to handle mouse leave
}

void UIElement::onFocus()
{
	m_isFocused = true;
	// Derived classes can override to handle focus gain
}

void UIElement::onBlur()
{
	m_isFocused = false;
	// Derived classes can override to handle focus loss
}

void UIElement::onKeyDown(KeyEvent& event)
{
	// Base implementation does nothing
	// Derived classes can override to handle key presses
}

void UIElement::onKeyUp(KeyEvent& event)
{
	// Base implementation does nothing
	// Derived classes can override to handle key releases
}

void UIElement::serialize(ISerializer& s)
{
	s >> Member< std::wstring >(L"name", m_name);
}

}
