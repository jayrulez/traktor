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
 * \brief Padding structure for layout spacing
 */
struct Padding
{
	float left = 0.0f;
	float right = 0.0f;
	float top = 0.0f;
	float bottom = 0.0f;

	Padding() = default;
	Padding(float all) : left(all), right(all), top(all), bottom(all) {}
	Padding(float horizontal, float vertical)
	:	left(horizontal), right(horizontal), top(vertical), bottom(vertical) {}
	Padding(float l, float r, float t, float b)
	:	left(l), right(r), top(t), bottom(b) {}
};

/*!
 * \brief Base class for all layout container elements.
 *
 * Layout extends UIElement with child layout management:
 * - Padding and spacing
 * - Automatic child positioning
 * - Layout recalculation
 * - Content size calculation
 *
 * This is an abstract base class. Use derived classes like HorizontalLayout,
 * VerticalLayout, GridLayout, etc.
 */
class T_DLLCLASS Layout : public UIElement
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit Layout(const std::wstring& name = L"");

	/*!
	 * \brief Destructor
	 */
	virtual ~Layout();

	// === Layout ===

	/*!
	 * \brief Recalculate layout (position and size children)
	 *
	 * Override this to implement custom layout logic.
	 * Call this when children are added/removed or properties change.
	 */
	virtual void updateLayout();

	/*!
	 * \brief Calculate minimum size required for all children
	 */
	virtual Vector2 calculateMinSize() const;

	/*!
	 * \brief Calculate preferred size for layout
	 */
	virtual Vector2 calculatePreferredSize() const;

	// === Properties ===

	/*!
	 * \brief Get padding
	 */
	Padding getPadding() const { return m_padding; }

	/*!
	 * \brief Set padding
	 */
	void setPadding(const Padding& padding);

	/*!
	 * \brief Get spacing between children
	 */
	float getSpacing() const { return m_spacing; }

	/*!
	 * \brief Set spacing between children
	 */
	void setSpacing(float spacing);

	/*!
	 * \brief Get whether to ignore invisible children in layout
	 */
	bool getIgnoreInvisibleChildren() const { return m_ignoreInvisibleChildren; }

	/*!
	 * \brief Set whether to ignore invisible children
	 */
	void setIgnoreInvisibleChildren(bool ignore);

	/*!
	 * \brief Get whether layout automatically updates
	 */
	bool getAutoLayout() const { return m_autoLayout; }

	/*!
	 * \brief Set whether layout automatically updates
	 */
	void setAutoLayout(bool autoLayout);

	// === Overrides ===

	virtual void update(float deltaTime) override;

	virtual void sizeChanged() override;

protected:
	/*!
	 * \brief Mark layout as dirty (needs recalculation)
	 */
	void markLayoutDirty();

	/*!
	 * \brief Get content area (size minus padding)
	 */
	Vector2 getContentSize() const;

	/*!
	 * \brief Get content offset (top-left of content area)
	 */
	Vector2 getContentOffset() const;

	/*!
	 * \brief Get list of children to layout (respects ignoreInvisible)
	 */
	AlignedVector<UIElement*> getLayoutChildren() const;

protected:
	Padding m_padding;
	float m_spacing = 0.0f;
	bool m_ignoreInvisibleChildren = true;
	bool m_autoLayout = true;
	bool m_layoutDirty = true;
};

}
