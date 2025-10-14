/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Ref.h"
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

/*! Border decorator control.
 * \ingroup Paper
 *
 * Border draws a background and/or border around a single child element.
 */
class T_DLLCLASS Border : public UIElement
{
	T_RTTI_CLASS;

public:
	void setChild(UIElement* child) { m_child = child; }

	UIElement* getChild() const { return m_child; }

	void setBackground(const Color4f& color) { m_background = color; }

	const Color4f& getBackground() const { return m_background; }

	void setBorderBrush(const Color4f& color) { m_borderBrush = color; }

	const Color4f& getBorderBrush() const { return m_borderBrush; }

	void setBorderThickness(float thickness) { m_borderThickness = thickness; }

	float getBorderThickness() const { return m_borderThickness; }

	void setPadding(const Vector2& padding) { m_padding = padding; }

	const Vector2& getPadding() const { return m_padding; }

	virtual Vector2 measure(const Vector2& availableSize) override;

	virtual void arrange(const Vector2& position, const Vector2& size) override;

	virtual void render(Draw2D* renderer) override;

	virtual void serialize(ISerializer& s) override;

private:
	Ref< UIElement > m_child;
	Color4f m_background = Color4f(0.0f, 0.0f, 0.0f, 0.0f);
	Color4f m_borderBrush = Color4f(0.0f, 0.0f, 0.0f, 0.0f);
	float m_borderThickness = 0.0f;
	Vector2 m_padding = Vector2::zero();
};

}
