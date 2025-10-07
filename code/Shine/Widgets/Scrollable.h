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

/*!
 * \brief Scrollable widget for adding scroll capability to elements.
 *
 * Scrollable makes an element scrollable without needing a full ScrollView,
 * useful for simple drag-to-scroll interfaces.
 *
 * Usage:
 *   Ref<Scrollable> scrollable = new Scrollable(L"Map");
 *   scrollable->setScrollBounds(Aabb2(Vector2(-1000, -1000), Vector2(1000, 1000)));
 *   scrollable->setDragEnabled(true);
 */
class T_DLLCLASS Scrollable : public Interactable
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit Scrollable(const std::wstring& name = L"Scrollable");

	/*!
	 * \brief Destructor
	 */
	virtual ~Scrollable();

	// === Scroll Configuration ===

	/*!
	 * \brief Get scroll offset
	 */
	Vector2 getScrollOffset() const { return m_scrollOffset; }

	/*!
	 * \brief Set scroll offset
	 */
	void setScrollOffset(const Vector2& offset);

	/*!
	 * \brief Get scroll bounds
	 */
	Aabb2 getScrollBounds() const { return m_scrollBounds; }

	/*!
	 * \brief Set scroll bounds
	 */
	void setScrollBounds(const Aabb2& bounds) { m_scrollBounds = bounds; }

	// === Drag Scrolling ===

	/*!
	 * \brief Get whether drag scrolling is enabled
	 */
	bool getDragEnabled() const { return m_dragEnabled; }

	/*!
	 * \brief Set whether drag scrolling is enabled
	 */
	void setDragEnabled(bool enabled) { m_dragEnabled = enabled; }

	/*!
	 * \brief Get drag inertia
	 */
	float getDragInertia() const { return m_dragInertia; }

	/*!
	 * \brief Set drag inertia (0 = none, 1 = full)
	 */
	void setDragInertia(float inertia) { m_dragInertia = inertia; }

	// === Events ===

	std::function<void(const Vector2&)> onScrollChanged;

	// === Input ===

	virtual bool onMouseButtonDown(MouseButton button, const Vector2& position) override;
	virtual bool onMouseButtonUp(MouseButton button, const Vector2& position) override;
	virtual bool onMouseMove(const Vector2& position) override;

	// === Update ===

	virtual void update(float deltaTime) override;

private:
	Vector2 m_scrollOffset = Vector2(0.0f, 0.0f);
	Aabb2 m_scrollBounds = Aabb2(Vector2(-10000, -10000), Vector2(10000, 10000));
	bool m_dragEnabled = true;
	float m_dragInertia = 0.9f;

	// Drag state
	bool m_isDragging = false;
	Vector2 m_dragStart = Vector2(0.0f, 0.0f);
	Vector2 m_dragVelocity = Vector2(0.0f, 0.0f);
	Vector2 m_lastDragPosition = Vector2(0.0f, 0.0f);

	// Helper methods
	void applyScrollOffset(const Vector2& offset);
	void clampScrollOffset();
};

}
