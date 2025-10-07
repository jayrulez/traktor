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
 * \brief Horizontal layout widget (HBox).
 *
 * HorizontalLayout arranges children horizontally from left to right with:
 * - Spacing between children
 * - Padding around content area
 * - Optional child stretching
 * - Alignment options
 *
 * Usage:
 *   Ref<HorizontalLayout> hbox = new HorizontalLayout();
 *   hbox->setSpacing(10);
 *   hbox->addChild(new Button(L"OK"));
 *   hbox->addChild(new Button(L"Cancel"));
 */
class T_DLLCLASS HorizontalLayout : public Layout
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Child alignment mode
	 */
	enum class ChildAlignment
	{
		Start,    // Align to top
		Center,   // Center vertically
		End,      // Align to bottom
		Stretch   // Stretch to fill height
	};

	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit HorizontalLayout(const std::wstring& name = L"HorizontalLayout");

	/*!
	 * \brief Destructor
	 */
	virtual ~HorizontalLayout();

	// === Alignment ===

	/*!
	 * \brief Get child alignment
	 */
	ChildAlignment getChildAlignment() const { return m_childAlignment; }

	/*!
	 * \brief Set child alignment
	 */
	void setChildAlignment(ChildAlignment alignment);

	// === Layout ===

	virtual void updateLayout() override;

	virtual Vector2 calculateMinSize() const override;

	virtual Vector2 calculatePreferredSize() const override;

private:
	ChildAlignment m_childAlignment = ChildAlignment::Start;
};

}
