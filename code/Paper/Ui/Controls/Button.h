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
#include "Core/Ref.h"
#include "Core/Guid.h"
#include "Core/Math/Color4f.h"
#include "Paper/Ui/Controls/Border.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_PAPER_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::paper
{

class TextBlock;

/*! Button control.
 * \ingroup Paper
 *
 * Button is a clickable control that combines a Border with a TextBlock.
 * It provides visual feedback for hover and pressed states.
 */
class T_DLLCLASS Button : public Border
{
	T_RTTI_CLASS;

public:
	Button();

	void setText(const std::wstring& text);

	const std::wstring& getText() const;

	void setFontId(const Guid& fontId);

	const Guid& getFontId() const;

	virtual void applyStyle(const UIStyle* style) override;

	virtual void onMouseEnter(MouseEvent& event) override;

	virtual void onMouseLeave(MouseEvent& event) override;

	virtual void onMouseDown(MouseEvent& event) override;

	virtual void onMouseUp(MouseEvent& event) override;

	virtual void serialize(ISerializer& s) override;

private:
	Ref< TextBlock > m_textBlock;
	Color4f m_normalBackground;
	Color4f m_hoverBackground;
	Color4f m_pressedBackground;
	bool m_isPressed = false;
};

}
