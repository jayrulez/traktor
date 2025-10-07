/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/ScrollView.h"
#include "Shine/Widgets/Panel.h"
#include "Shine/Widgets/ScrollBar.h"
#include "Shine/RenderContext.h"
#include <algorithm>

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.ScrollView", ScrollView, Interactable)

ScrollView::ScrollView(const std::wstring& name)
:	Interactable(name)
{
	createDefaultWidgets();
	updateScrollbars();
}

ScrollView::~ScrollView()
{
}

void ScrollView::setScrollOffset(const Vector2& offset)
{
	Vector2 newOffset = offset;

	// Apply scroll mode restrictions
	if (m_scrollMode == ScrollMode::Vertical)
		newOffset.x = 0;
	else if (m_scrollMode == ScrollMode::Horizontal)
		newOffset.y = 0;

	if (m_scrollOffset != newOffset)
	{
		m_scrollOffset = newOffset;
		clampScrollOffset();
		updateContentPosition();
		updateScrollbars();

		if (onScrollChanged)
			onScrollChanged(m_scrollOffset);
	}
}

void ScrollView::scrollTo(const Vector2& position, bool animated)
{
	// TODO: Implement animated scrolling
	setScrollOffset(position);
}

void ScrollView::scrollBy(const Vector2& delta)
{
	setScrollOffset(m_scrollOffset + delta);
}

void ScrollView::setContentSize(const Vector2& size)
{
	if (m_contentSize != size)
	{
		m_contentSize = size;

		if (m_contentArea)
			m_contentArea->setSize(size);

		clampScrollOffset();
		updateContentPosition();
		updateScrollbars();
	}
}

void ScrollView::setScrollMode(ScrollMode mode)
{
	if (m_scrollMode != mode)
	{
		m_scrollMode = mode;
		clampScrollOffset();
		updateScrollbars();
	}
}

void ScrollView::setScrollbarVisibility(ScrollbarVisibility visibility)
{
	if (m_scrollbarVisibility != visibility)
	{
		m_scrollbarVisibility = visibility;
		updateScrollbars();
	}
}

bool ScrollView::onMouseButtonDown(MouseButton button, const Vector2& position)
{
	if (!Interactable::onMouseButtonDown(button, position))
		return false;

	if (button == MouseButton::Left)
	{
		m_dragging = true;
		m_dragStartPosition = position;
		m_dragStartScrollOffset = m_scrollOffset;
		m_velocity = Vector2(0, 0);
		return true;
	}

	return false;
}

bool ScrollView::onMouseButtonUp(MouseButton button, const Vector2& position)
{
	bool handled = Interactable::onMouseButtonUp(button, position);

	if (button == MouseButton::Left && m_dragging)
	{
		m_dragging = false;
		return true;
	}

	return handled;
}

bool ScrollView::onMouseMove(const Vector2& position)
{
	bool handled = Interactable::onMouseMove(position);

	if (m_dragging)
	{
		Vector2 delta = position - m_dragStartPosition;
		Vector2 newOffset = m_dragStartScrollOffset - delta; // Negative for natural scrolling

		setScrollOffset(newOffset);

		// Track velocity for inertia
		if (m_inertiaEnabled)
			m_velocity = -delta * 0.1f; // Scale down velocity

		return true;
	}

	return handled;
}

bool ScrollView::onMouseWheel(float delta)
{
	if (!isInteractable())
		return false;

	// Scroll with mouse wheel
	float scrollAmount = delta * 50.0f * m_scrollSensitivity;

	if (m_scrollMode == ScrollMode::Vertical || m_scrollMode == ScrollMode::Both)
		scrollBy(Vector2(0, -scrollAmount));
	else if (m_scrollMode == ScrollMode::Horizontal)
		scrollBy(Vector2(-scrollAmount, 0));

	m_velocity = Vector2(0, 0); // Cancel inertia
	return true;
}

void ScrollView::update(float deltaTime)
{
	Interactable::update(deltaTime);

	// Apply inertia
	if (m_inertiaEnabled && !m_dragging && (m_velocity.x != 0 || m_velocity.y != 0))
	{
		// Apply velocity
		scrollBy(m_velocity * deltaTime * 60.0f); // Scale by frame time

		// Apply friction
		m_velocity = m_velocity * m_friction;

		// Stop if velocity is negligible
		if (std::abs(m_velocity.x) < 0.1f && std::abs(m_velocity.y) < 0.1f)
			m_velocity = Vector2(0, 0);
	}
}

void ScrollView::render(RenderContext* rc, float parentOpacity)
{
	// Let base class handle rendering (will render children with clipping)
	Interactable::render(rc, parentOpacity);
}

