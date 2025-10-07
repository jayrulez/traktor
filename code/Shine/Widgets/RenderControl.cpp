/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/RenderControl.h"
#include "Shine/RenderContext.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.RenderControl", RenderControl, Visual)

RenderControl::RenderControl(const std::wstring& name)
:	Visual(name)
{
}

RenderControl::~RenderControl()
{
}

void RenderControl::render(RenderContext* rc, float parentOpacity)
{
	if (!isVisible())
		return;

	Vector2 worldPos = getWorldPosition();
	Vector2 size = getSize();
	Aabb2 bounds(worldPos, worldPos + size);

	// Apply clipping if enabled
	if (m_clipToBounds)
		rc->pushClipRect(bounds);

	// Clear background if enabled
	if (m_clearBackground)
	{
		Color4f bgColor = m_backgroundColor;
		bgColor.setAlpha(Scalar(bgColor.getAlpha() * getOpacity() * parentOpacity));
		rc->drawRect(worldPos, size, bgColor);
	}

	// Invoke custom render callback
	if (onCustomRender)
		onCustomRender(rc, bounds);

	// Pop clipping
	if (m_clipToBounds)
		rc->popClipRect();
}

}
