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

class DropTarget;

/*!
 * \brief Draggable widget that can be dragged and dropped.
 *
 * Draggable provides drag-and-drop functionality, allowing elements to be
 * dragged to DropTarget widgets.
 *
 * Usage:
 *   Ref<Draggable> draggable = new Draggable(L"DraggableItem");
 *   draggable->setDragEnabled(true);
 *   draggable->onDragBegin = []() { ... };
 *   draggable->onDragEnd = [](DropTarget* target) { ... };
 */
class T_DLLCLASS Draggable : public Interactable
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit Draggable(const std::wstring& name = L"Draggable");

	/*!
	 * \brief Destructor
	 */
	virtual ~Draggable();

	// === Drag Configuration ===

	/*!
	 * \brief Get whether dragging is enabled
	 */
	bool getDragEnabled() const { return m_dragEnabled; }

	/*!
	 * \brief Set whether dragging is enabled
	 */
	void setDragEnabled(bool enabled) { m_dragEnabled = enabled; }

	/*!
	 * \brief Get drag threshold (pixels before drag starts)
	 */
	float getDragThreshold() const { return m_dragThreshold; }

	/*!
	 * \brief Set drag threshold
	 */
	void setDragThreshold(float threshold) { m_dragThreshold = threshold; }

	// === Drag State ===

	/*!
	 * \brief Get whether currently dragging
	 */
	bool isDragging() const { return m_isDragging; }

	/*!
	 * \brief Get drag offset from initial position
	 */
	Vector2 getDragOffset() const { return m_dragOffset; }

	/*!
	 * \brief Get current drop target (if hovering over one)
	 */
	DropTarget* getCurrentDropTarget() const { return m_currentDropTarget; }

	// === Drag Data ===

	/*!
	 * \brief Get drag data (user-defined payload)
	 */
	void* getDragData() const { return m_dragData; }

	/*!
	 * \brief Set drag data
	 */
	void setDragData(void* data) { m_dragData = data; }

	// === Events ===

	std::function<void()> onDragBegin;
	std::function<void(const Vector2&)> onDragging;
	std::function<void(DropTarget*)> onDragEnd;
	std::function<void()> onDragCancelled;

	// === Interaction ===

	virtual bool onMouseButtonDown(MouseButton button, const Vector2& position) override;
	virtual bool onMouseButtonUp(MouseButton button, const Vector2& position) override;
	virtual bool onMouseMove(const Vector2& position) override;

	virtual void update(float deltaTime) override;
	virtual void render(RenderContext* rc, float parentOpacity) override;

private:
	bool m_dragEnabled = true;
	float m_dragThreshold = 5.0f;

	// Drag state
	bool m_isDragging = false;
	Vector2 m_dragStartPosition;
	Vector2 m_dragOffset;
	Vector2 m_originalPosition;
	DropTarget* m_currentDropTarget = nullptr;

	// Drag data (user-defined)
	void* m_dragData = nullptr;

	// Helper methods
	void startDrag();
	void updateDrag(const Vector2& position);
	void endDrag(bool cancelled);
	DropTarget* findDropTargetAt(const Vector2& position);
};

}
