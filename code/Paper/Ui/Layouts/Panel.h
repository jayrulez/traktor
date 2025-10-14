/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/RefArray.h"
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

/*! Base class for layout panels.
 * \ingroup Paper
 *
 * Panel is the base class for layout containers that can hold
 * multiple child UIElements and arrange them according to layout rules.
 */
class T_DLLCLASS Panel : public UIElement
{
	T_RTTI_CLASS;

public:
	/*! Add a child element to the panel.
	 */
	void addChild(UIElement* child);

	/*! Remove a child element from the panel.
	 */
	void removeChild(UIElement* child);

	/*! Get all children.
	 */
	const RefArray< UIElement >& getChildren() const { return m_children; }

	virtual void render(UIContext* context) override;

	virtual void renderDebug(UIContext* context) override;

	virtual void serialize(ISerializer& s) override;

protected:
	RefArray< UIElement > m_children;
};

}
