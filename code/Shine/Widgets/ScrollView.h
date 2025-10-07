/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Shine/Interactable.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

class Panel;
class ScrollBar;

/*!
 * \brief ScrollView widget for scrollable content.
 *
 * ScrollView provides a viewport with scrollable content area, optional scrollbars,
 * and scroll inertia.
 *
 * Usage:
 *   Ref<ScrollView> scrollView = new ScrollView(L"ContentScroller");
 *   scrollView->setContentSize(Vector2(400, 2000)); // Large content
 *
 *   // Add content to scroll view's content area
 *   UIElement* contentArea = scrollView->getContentArea();
 *   contentArea->addChild(new Text(L"Item 1"));
 *   contentArea->addChild(new Text(L"Item 2"));
 *   // ... etc
 */
class T_DLLCLASS ScrollView : public Interactable
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Scroll mode
	 */
	enum class ScrollMode
	{
		Vertical,      // Vertical scrolling only
		Horizontal,    // Horizontal scrolling only
		Both           // Both directions
	};

	/*!
	 * \brief Scrollbar visibility mode
	 */
	enum class ScrollbarVisibility
	{
		AlwaysHidden,  // Never show scrollbars
		AlwaysVisible, // Always show scrollbars
		AutoHide       // Show only when content exceeds viewport
	};

	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit ScrollView(const std::wstring& name = L"ScrollView");

	/*!
	 * \brief Destructor
	 */
	virtual ~ScrollView();

	// === Scroll Position ===

	/*!
	 * \brief Get scroll offset
	 */
	Vector2 getScrollOffset() const { return m_scrollOffset; }

	/*!
	 * \brief Set scroll offset
	 */
	void setScrollOffset(const Vector2& offset);

	/*!
	 * \brief Scroll to position
	 */
	void scrollTo(const Vector2& position, bool animated = false);

	/*!
	 * \brief Scroll by delta
	 */
	void scrollBy(const Vector2& delta);

	// === Content ===

	/*!
	 * \brief Get content area (add children here)
	 */
	UIElement* getContentArea() const { return m_contentArea; }

	/*!
	 * \brief Get content size
	 */
	Vector2 getContentSize() const { return m_contentSize; }

	/*!
	 * \brief Set content size
	 */
	void setContentSize(const Vector2& size);

	// === Scroll Mode ===

	/*!
	 * \brief Get scroll mode
	 */
	ScrollMode getScrollMode() const { return m_scrollMode; }

	/*!
	 * \brief Set scroll mode
	 */
	void setScrollMode(ScrollMode mode);

	// === Scrollbars ===

	/*!
	 * \brief Get scrollbar visibility mode
	 */
	ScrollbarVisibility getScrollbarVisibility() const { return m_scrollbarVisibility; }

	/*!
	 * \brief Set scrollbar visibility
	 */
	void setScrollbarVisibility(ScrollbarVisibility visibility);

	/*!
	 * \brief Get horizontal scrollbar
	 */
	ScrollBar* getHorizontalScrollbar() const { return m_horizontalScrollbar; }

	/*!
	 * \brief Get vertical scrollbar
	 */
	ScrollBar* getVerticalScrollbar() const { return m_verticalScrollbar; }

	// === Inertia ===

	/*!
	 * \brief Get whether scroll inertia is enabled
	 */
	bool getInertiaEnabled() const { return m_inertiaEnabled; }

	/*!
	 * \brief Set inertia enabled
	 */
	void setInertiaEnabled(bool enabled) { m_inertiaEnabled = enabled; }

	/*!
	 * \brief Get scroll sensitivity
	 */
	float getScrollSensitivity() const { return m_scrollSensitivity; }

	/*!
	 * \brief Set scroll sensitivity
	 */
	void setScrollSensitivity(float sensitivity) { m_scrollSensitivity = sensitivity; }

	// === Visuals ===

	/*!
	 * \brief Get viewport panel
	 */
	Panel* getViewport() const { return m_viewport; }

	// === Events ===

	std::function<void(const Vector2&)> onScrollChanged;

	// === Interaction ===

	virtual bool onMouseButtonDown(MouseButton button, const Vector2& position) override;
	virtual bool onMouseButtonUp(MouseButton button, const Vector2& position) override;
	virtual bool onMouseMove(const Vector2& position) override;
	virtual bool onMouseWheel(float delta) override;

	virtual void update(float deltaTime) override;
	virtual void render(RenderContext* rc, float parentOpacity) override;

protected:
	Vector2 m_scrollOffset = Vector2(0, 0);
	Vector2 m_contentSize = Vector2(0, 0);
	ScrollMode m_scrollMode = ScrollMode::Vertical;
	ScrollbarVisibility m_scrollbarVisibility = ScrollbarVisibility::AutoHide;
	bool m_inertiaEnabled = true;
	float m_scrollSensitivity = 1.0f;

	// Inertia/dragging state
	bool m_dragging = false;
	Vector2 m_dragStartPosition;
	Vector2 m_dragStartScrollOffset;
	Vector2 m_velocity;
	float m_friction = 0.95f;

	// Child widgets
	Panel* m_viewport = nullptr;
	UIElement* m_contentArea = nullptr;
	ScrollBar* m_horizontalScrollbar = nullptr;
	ScrollBar* m_verticalScrollbar = nullptr;

	// Helper methods
	void createDefaultWidgets();
	void updateContentPosition();
	void updateScrollbars();
	void clampScrollOffset();
	Vector2 getMaxScrollOffset() const;
	bool needsHorizontalScrollbar() const;
	bool needsVerticalScrollbar() const;
};

}
