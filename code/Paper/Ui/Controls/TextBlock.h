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

class IFont;
class IFontRenderer;

/*! Text display control.
 * \ingroup Paper
 *
 * TextBlock renders text using a bitmap font.
 */
class T_DLLCLASS TextBlock : public UIElement
{
	T_RTTI_CLASS;

public:
	void setText(const std::wstring& text) { m_text = text; }

	const std::wstring& getText() const { return m_text; }

	void setFontId(const Guid& fontId) { m_fontId = fontId; }

	const Guid& getFontId() const { return m_fontId; }

	void setForeground(const Color4f& color) { m_foreground = color; }

	const Color4f& getForeground() const { return m_foreground; }

	virtual Vector2 measure(const Vector2& availableSize) override;

	virtual void render(Draw2D* renderer) override;

	virtual void serialize(ISerializer& s) override;

private:
	std::wstring m_text;
	Guid m_fontId;
	Color4f m_foreground = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
};

}
