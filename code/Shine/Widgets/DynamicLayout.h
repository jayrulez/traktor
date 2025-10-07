/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Shine/Layout.h"

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
 * \brief DynamicLayout widget for prototype-based dynamic layouts.
 *
 * DynamicLayout creates a layout populated with clones of a prototype
 * element, useful for data-driven lists, grids, menus.
 *
 * Usage:
 *   Ref<DynamicLayout> layout = new DynamicLayout(L"Menu");
 *   layout->setPrototype(menuItemTemplate);
 *   layout->setItemCount(5);
 *   layout->rebuild();
 */
class T_DLLCLASS DynamicLayout : public Layout
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Layout orientation
	 */
	enum class Orientation
	{
		Horizontal,
		Vertical,
		Grid
	};

	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit DynamicLayout(const std::wstring& name = L"DynamicLayout");

	/*!
	 * \brief Destructor
	 */
	virtual ~DynamicLayout();

	// === Prototype ===

	/*!
	 * \brief Get prototype element
	 */
	UIElement* getPrototype() const { return m_prototype; }

	/*!
	 * \brief Set prototype element
	 */
	void setPrototype(UIElement* prototype);

	// === Items ===

	/*!
	 * \brief Get item count
	 */
	int getItemCount() const { return m_itemCount; }

	/*!
	 * \brief Set item count
	 */
	void setItemCount(int count);

	/*!
	 * \brief Rebuild layout with current settings
	 */
	void rebuild();

	/*!
	 * \brief Get generated items
	 */
	const AlignedVector<Ref<UIElement>>& getItems() const { return m_items; }

	// === Layout Configuration ===

	/*!
	 * \brief Get orientation
	 */
	Orientation getOrientation() const { return m_orientation; }

	/*!
	 * \brief Set orientation
	 */
	void setOrientation(Orientation orientation);

	/*!
	 * \brief Get grid columns (for grid orientation)
	 */
	int getColumns() const { return m_columns; }

	/*!
	 * \brief Set grid columns
	 */
	void setColumns(int columns);

	// === Events ===

	std::function<void(UIElement*, int)> onItemCreated;

	// === Update ===

	virtual void updateLayout() override;

private:
	Ref<UIElement> m_prototype;
	AlignedVector<Ref<UIElement>> m_items;
	int m_itemCount = 0;
	Orientation m_orientation = Orientation::Vertical;
	int m_columns = 1;

	// Helper methods
	void clearItems();
	UIElement* createItem(int index);
	void layoutHorizontal();
	void layoutVertical();
	void layoutGrid();
};

}