void ScrollView::createDefaultWidgets()
{
	Vector2 size = getSize();
	if (size.x < 20) size.x = 400;
	if (size.y < 20) size.y = 300;
	setSize(size);

	// Create viewport (clips content)
	Ref<Panel> viewport = new Panel(L"Viewport");
	viewport->setPosition(Vector2(0, 0));
	viewport->setSize(size);
	viewport->setClipChildren(true);
	viewport->setBackgroundColor(Color4f(0.1f, 0.1f, 0.1f, 1));
	addChild(viewport);
	m_viewport = viewport;

	// Viewport anchoring - resize with parent
	viewport->setAnchorMin(Vector2(0, 0));
	viewport->setAnchorMax(Vector2(1, 1));
	viewport->setOffsetMin(Vector2(0, 0));
	viewport->setOffsetMax(Vector2(-20, -20)); // Leave space for scrollbars

	// Create content area (holds scrollable content)
	Ref<UIElement> contentArea = new UIElement(L"ContentArea");
	contentArea->setPosition(Vector2(0, 0));
	contentArea->setSize(m_contentSize);
	viewport->addChild(contentArea);
	m_contentArea = contentArea;

	// Create vertical scrollbar
	Ref<ScrollBar> vScrollbar = new ScrollBar(L"VerticalScrollbar");
	vScrollbar->setOrientation(ScrollBar::Orientation::Vertical);
	vScrollbar->setPosition(Vector2(size.x - 16, 0));
	vScrollbar->setSize(Vector2(16, size.y - 20));
	vScrollbar->onValueChanged = [this](float value) {
		Vector2 maxOffset = getMaxScrollOffset();
		setScrollOffset(Vector2(m_scrollOffset.x, maxOffset.y * value));
	};
	addChild(vScrollbar);
	m_verticalScrollbar = vScrollbar;

	// Vertical scrollbar anchoring
	vScrollbar->setAnchorMin(Vector2(1, 0));
	vScrollbar->setAnchorMax(Vector2(1, 1));
	vScrollbar->setOffsetMin(Vector2(-16, 0));
	vScrollbar->setOffsetMax(Vector2(0, -20));

	// Create horizontal scrollbar
	Ref<ScrollBar> hScrollbar = new ScrollBar(L"HorizontalScrollbar");
	hScrollbar->setOrientation(ScrollBar::Orientation::Horizontal);
	hScrollbar->setPosition(Vector2(0, size.y - 16));
	hScrollbar->setSize(Vector2(size.x - 20, 16));
	hScrollbar->onValueChanged = [this](float value) {
		Vector2 maxOffset = getMaxScrollOffset();
		setScrollOffset(Vector2(maxOffset.x * value, m_scrollOffset.y));
	};
	addChild(hScrollbar);
	m_horizontalScrollbar = hScrollbar;

	// Horizontal scrollbar anchoring
	hScrollbar->setAnchorMin(Vector2(0, 1));
	hScrollbar->setAnchorMax(Vector2(1, 1));
	hScrollbar->setOffsetMin(Vector2(0, -16));
	hScrollbar->setOffsetMax(Vector2(-20, 0));
}

void ScrollView::updateContentPosition()
{
	if (m_contentArea)
	{
		m_contentArea->setPosition(-m_scrollOffset);
	}
}

void ScrollView::updateScrollbars()
{
	Vector2 viewportSize = m_viewport ? m_viewport->getSize() : getSize();
	bool showHScroll = false;
	bool showVScroll = false;

	// Determine scrollbar visibility
	switch (m_scrollbarVisibility)
	{
	case ScrollbarVisibility::AlwaysHidden:
		showHScroll = false;
		showVScroll = false;
		break;

	case ScrollbarVisibility::AlwaysVisible:
		showHScroll = (m_scrollMode == ScrollMode::Horizontal || m_scrollMode == ScrollMode::Both);
		showVScroll = (m_scrollMode == ScrollMode::Vertical || m_scrollMode == ScrollMode::Both);
		break;

	case ScrollbarVisibility::AutoHide:
		showHScroll = needsHorizontalScrollbar();
		showVScroll = needsVerticalScrollbar();
		break;
	}

	// Update scrollbar visibility
	if (m_horizontalScrollbar)
		m_horizontalScrollbar->setVisible(showHScroll);

	if (m_verticalScrollbar)
		m_verticalScrollbar->setVisible(showVScroll);

	// Update scrollbar values
	Vector2 maxOffset = getMaxScrollOffset();

	if (m_verticalScrollbar && maxOffset.y > 0)
	{
		m_verticalScrollbar->setContentSize(m_contentSize.y);
		m_verticalScrollbar->setViewportSize(viewportSize.y);
		m_verticalScrollbar->setValue(m_scrollOffset.y / maxOffset.y);
	}

	if (m_horizontalScrollbar && maxOffset.x > 0)
	{
		m_horizontalScrollbar->setContentSize(m_contentSize.x);
		m_horizontalScrollbar->setViewportSize(viewportSize.x);
		m_horizontalScrollbar->setValue(m_scrollOffset.x / maxOffset.x);
	}
}

void ScrollView::clampScrollOffset()
{
	Vector2 maxOffset = getMaxScrollOffset();

	m_scrollOffset.x = clamp(m_scrollOffset.x, 0.0f, maxOffset.x);
	m_scrollOffset.y = clamp(m_scrollOffset.y, 0.0f, maxOffset.y);

	// Apply scroll mode restrictions
	if (m_scrollMode == ScrollMode::Vertical)
		m_scrollOffset.x = 0;
	else if (m_scrollMode == ScrollMode::Horizontal)
		m_scrollOffset.y = 0;
}

Vector2 ScrollView::getMaxScrollOffset() const
{
	Vector2 viewportSize = m_viewport ? m_viewport->getSize() : getSize();
	Vector2 maxOffset;

	maxOffset.x = std::max(0.0f, m_contentSize.x - viewportSize.x);
	maxOffset.y = std::max(0.0f, m_contentSize.y - viewportSize.y);

	return maxOffset;
}

bool ScrollView::needsHorizontalScrollbar() const
{
	if (m_scrollMode == ScrollMode::Vertical)
		return false;

	Vector2 viewportSize = m_viewport ? m_viewport->getSize() : getSize();
	return m_contentSize.x > viewportSize.x;
}

bool ScrollView::needsVerticalScrollbar() const
{
	if (m_scrollMode == ScrollMode::Horizontal)
		return false;

	Vector2 viewportSize = m_viewport ? m_viewport->getSize() : getSize();
	return m_contentSize.y > viewportSize.y;
}

}
