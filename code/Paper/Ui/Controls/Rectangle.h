/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

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

/*! Rectangle shape control.
 * \ingroup Paper
 *
 * Rectangle renders a simple filled rectangle.
 */
class T_DLLCLASS Rectangle : public UIElement
{
	T_RTTI_CLASS;

public:
	void setFill(const Color4f& color) { m_fill = color; }

	const Color4f& getFill() const { return m_fill; }

	void setWidth(float width) { m_width = width; }

	float getWidth() const { return m_width; }

	void setHeight(float height) { m_height = height; }

	float getHeight() const { return m_height; }

	virtual void applyStyle(const UIStyle* style) override;

	virtual Vector2 measure(const Vector2& availableSize, UIContext* context) override;

	virtual void render(UIContext* context) override;

	virtual void serialize(ISerializer& s) override;

private:
	Color4f m_fill = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
	float m_width = 0.0f;
	float m_height = 0.0f;
};

}
