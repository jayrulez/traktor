/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Shine/Widgets/ScrollView.h"

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
 * \brief DynamicScrollBox widget for virtual scrolling.
 *
 * DynamicScrollBox implements virtual scrolling - only visible items
 * are created, making it efficient for very large lists.
 *
 * Usage:
 *   Ref<DynamicScrollBox> scrollBox = new DynamicScrollBox(L"ItemList");
 *   scrollBox->setPrototype(itemTemplate);
 *   scrollBox->setItemCount(10000);  // 10k items, but only visible ones created
 *   scrollBox->setItemHeight(50);
 *   scrollBox->rebuild();
 */
class T_DLLCLASS DynamicScrollBox : public ScrollView
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit DynamicScrollBox(const std::wstring& name = L"DynamicScrollBox");

	/*!
	 * \brief Destructor
	 */
	virtual ~DynamicScrollBox();

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
	 * \brief Get total item count
	 */
	int getItemCount() const { return m_itemCount; }

	/*!
	 * \brief Set total item count
	 */
	void setItemCount(int count);

	/*!
	 * \brief Get item height (must be uniform)
	 */
	float getItemHeight() const { return m_itemHeight; }

	/*!
	 * \brief Set item height
	 */
	void setItemHeight(float height);

	/*!
	 * \brief Rebuild virtual scroll
	 */
	void rebuild();

	/*!
	 * \brief Scroll to item index
	 */
	void scrollToItem(int index);

	// === Events ===

	std::function<void(UIElement*, int)> onItemCreated;
	std::function<void(UIElement*, int)> onItemVisible;

	// === Update ===

	virtual void update(float deltaTime) override;

private:
	Ref<UIElement> m_prototype;
	AlignedVector<Ref<UIElement>> m_visibleItems;
	int m_itemCount = 0;
	float m_itemHeight = 50.0f;
	int m_firstVisibleIndex = 0;
	int m_lastVisibleIndex = 0;

	// Helper methods
	void updateVisibleItems();
	void clearVisibleItems();
	UIElement* createItem(int index);
};

}
