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
 * \brief Vertical layout widget (VBox).
 *
 * VerticalLayout arranges children vertically from top to bottom with:
 * - Spacing between children
 * - Padding around content area
 * - Optional child stretching
 * - Alignment options
 *
 * Usage:
 *   Ref<VerticalLayout> vbox = new VerticalLayout();
 *   vbox->setSpacing(5);
 *   vbox->addChild(new Text(L"Title"));
 *   vbox->addChild(new Button(L"Start"));
 */
class T_DLLCLASS VerticalLayout : public Layout
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Child alignment mode
	 */
	enum class ChildAlignment
	{
		Start,    // Align to left
		Center,   // Center horizontally
		End,      // Align to right
		Stretch   // Stretch to fill width
	};

	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit VerticalLayout(const std::wstring& name = L"VerticalLayout");

	/*!
	 * \brief Destructor
	 */
	virtual ~VerticalLayout();

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
