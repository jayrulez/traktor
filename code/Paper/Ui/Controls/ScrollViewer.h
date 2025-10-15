/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Ref.h"
#include "Core/Math/Vector2.h"
#include "Paper/Ui/UIElement.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_PAPER_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::paper
{

class ScrollBar;
class UIContext;

/*! ScrollViewer scrollbar visibility modes. */
enum class ScrollBarVisibility
{
	Auto,      // Show scrollbar only when content exceeds viewport
	Visible,   // Always show scrollbar
	Hidden     // Never show scrollbar
};

/*! ScrollViewer control.
 * \ingroup Paper
 *
 * A container that provides a scrollable viewport for content larger than the available space.
 * Automatically shows/hides scrollbars based on content size.
 */
class T_DLLCLASS ScrollViewer : public UIElement
{
	T_RTTI_CLASS;

public:
	ScrollViewer();

	/*! Set the content element to be scrolled. */
	void setContent(UIElement* content);

	UIElement* getContent() const { return m_content; }

	/*! Set vertical scrollbar visibility. */
	void setVerticalScrollBarVisibility(ScrollBarVisibility visibility) { m_verticalScrollBarVisibility = visibility; }

	ScrollBarVisibility getVerticalScrollBarVisibility() const { return m_verticalScrollBarVisibility; }

	/*! Set horizontal scrollbar visibility. */
	void setHorizontalScrollBarVisibility(ScrollBarVisibility visibility) { m_horizontalScrollBarVisibility = visibility; }

	ScrollBarVisibility getHorizontalScrollBarVisibility() const { return m_horizontalScrollBarVisibility; }

	/*! Get current vertical scroll offset (0.0 to 1.0). */
	float getVerticalOffset() const { return m_verticalOffset; }

	/*! Get current horizontal scroll offset (0.0 to 1.0). */
	float getHorizontalOffset() const { return m_horizontalOffset; }

	/*! Scroll to a specific vertical offset (0.0 to 1.0). */
	void scrollToVerticalOffset(float offset);

	/*! Scroll to a specific horizontal offset (0.0 to 1.0). */
	void scrollToHorizontalOffset(float offset);

	virtual void applyStyle(const UIStyle* style) override;

	virtual Vector2 measure(const Vector2& availableSize, UIContext* context) override;

	virtual void arrange(const Vector2& position, const Vector2& size) override;

	virtual void render(UIContext* context) override;

	virtual void renderDebug(UIContext* context) override;

	virtual UIElement* hitTest(const Vector2& position) override;

	virtual void onMouseWheel(MouseWheelEvent& event) override;

	virtual void serialize(ISerializer& s) override;

private:
	Ref< UIElement > m_content;
	Ref< ScrollBar > m_verticalScrollBar;
	Ref< ScrollBar > m_horizontalScrollBar;

	ScrollBarVisibility m_verticalScrollBarVisibility = ScrollBarVisibility::Auto;
	ScrollBarVisibility m_horizontalScrollBarVisibility = ScrollBarVisibility::Auto;

	float m_verticalOffset = 0.0f;
	float m_horizontalOffset = 0.0f;

	// Cached layout information
	Vector2 m_viewportSize;      // Size of the visible viewport area
	Vector2 m_contentSize;       // Total size of the content
	Vector2 m_scrollableSize;    // Content size - viewport size
	bool m_showVerticalScrollBar = false;
	bool m_showHorizontalScrollBar = false;

	// Helper methods
	void updateScrollBars();
	void onVerticalScrollChanged(float value);
	void onHorizontalScrollChanged(float value);
	Vector2 getViewportPosition() const;
	Vector2 getContentOffset() const;
};

}
