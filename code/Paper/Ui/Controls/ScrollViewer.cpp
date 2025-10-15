/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Paper/Ui/Controls/ScrollViewer.h"
#include "Paper/Ui/Controls/ScrollBar.h"
#include "Paper/Ui/UIContext.h"
#include "Paper/Ui/UIStyle.h"
#include "Paper/Draw2D.h"
#include "Core/Log/Log.h"
#include "Core/Serialization/ISerializer.h"
#include "Core/Serialization/MemberRef.h"
#include <algorithm>
#include <cmath>

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.ScrollViewer", 0, ScrollViewer, UIElement)

ScrollViewer::ScrollViewer()
{
	// Create scrollbars
	m_verticalScrollBar = new ScrollBar();
	m_verticalScrollBar->setOrientation(ScrollBarOrientation::Vertical);
	m_verticalScrollBar->setValueChangedCallback([this](float value) {
		onVerticalScrollChanged(value);
	});
	m_verticalScrollBar->setParent(this);

	m_horizontalScrollBar = new ScrollBar();
	m_horizontalScrollBar->setOrientation(ScrollBarOrientation::Horizontal);
	m_horizontalScrollBar->setValueChangedCallback([this](float value) {
		onHorizontalScrollChanged(value);
	});
	m_horizontalScrollBar->setParent(this);
}

void ScrollViewer::setContent(UIElement* content)
{
	m_content = content;
	if (m_content)
		m_content->setParent(this);
}

void ScrollViewer::scrollToVerticalOffset(float offset)
{
	m_verticalOffset = std::max(0.0f, std::min(1.0f, offset));
	if (m_verticalScrollBar)
		m_verticalScrollBar->setValue(m_verticalOffset);
}

void ScrollViewer::scrollToHorizontalOffset(float offset)
{
	m_horizontalOffset = std::max(0.0f, std::min(1.0f, offset));
	if (m_horizontalScrollBar)
		m_horizontalScrollBar->setValue(m_horizontalOffset);
}

void ScrollViewer::applyStyle(const UIStyle* style)
{
	// ScrollViewer can delegate style to scrollbars if needed
	if (style && m_verticalScrollBar)
		m_verticalScrollBar->applyStyle(style);
	if (style && m_horizontalScrollBar)
		m_horizontalScrollBar->applyStyle(style);
}

