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

class Draggable;

/*!
 * \brief DropTarget widget that accepts dragged items.
 *
 * DropTarget defines an area that can accept Draggable widgets when dropped.
 *
 * Usage:
 *   Ref<DropTarget> dropTarget = new DropTarget(L"InventorySlot");
 *   dropTarget->onDrop = [](Draggable* item) {
 *       addToInventory(item);
 *   };
 */
class T_DLLCLASS DropTarget : public Interactable
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit DropTarget(const std::wstring& name = L"DropTarget");

	/*!
	 * \brief Destructor
	 */
	virtual ~DropTarget();

	// === Drop Configuration ===

	/*!
	 * \brief Get whether drop is enabled
	 */
	bool getDropEnabled() const { return m_dropEnabled; }

	/*!
	 * \brief Set whether drop is enabled
	 */
	void setDropEnabled(bool enabled) { m_dropEnabled = enabled; }

	// === Drop State ===

	/*!
	 * \brief Get whether a draggable is currently over this target
	 */
	bool hasHoverDraggable() const { return m_hoverDraggable != nullptr; }

	/*!
	 * \brief Get current hover draggable
	 */
	Draggable* getHoverDraggable() const { return m_hoverDraggable; }

	// === Drop Validation ===

	/*!
	 * \brief Check if draggable can be dropped here
	 */
	virtual bool canAcceptDrop(Draggable* draggable) const;

	// === Events (called by Draggable) ===

	/*!
	 * \brief Called when draggable enters this target
	 */
	void onDragEnter(Draggable* draggable);

	/*!
	 * \brief Called when draggable is over this target
	 */
	void onDragOver(Draggable* draggable, const Vector2& position);

	/*!
	 * \brief Called when draggable leaves this target
	 */
	void onDragLeave(Draggable* draggable);

	/*!
	 * \brief Called when draggable is dropped on this target
	 */
	void onDrop(Draggable* draggable);

	// === Events (user callbacks) ===

	std::function<bool(Draggable*)> onValidateDrop; // Return true to accept
	std::function<void(Draggable*)> onDropAccepted;
	std::function<void(Draggable*)> onHoverChanged;

	virtual void render(RenderContext* rc, float parentOpacity) override;

private:
	bool m_dropEnabled = true;
	Draggable* m_hoverDraggable = nullptr;
};

}
