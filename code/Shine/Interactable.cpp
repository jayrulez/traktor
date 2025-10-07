/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Interactable.h"
#include "Shine/Canvas.h"
#include "Core/Timer/Timer.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Interactable", Interactable, Visual)

Interactable::Interactable(const std::wstring& name)
:	Visual(name)
{
}

Interactable::~Interactable()
{
}

bool Interactable::onMouseButtonDown(MouseButton button, const Vector2& position)
{
	if (!isInteractable() || !hitTest(position))
		return false;

	if (button == MouseButton::Left)
	{
		setPressed(true);
		focus();

		// Store drag start position
		m_dragStartPosition = position;
		m_dragging = false;

		if (onPressStart)
			onPressStart();

		return true;
	}

	return false;
}

bool Interactable::onMouseButtonUp(MouseButton button, const Vector2& position)
{
	if (!isInteractable())
		return false;

	if (button == MouseButton::Left && m_pressed)
	{
		setPressed(false);

		if (onPressEnd)
			onPressEnd();

		// End drag if dragging
		if (m_dragging)
		{
			m_dragging = false;
			if (onDragEnd)
				onDragEnd(position);
		}
		// Only trigger click if we're still inside bounds and not dragging
		else if (hitTest(position))
		{
			// Check for double-click
			double currentTime = m_clickTimer.getElapsedTime();
			if (currentTime - m_lastClickTime < m_doubleClickThreshold)
			{
				if (onDoubleClick)
					onDoubleClick();
				m_lastClickTime = 0.0f; // Reset to prevent triple-click
			}
			else
			{
				if (onClick)
					onClick();
				m_lastClickTime = currentTime;
			}
		}

		return true;
	}

	return false;
}

bool Interactable::onMouseMove(const Vector2& position)
{
	if (!isInteractable())
		return false;

	bool inside = hitTest(position);

	// Update hover state
	if (inside && !m_hovered)
	{
		onMouseEnter();
	}
	else if (!inside && m_hovered)
	{
		onMouseLeave();
	}

	// Handle dragging
	if (m_pressed)
	{
		Vector2 dragDelta = position - m_dragStartPosition;
		float dragDistance = std::sqrt(dragDelta.x * dragDelta.x + dragDelta.y * dragDelta.y);

		if (!m_dragging && dragDistance >= m_dragThreshold)
		{
			// Start drag
			m_dragging = true;
			if (onDragStart)
				onDragStart(m_dragStartPosition);
		}

		if (m_dragging)
		{
			if (onDrag)
				onDrag(position);
			return true;
		}
	}

	return inside;
}

void Interactable::onMouseEnter()
{
	setHovered(true);

	if (onHoverStart)
		onHoverStart();
}

void Interactable::onMouseLeave()
{
	setHovered(false);

	if (onHoverEnd)
		onHoverEnd();
}

bool Interactable::onMouseWheel(float delta)
{
	if (!isInteractable())
		return false;

	// Base implementation does nothing
	// Override in derived classes (e.g., ScrollView)
	return false;
}

bool Interactable::onKeyDown(int key)
{
	if (!isInteractable() || !m_focused)
		return false;

	// Base implementation does nothing
	// Override in derived classes (e.g., TextInput)
	return false;
}

bool Interactable::onKeyUp(int key)
{
	if (!isInteractable() || !m_focused)
		return false;

	// Base implementation does nothing
	// Override in derived classes
	return false;
}

bool Interactable::onTextInput(wchar_t character)
{
	if (!isInteractable() || !m_focused)
		return false;

	// Base implementation does nothing
	// Override in derived classes (e.g., TextInput)
	return false;
}

void Interactable::onFocusGained()
{
	// Override in derived classes if needed
}

void Interactable::onFocusLost()
{
	// Override in derived classes if needed
}

bool Interactable::hitTest(const Vector2& worldPosition) const
{
	if (!isVisibleInHierarchy())
		return false;

	// Simple AABB test
	Aabb2 bounds = getWorldBounds();
	return bounds.inside(worldPosition);
}

bool Interactable::isInteractable() const
{
	return isEnabledInHierarchy() && isVisibleInHierarchy();
}

void Interactable::focus()
{
	if (m_focused)
		return;

	// Remove focus from other elements in canvas
	Canvas* canvas = getCanvas();
	if (canvas)
	{
		// TODO: Canvas needs to track focused element
		// For now, just set our state
	}

	setFocused(true);
	onFocusGained();
}

void Interactable::unfocus()
{
	if (!m_focused)
		return;

	setFocused(false);
	onFocusLost();
}

void Interactable::setHovered(bool hovered)
{
	if (m_hovered != hovered)
	{
		m_hovered = hovered;
		onInteractionStateChanged();
	}
}

void Interactable::setPressed(bool pressed)
{
	if (m_pressed != pressed)
	{
		m_pressed = pressed;
		onInteractionStateChanged();
	}
}

void Interactable::setFocused(bool focused)
{
	if (m_focused != focused)
	{
		m_focused = focused;
		onInteractionStateChanged();

		if (onFocusChanged)
			onFocusChanged();
	}
}

void Interactable::onInteractionStateChanged()
{
	// Override in derived classes if needed
}

}
