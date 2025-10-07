/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/Panel.h"
#include "Shine/RenderContext.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Panel", Panel, Visual)

Panel::Panel(const std::wstring& name)
:	Visual(name)
{
}

Panel::~Panel()
{
}

void Panel::render(RenderContext* rc, float parentOpacity)
{
	if (!rc)
		return;

	// Calculate final opacity
	float finalOpacity = parentOpacity * getOpacity();
	Color4f finalColor = getColor();
	finalColor.setAlpha(Scalar(finalColor.getAlpha() * finalOpacity));

	// Push transform
	rc->pushTransform(getPosition(), getRotation(), getScale());

	// Draw rectangle
	rc->drawRect(Vector2(0, 0), getSize(), finalColor);

	// Pop transform
	rc->popTransform();

	// Render children
	Visual::render(rc, parentOpacity);
}

}
