/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Paper/Ui/Controls/ScrollBar.h"
#include "Paper/Ui/UIContext.h"
#include "Paper/Ui/UIStyle.h"
#include "Paper/Draw2D.h"
#include "Core/Serialization/ISerializer.h"
#include "Core/Serialization/Member.h"
#include <algorithm>

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.ScrollBar", 0, ScrollBar, UIElement)

ScrollBar::ScrollBar()
{
}

void ScrollBar::setValue(float value)
{
	float oldValue = m_value;
	m_value = std::max(0.0f, std::min(1.0f, value));

	// Trigger callback if value changed
	if (m_value != oldValue && m_valueChangedCallback)
		m_valueChangedCallback(m_value);
}

void ScrollBar::setViewportSize(float viewportSize)
{
	m_viewportSize = std::max(0.0f, std::min(1.0f, viewportSize));
}

void ScrollBar::applyStyle(const UIStyle* style)
{
	if (!style)
		return;

	Color4f trackColor;
	if (style->tryGetColor(L"TrackColor", trackColor))
		m_trackColor = trackColor;

	Color4f thumbColor;
	if (style->tryGetColor(L"ThumbColor", thumbColor))
		m_thumbColor = thumbColor;

	Color4f thumbHoverColor;
	if (style->tryGetColor(L"ThumbHoverColor", thumbHoverColor))
		m_thumbHoverColor = thumbHoverColor;

	Color4f thumbPressedColor;
	if (style->tryGetColor(L"ThumbPressedColor", thumbPressedColor))
		m_thumbPressedColor = thumbPressedColor;
}

Vector2 ScrollBar::measure(const Vector2& availableSize, UIContext* context)
{
	// ScrollBar has a fixed size depending on orientation
	if (m_orientation == ScrollBarOrientation::Vertical)
	{
		m_desiredSize = Vector2(16.0f, availableSize.y);
	}
	else
	{
		m_desiredSize = Vector2(availableSize.x, 16.0f);
	}

	return m_desiredSize;
}

void ScrollBar::arrange(const Vector2& position, const Vector2& size)
{
	UIElement::arrange(position, size);
}

void ScrollBar::render(UIContext* context)
{
	Draw2D* renderer = context->getRenderer();
	if (!renderer)
		return;

	// Draw track
	renderer->drawQuad(m_actualPosition, m_actualSize, m_trackColor);

	// Calculate thumb bounds
	Vector2 thumbPos = getThumbPosition();
	Vector2 thumbSize = getThumbSize();

	// Determine thumb color based on state
	Color4f thumbColor = m_thumbColor;
	if (m_isDragging)
		thumbColor = m_thumbPressedColor;
	else if (m_isThumbHovered)
		thumbColor = m_thumbHoverColor;

	// Draw thumb
	renderer->drawQuad(thumbPos, thumbSize, thumbColor);
}

void ScrollBar::onMouseDown(MouseEvent& event)
{
	Vector2 localPos = event.position - m_actualPosition;

	// Request mouse capture so we continue to receive events even if mouse moves outside
	event.capture = true;

	// Check if clicking on thumb
	if (isPointInThumb(event.position))
	{
		m_isDragging = true;
		m_dragStartPos = event.position;
		m_dragStartValue = m_value;
	}
	else
	{
		// Clicked on track - jump to that position
		float newValue = getValueFromPosition(event.position);
		setValue(newValue);
	}
}

void ScrollBar::onMouseMove(MouseEvent& event)
{
	// Update thumb hover state
	m_isThumbHovered = isPointInThumb(event.position);

	// Handle thumb dragging
	if (m_isDragging)
	{
		Vector2 delta = event.position - m_dragStartPos;
		float trackLength;
		float dragOffset;

		if (m_orientation == ScrollBarOrientation::Vertical)
		{
			trackLength = m_actualSize.y;
			dragOffset = delta.y;
		}
		else
		{
			trackLength = m_actualSize.x;
			dragOffset = delta.x;
		}

		// Calculate thumb size to get scrollable range
		float thumbLength = trackLength * m_viewportSize;
		float scrollableRange = trackLength - thumbLength;

		if (scrollableRange > 0.0f)
		{
			float valueDelta = dragOffset / scrollableRange;
			setValue(m_dragStartValue + valueDelta);
		}
	}
}

void ScrollBar::onMouseUp(MouseEvent& event)
{
	m_isDragging = false;
}

void ScrollBar::onMouseEnter(MouseEvent& event)
{
	UIElement::onMouseEnter(event);
	m_isThumbHovered = isPointInThumb(event.position);
}

void ScrollBar::onMouseLeave(MouseEvent& event)
{
	UIElement::onMouseLeave(event);
	m_isThumbHovered = false;
}

void ScrollBar::serialize(ISerializer& s)
{
	UIElement::serialize(s);
	// Orientation and viewport size are typically set programmatically, not serialized
}

Vector2 ScrollBar::getThumbPosition() const
{
	float trackLength;
	Vector2 thumbPos = m_actualPosition;

	if (m_orientation == ScrollBarOrientation::Vertical)
	{
		trackLength = m_actualSize.y;
		float thumbHeight = trackLength * m_viewportSize;
		float scrollableRange = trackLength - thumbHeight;
		thumbPos.y += m_value * scrollableRange;
	}
	else
	{
		trackLength = m_actualSize.x;
		float thumbWidth = trackLength * m_viewportSize;
		float scrollableRange = trackLength - thumbWidth;
		thumbPos.x += m_value * scrollableRange;
	}

	return thumbPos;
}

Vector2 ScrollBar::getThumbSize() const
{
	Vector2 thumbSize = m_actualSize;

	if (m_orientation == ScrollBarOrientation::Vertical)
	{
		thumbSize.y = m_actualSize.y * m_viewportSize;
		// Ensure minimum thumb size for usability
		thumbSize.y = std::max(thumbSize.y, 20.0f);
	}
	else
	{
		thumbSize.x = m_actualSize.x * m_viewportSize;
		// Ensure minimum thumb size for usability
		thumbSize.x = std::max(thumbSize.x, 20.0f);
	}

	return thumbSize;
}

bool ScrollBar::isPointInThumb(const Vector2& point) const
{
	Vector2 thumbPos = getThumbPosition();
	Vector2 thumbSize = getThumbSize();

	return point.x >= thumbPos.x &&
	       point.y >= thumbPos.y &&
	       point.x < thumbPos.x + thumbSize.x &&
	       point.y < thumbPos.y + thumbSize.y;
}

float ScrollBar::getValueFromPosition(const Vector2& position) const
{
	Vector2 localPos = position - m_actualPosition;
	float trackLength;
	float clickOffset;

	if (m_orientation == ScrollBarOrientation::Vertical)
	{
		trackLength = m_actualSize.y;
		clickOffset = localPos.y;
	}
	else
	{
		trackLength = m_actualSize.x;
		clickOffset = localPos.x;
	}

	// Calculate thumb size to get scrollable range
	float thumbLength = trackLength * m_viewportSize;
	float scrollableRange = trackLength - thumbLength;

	if (scrollableRange <= 0.0f)
		return 0.0f;

	// Center the thumb on the click position
	float targetThumbCenter = clickOffset;
	float targetThumbStart = targetThumbCenter - (thumbLength / 2.0f);

	return targetThumbStart / scrollableRange;
}

}
