/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/DropTarget.h"
#include "Shine/Widgets/Draggable.h"
#include "Shine/RenderContext.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.DropTarget", DropTarget, Interactable)

DropTarget::DropTarget(const std::wstring& name)
:	Interactable(name)
{
}

DropTarget::~DropTarget()
{
}

bool DropTarget::canAcceptDrop(Draggable* draggable) const
{
	if (!m_dropEnabled)
		return false;

	// Allow user validation callback
	if (onValidateDrop)
		return onValidateDrop(draggable);

	// Default: accept all drops
	return true;
}

void DropTarget::onDragEnter(Draggable* draggable)
{
	if (!m_dropEnabled)
		return;

	if (canAcceptDrop(draggable))
	{
		m_hoverDraggable = draggable;

		if (onHoverChanged)
			onHoverChanged(draggable);
	}
}

void DropTarget::onDragOver(Draggable* draggable, const Vector2& position)
{
	// Update hover state if needed
	if (m_hoverDraggable != draggable)
	{
		onDragEnter(draggable);
	}
}

void DropTarget::onDragLeave(Draggable* draggable)
{
	if (m_hoverDraggable == draggable)
	{
		m_hoverDraggable = nullptr;

		if (onHoverChanged)
			onHoverChanged(nullptr);
	}
}

void DropTarget::onDrop(Draggable* draggable)
{
	if (!m_dropEnabled)
		return;

	if (canAcceptDrop(draggable))
	{
		if (onDropAccepted)
			onDropAccepted(draggable);
	}

	m_hoverDraggable = nullptr;
}

void DropTarget::render(RenderContext* rc, float parentOpacity)
{
	// Draw highlight when draggable is hovering
	if (m_hoverDraggable)
	{
		rc->pushTransform(getPosition(), getRotation(), getScale());

		// Draw highlight border
		rc->drawRectOutline(
			Vector2(0, 0),
			getSize(),
			Color4f(0, 1, 1, 0.5f), // Cyan highlight
			3.0f
		);

		rc->popTransform();
	}

	// Let base class handle rendering (will render children)
	Interactable::render(rc, parentOpacity);
}

}
