/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/Scrollable.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Scrollable", Scrollable, Interactable)

Scrollable::Scrollable(const std::wstring& name)
:	Interactable(name)
{
}

Scrollable::~Scrollable()
{
}

void Scrollable::setScrollOffset(const Vector2& offset)
{
	m_scrollOffset = offset;
	clampScrollOffset();

	if (onScrollChanged)
		onScrollChanged(m_scrollOffset);
}

bool Scrollable::onMouseButtonDown(MouseButton button, const Vector2& position)
{
	if (!m_dragEnabled)
		return Interactable::onMouseButtonDown(button, position);

	if (button == MouseButton::Left)
	{
		m_isDragging = true;
		m_dragStart = position;
		m_lastDragPosition = position;
		m_dragVelocity = Vector2(0.0f, 0.0f);
		return true;
	}

	return Interactable::onMouseButtonDown(button, position);
}

bool Scrollable::onMouseButtonUp(MouseButton button, const Vector2& position)
{
	if (button == MouseButton::Left && m_isDragging)
	{
		m_isDragging = false;
		return true;
	}

	return Interactable::onMouseButtonUp(button, position);
}

bool Scrollable::onMouseMove(const Vector2& position)
{
	if (m_isDragging)
	{
		// Calculate drag delta
		Vector2 delta = position - m_lastDragPosition;

		// Apply scroll offset
		applyScrollOffset(delta);

		// Track velocity for inertia
		m_dragVelocity = delta;

		m_lastDragPosition = position;
		return true;
	}

	return Interactable::onMouseMove(position);
}

void Scrollable::update(float deltaTime)
{
	Interactable::update(deltaTime);

	// Apply inertia when not dragging
	if (!m_isDragging && m_dragInertia > 0.0f)
	{
		if (m_dragVelocity.length() > 0.01f)
		{
			// Apply velocity to scroll offset
			applyScrollOffset(m_dragVelocity);

			// Dampen velocity
			m_dragVelocity *= m_dragInertia;
		}
		else
		{
			m_dragVelocity = Vector2(0.0f, 0.0f);
		}
	}
}

void Scrollable::applyScrollOffset(const Vector2& offset)
{
	m_scrollOffset += offset;
	clampScrollOffset();

	if (onScrollChanged)
		onScrollChanged(m_scrollOffset);
}

void Scrollable::clampScrollOffset()
{
	m_scrollOffset.x = clamp(m_scrollOffset.x, m_scrollBounds.mn.x, m_scrollBounds.mx.x);
	m_scrollOffset.y = clamp(m_scrollOffset.y, m_scrollBounds.mn.y, m_scrollBounds.mx.y);
}

}