Vector2 ScrollViewer::measure(const Vector2& availableSize, UIContext* context)
{
	// First, apply our explicit sizing constraints (if any)
	float constrainedWidth = std::isnan(m_width) ? availableSize.x : m_width;
	constrainedWidth = std::min(constrainedWidth, m_maxWidth);
	constrainedWidth = std::max(constrainedWidth, m_minWidth);

	float constrainedHeight = std::isnan(m_height) ? availableSize.y : m_height;
	constrainedHeight = std::min(constrainedHeight, m_maxHeight);
	constrainedHeight = std::max(constrainedHeight, m_minHeight);

	// Measure content with infinite size to get its desired size
	if (m_content)
	{
		Vector2 infiniteSize(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		m_content->measure(infiniteSize, context);
	}

	// ScrollViewer takes constrained size (respects explicit sizing)
	m_desiredSize = Vector2(constrainedWidth, constrainedHeight);
	return m_desiredSize;
}

void ScrollViewer::arrange(const Vector2& position, const Vector2& size)
{
	UIElement::arrange(position, size);

	// Calculate viewport size (excluding scrollbar space)
	const float scrollBarSize = 16.0f;
	m_viewportSize = size;

	// Determine which scrollbars to show
	m_showVerticalScrollBar = false;
	m_showHorizontalScrollBar = false;

	if (m_content)
	{
		Vector2 contentDesiredSize = m_content->getDesiredSize();

		// Check if we need scrollbars based on visibility mode
		bool needVertical = false;
		bool needHorizontal = false;

		if (m_verticalScrollBarVisibility == ScrollBarVisibility::Visible)
			needVertical = true;
		else if (m_verticalScrollBarVisibility == ScrollBarVisibility::Auto)
			needVertical = contentDesiredSize.y > m_viewportSize.y;

		if (m_horizontalScrollBarVisibility == ScrollBarVisibility::Visible)
			needHorizontal = true;
		else if (m_horizontalScrollBarVisibility == ScrollBarVisibility::Auto)
			needHorizontal = contentDesiredSize.x > m_viewportSize.x;

		// Adjust viewport size if scrollbars are shown
		if (needVertical)
		{
			m_viewportSize.x -= scrollBarSize;
			m_showVerticalScrollBar = true;
		}

		if (needHorizontal)
		{
			m_viewportSize.y -= scrollBarSize;
			m_showHorizontalScrollBar = true;

			// Check again for vertical scrollbar if horizontal appeared
			if (!needVertical && m_verticalScrollBarVisibility == ScrollBarVisibility::Auto)
			{
				if (contentDesiredSize.y > m_viewportSize.y)
				{
					m_viewportSize.x -= scrollBarSize;
					m_showVerticalScrollBar = true;
				}
			}
		}

		// Calculate scrollable size
		m_contentSize = contentDesiredSize;
		m_scrollableSize.x = std::max(0.0f, m_contentSize.x - m_viewportSize.x);
		m_scrollableSize.y = std::max(0.0f, m_contentSize.y - m_viewportSize.y);

		// Arrange content at offset position
		Vector2 contentOffset = getContentOffset();
		Vector2 contentPosition = position + contentOffset;
		m_content->arrange(contentPosition, m_contentSize);

		// Update scrollbar states
		updateScrollBars();

		// Arrange scrollbars
		if (m_showVerticalScrollBar && m_verticalScrollBar)
		{
			Vector2 vsbPosition = position + Vector2(m_viewportSize.x, 0.0f);
			Vector2 vsbSize = Vector2(scrollBarSize, m_showHorizontalScrollBar ? m_viewportSize.y : size.y);
			m_verticalScrollBar->arrange(vsbPosition, vsbSize);
		}

		if (m_showHorizontalScrollBar && m_horizontalScrollBar)
		{
			Vector2 hsbPosition = position + Vector2(0.0f, m_viewportSize.y);
			Vector2 hsbSize = Vector2(m_showVerticalScrollBar ? m_viewportSize.x : size.x, scrollBarSize);
			m_horizontalScrollBar->arrange(hsbPosition, hsbSize);
		}
	}
}

void ScrollViewer::render(UIContext* context)
{
	Draw2D* renderer = context->getRenderer();
	if (!renderer)
		return;

	// Get viewport bounds for clipping
	Vector2 viewportPos = getViewportPosition();

	// Push scissor rect to clip content to viewport
	renderer->pushScissor(viewportPos, m_viewportSize);

	// Render content
	if (m_content)
		m_content->render(context);

	// Pop scissor rect
	renderer->popScissor();

	// Render scrollbars
	if (m_showVerticalScrollBar && m_verticalScrollBar)
		m_verticalScrollBar->render(context);

	if (m_showHorizontalScrollBar && m_horizontalScrollBar)
		m_horizontalScrollBar->render(context);
}

void ScrollViewer::renderDebug(UIContext* context)
{
	// Draw viewport bounds
	Draw2D* renderer = context->getRenderer();
	Vector2 viewportPos = getViewportPosition();

	const float borderThickness = 2.0f;
	Color4f debugColor(1.0f, 0.5f, 0.0f, 0.8f);

	// Draw viewport border
	renderer->drawQuad(viewportPos, Vector2(m_viewportSize.x, borderThickness), debugColor);
	renderer->drawQuad(Vector2(viewportPos.x, viewportPos.y + m_viewportSize.y - borderThickness),
		Vector2(m_viewportSize.x, borderThickness), debugColor);
	renderer->drawQuad(viewportPos, Vector2(borderThickness, m_viewportSize.y), debugColor);
	renderer->drawQuad(Vector2(viewportPos.x + m_viewportSize.x - borderThickness, viewportPos.y),
		Vector2(borderThickness, m_viewportSize.y), debugColor);

	// Render content debug
	if (m_content)
		m_content->renderDebug(context);

	// Render scrollbar debug
	if (m_showVerticalScrollBar && m_verticalScrollBar)
		m_verticalScrollBar->renderDebug(context);

	if (m_showHorizontalScrollBar && m_horizontalScrollBar)
		m_horizontalScrollBar->renderDebug(context);
}

UIElement* ScrollViewer::hitTest(const Vector2& position)
{
	// Check scrollbars first
	if (m_showVerticalScrollBar && m_verticalScrollBar)
	{
		UIElement* hit = m_verticalScrollBar->hitTest(position);
		if (hit)
			return hit;
	}

	if (m_showHorizontalScrollBar && m_horizontalScrollBar)
	{
		UIElement* hit = m_horizontalScrollBar->hitTest(position);
		if (hit)
			return hit;
	}

	// Check if hit is within viewport
	Vector2 viewportPos = getViewportPosition();
	bool inViewport = position.x >= viewportPos.x &&
	                  position.y >= viewportPos.y &&
	                  position.x < viewportPos.x + m_viewportSize.x &&
	                  position.y < viewportPos.y + m_viewportSize.y;

	if (inViewport && m_content)
	{
		UIElement* hit = m_content->hitTest(position);
		if (hit)
			return hit;
	}

	// Check if within our bounds
	if (containsPoint(position))
		return this;

	return nullptr;
}

void ScrollViewer::onMouseWheel(MouseWheelEvent& event)
{
	// Handle horizontal scrolling when Shift is held
	if (event.shift && m_scrollableSize.x > 0.0f && m_showHorizontalScrollBar)
	{
		// Scroll by a fixed amount per wheel notch
		const float scrollAmount = 60.0f;  // pixels to scroll per wheel notch
		const float scrollDelta = -event.delta * scrollAmount;  // Negative because wheel up = scroll left (decrease offset)

		// Convert pixel delta to normalized offset delta
		float offsetDelta = scrollDelta / m_scrollableSize.x;

		// Apply the scroll
		float newOffset = m_horizontalOffset + offsetDelta;
		scrollToHorizontalOffset(newOffset);

		event.handled = true;
	}
	// Handle vertical scrolling (default behavior)
	else if (m_scrollableSize.y > 0.0f && m_showVerticalScrollBar)
	{
		// Scroll by a fixed amount per wheel notch (e.g., 3 lines worth)
		const float scrollAmount = 60.0f;  // pixels to scroll per wheel notch
		const float scrollDelta = -event.delta * scrollAmount;  // Negative because wheel up = scroll up (decrease offset)

		// Convert pixel delta to normalized offset delta
		float offsetDelta = scrollDelta / m_scrollableSize.y;

		// Apply the scroll
		float newOffset = m_verticalOffset + offsetDelta;
		scrollToVerticalOffset(newOffset);

		event.handled = true;
	}
}

void ScrollViewer::serialize(ISerializer& s)
{
	UIElement::serialize(s);
	s >> MemberRef< UIElement >(L"content", m_content);

	// After deserialization, set parent pointer
	if (s.getDirection() == ISerializer::Direction::Read && m_content)
	{
		m_content->setParent(this);
	}
}

void ScrollViewer::updateScrollBars()
{
	// Update vertical scrollbar
	if (m_verticalScrollBar)
	{
		if (m_scrollableSize.y > 0.0f)
		{
			float viewportRatio = m_viewportSize.y / m_contentSize.y;
			m_verticalScrollBar->setViewportSize(viewportRatio);
		}
		else
		{
			m_verticalScrollBar->setViewportSize(1.0f);
		}
	}

	// Update horizontal scrollbar
	if (m_horizontalScrollBar)
	{
		if (m_scrollableSize.x > 0.0f)
		{
			float viewportRatio = m_viewportSize.x / m_contentSize.x;
			m_horizontalScrollBar->setViewportSize(viewportRatio);
		}
		else
		{
			m_horizontalScrollBar->setViewportSize(1.0f);
		}
	}
}

void ScrollViewer::onVerticalScrollChanged(float value)
{
	m_verticalOffset = value;

	// Immediately re-arrange content at new scroll position
	if (m_content)
	{
		Vector2 contentOffset = getContentOffset();
		Vector2 contentPosition = m_actualPosition + contentOffset;
		m_content->arrange(contentPosition, m_contentSize);
	}
}

void ScrollViewer::onHorizontalScrollChanged(float value)
{
	m_horizontalOffset = value;

	// Immediately re-arrange content at new scroll position
	if (m_content)
	{
		Vector2 contentOffset = getContentOffset();
		Vector2 contentPosition = m_actualPosition + contentOffset;
		m_content->arrange(contentPosition, m_contentSize);
	}
}

Vector2 ScrollViewer::getViewportPosition() const
{
	return m_actualPosition;
}

Vector2 ScrollViewer::getContentOffset() const
{
	Vector2 offset;
	offset.x = -m_horizontalOffset * m_scrollableSize.x;
	offset.y = -m_verticalOffset * m_scrollableSize.y;
	return offset;
}

}
