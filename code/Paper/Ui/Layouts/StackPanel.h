/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Paper/Ui/Layouts/Panel.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_PAPER_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::paper
{

/*! Stack panel layout.
 * \ingroup Paper
 *
 * StackPanel arranges children in a single line (vertical or horizontal).
 */
class T_DLLCLASS StackPanel : public Panel
{
	T_RTTI_CLASS;

public:
	enum class Orientation
	{
		Vertical,
		Horizontal
	};

	void setOrientation(Orientation orientation) { m_orientation = orientation; }

	Orientation getOrientation() const { return m_orientation; }

	virtual Vector2 measure(const Vector2& availableSize, UIContext* context) override;

	virtual void arrange(const Vector2& position, const Vector2& size) override;

	virtual void serialize(ISerializer& s) override;

private:
	Orientation m_orientation = Orientation::Vertical;
};

}
