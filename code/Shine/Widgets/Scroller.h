/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Shine/UIElement.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

/*!
 * \brief Scroller widget for automatic scrolling content.
 *
 * Scroller automatically scrolls its children at a constant speed,
 * useful for credits, news tickers, parallax backgrounds, etc.
 *
 * Usage:
 *   Ref<Scroller> scroller = new Scroller(L"Credits");
 *   scroller->setScrollSpeed(Vector2(0, 50)); // 50 units/sec upward
 *   scroller->setLooping(true);
 *   scroller->play();
 */
class T_DLLCLASS Scroller : public UIElement
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit Scroller(const std::wstring& name = L"Scroller");

	/*!
	 * \brief Destructor
	 */
	virtual ~Scroller();

	// === Scroll Control ===

	/*!
	 * \brief Start scrolling
	 */
	void play();

	/*!
	 * \brief Stop scrolling
	 */
	void stop();

	/*!
	 * \brief Pause scrolling
	 */
	void pause();

	/*!
	 * \brief Resume scrolling
	 */
	void resume();

	/*!
	 * \brief Reset scroll position
	 */
	void reset();

	/*!
	 * \brief Get whether scrolling is active
	 */
	bool isPlaying() const { return m_isPlaying && !m_isPaused; }

	/*!
	 * \brief Get whether scrolling is paused
	 */
	bool isPaused() const { return m_isPaused; }

	// === Scroll Configuration ===

	/*!
	 * \brief Get scroll speed (units per second)
	 */
	Vector2 getScrollSpeed() const { return m_scrollSpeed; }

	/*!
	 * \brief Set scroll speed
	 */
	void setScrollSpeed(const Vector2& speed) { m_scrollSpeed = speed; }

	/*!
	 * \brief Get current scroll offset
	 */
	Vector2 getScrollOffset() const { return m_scrollOffset; }

	/*!
	 * \brief Set scroll offset
	 */
	void setScrollOffset(const Vector2& offset);

	// === Looping ===

	/*!
	 * \brief Get whether scrolling loops
	 */
	bool getLooping() const { return m_looping; }

	/*!
	 * \brief Set whether scrolling loops
	 */
	void setLooping(bool looping) { m_looping = looping; }

	/*!
	 * \brief Get loop bounds (reset position when exceeded)
	 */
	Vector2 getLoopBounds() const { return m_loopBounds; }

	/*!
	 * \brief Set loop bounds
	 */
	void setLoopBounds(const Vector2& bounds) { m_loopBounds = bounds; }

	// === Events ===

	std::function<void()> onScrollComplete;
	std::function<void()> onLoop;

	// === Update ===

	virtual void update(float deltaTime) override;

private:
	Vector2 m_scrollSpeed = Vector2(0.0f, 0.0f);
	Vector2 m_scrollOffset = Vector2(0.0f, 0.0f);
	Vector2 m_loopBounds = Vector2(0.0f, 0.0f); // 0 = no limit
	bool m_looping = false;

	// Playback state
	bool m_isPlaying = false;
	bool m_isPaused = false;

	// Store original child positions for offset calculation
	AlignedVector<Vector2> m_originalChildPositions;
	bool m_childPositionsCached = false;

	// Helper methods
	void updateChildPositions();
	void checkLoopBounds();
	void cacheChildPositions();
};

}
