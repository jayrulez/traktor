/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/Mask.h"
#include "Shine/RenderContext.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Mask", Mask, Visual)

Mask::Mask(const std::wstring& name)
:	Visual(name)
{
	// Masks clip children by default
	setClipChildren(true);
}

Mask::~Mask()
{
}

void Mask::setMaskType(MaskType type)
{
	if (m_maskType != type)
	{
		m_maskType = type;
	}
}

void Mask::render(RenderContext* rc, float parentOpacity)
{
	if (!rc)
		return;

	// Calculate final opacity
	float finalOpacity = parentOpacity * getOpacity();

	// Push transform
	rc->pushTransform(getPosition(), getRotation(), getScale());

	// Setup clipping based on mask type
	Aabb2 bounds(Vector2(0, 0), getSize());

	switch (m_maskType)
	{
	case MaskType::Rectangle:
		// Use scissor rectangle
		rc->pushClipRect(bounds);
		break;

	case MaskType::Circle:
		// TODO: Implement circular stencil mask
		// For now, fall back to rectangle
		rc->pushClipRect(bounds);
		break;

	case MaskType::Texture:
		// TODO: Implement texture-based stencil mask
		// For now, fall back to rectangle
		rc->pushClipRect(bounds);
		break;
	}

	// Optionally draw mask graphic
	if (m_showMaskGraphic)
	{
		Color4f maskColor = getColor();
		maskColor.setAlpha(Scalar(maskColor.getAlpha() * finalOpacity));

		switch (m_maskType)
		{
		case MaskType::Rectangle:
			rc->drawRect(Vector2(0, 0), getSize(), maskColor);
			break;

		case MaskType::Circle:
			{
				Vector2 center = getSize() * 0.5f;
				float radius = std::min(getSize().x, getSize().y) * 0.5f;
				rc->drawCircle(center, radius, maskColor);
			}
			break;

		case MaskType::Texture:
			// TODO: Draw texture
			rc->drawRect(Vector2(0, 0), getSize(), maskColor);
			break;
		}
	}

	// Render children (clipped)
	Visual::render(rc, parentOpacity);

	// Pop clipping
	rc->popClipRect();

	// Pop transform
	rc->popTransform();
}

}
