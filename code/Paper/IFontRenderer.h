/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <string>
#include "Core/Object.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Color4f.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_PAPER_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::paper
{

class IFont;

/*! Abstract font renderer interface.
 * \ingroup Paper
 */
class T_DLLCLASS IFontRenderer : public Object
{
	T_RTTI_CLASS;

public:
	/*! Measure text dimensions.
	 *
	 * \param font Font to use.
	 * \param text Text to measure.
	 * \return Text dimensions.
	 */
	virtual Vector2 measureText(const IFont* font, const std::wstring& text) const = 0;

	/*! Draw text.
	 *
	 * \param font Font to use.
	 * \param position Position to draw text.
	 * \param text Text to draw.
	 * \param color Text color.
	 */
	virtual void drawText(const IFont* font, const Vector2& position, const std::wstring& text, const Color4f& color) = 0;
};

}
