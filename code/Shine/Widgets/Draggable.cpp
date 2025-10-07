/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/Draggable.h"
#include "Shine/Widgets/DropTarget.h"
#include "Shine/Canvas.h"
#include "Shine/RenderContext.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Draggable", Draggable, Interactable)

Draggable::Draggable(const std::wstring& name)
:	Interactable(name)
{
}

Draggable::~Draggable()
{
}

bool Draggable::onMouseButtonDown(MouseButton button, const Vector2& position)
{
	if (!Interactable::onMouseButtonDown(button, position))
		return false;

	if (button == MouseButton::Left && m_dragEnabled)
	{
		m_dragStartPosition = position;
		m_originalPosition = getPosition();
		return true;
	}

	return false;
}

bool Draggable::onMouseButtonUp(MouseButton button, const Vector2& position)
{
	bool handled = Interactable::onMouseButtonUp(button, position);

	if (button == MouseButton::Left && m_isDragging)
	{
		endDrag(false);
		return true;
	}

	return handled;
}

bool Draggable::onMouseMove(const Vector2& position)
{
	bool handled = Interactable::onMouseMove(position);

	if (isPressed() && m_dragEnabled)
	{
		Vector2 delta = position - m_dragStartPosition;
		float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);

		// Check if drag threshold exceeded
		if (!m_isDragging && distance >= m_dragThreshold)
		{
			startDrag();
		}

		if (m_isDragging)
		{
			updateDrag(position);
			return true;
		}
	}

	return handled;
}

void Draggable::update(float deltaTime)
{
	Interactable::update(deltaTime);
}

void Draggable::render(RenderContext* rc, float parentOpacity)
{
	// Render with slightly reduced opacity while dragging
	float opacity = m_isDragging ? parentOpacity * 0.7f : parentOpacity;

	Interactable::render(rc, opacity);
}

void Draggable::startDrag()
{
	m_isDragging = true;
	m_dragOffset = Vector2(0, 0);

	if (onDragBegin)
		onDragBegin();
}

void Draggable::updateDrag(const Vector2& position)
{
	// Calculate drag offset
	m_dragOffset = position - m_dragStartPosition;

	// Move to new position
	setPosition(m_originalPosition + m_dragOffset);

	// Find drop target under cursor
	DropTarget* newTarget = findDropTargetAt(position);

	// Handle drop target changes
	if (newTarget != m_currentDropTarget)
	{
		if (m_currentDropTarget)
			m_currentDropTarget->onDragLeave(this);

		m_currentDropTarget = newTarget;

		if (m_currentDropTarget)
			m_currentDropTarget->onDragEnter(this);
	}

	// Update current drop target
	if (m_currentDropTarget)
		m_currentDropTarget->onDragOver(this, position);

	// Fire dragging event
	if (onDragging)
		onDragging(position);
}

void Draggable::endDrag(bool cancelled)
{
	m_isDragging = false;

	if (cancelled)
	{
		// Reset to original position
		setPosition(m_originalPosition);

		if (onDragCancelled)
			onDragCancelled();
	}
	else
	{
		// Notify drop target
		if (m_currentDropTarget)
		{
			m_currentDropTarget->onDrop(this);
			m_currentDropTarget->onDragLeave(this);
		}

		if (onDragEnd)
			onDragEnd(m_currentDropTarget);

		// Reset position if no valid drop
		if (!m_currentDropTarget)
			setPosition(m_originalPosition);
	}

	m_currentDropTarget = nullptr;
	m_dragOffset = Vector2(0, 0);
}

DropTarget* Draggable::findDropTargetAt(const Vector2& position)
{
	// TODO: Search canvas for drop targets at position
	// For now, return nullptr
	// This would require traversing the UI hierarchy looking for DropTarget widgets
	return nullptr;
}

}
