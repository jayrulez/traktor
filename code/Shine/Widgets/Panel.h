/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Shine/Visual.h"

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
 * \brief Simple colored rectangle panel widget.
 *
 * Panel is the simplest visual widget - just draws a solid colored rectangle.
 * Useful as backgrounds, dividers, and containers.
 *
 * Inherits from Visual to get color and opacity support.
 */
class T_DLLCLASS Panel : public Visual
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit Panel(const std::wstring& name = L"Panel");

	/*!
	 * \brief Destructor
	 */
	virtual ~Panel();

	// === Visual Properties ===

	/*!
	 * \brief Get background color
	 */
	Color4f getBackgroundColor() const { return getColor(); }

	/*!
	 * \brief Set background color
	 */
	void setBackgroundColor(const Color4f& color) { setColor(color); }

	// === Rendering ===

	virtual void render(RenderContext* rc, float parentOpacity) override;
};

}
