/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/Image.h"
#include "Shine/RenderContext.h"
#include "Render/ITexture.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Image", Image, Visual)

Image::Image(const std::wstring& name)
:	Visual(name)
{
}

Image::~Image()
{
}

void Image::setTexture(render::ITexture* texture)
{
	m_texture = texture;
}

void Image::render(RenderContext* rc, float parentOpacity)
{
	if (!rc || !m_texture)
	{
		// No texture - render children only
		Visual::render(rc, parentOpacity);
		return;
	}

	// Calculate final opacity
	float finalOpacity = parentOpacity * getOpacity();
	Color4f finalColor = getColor();
	finalColor.setAlpha(Scalar(finalColor.getAlpha() * finalOpacity));

	// Push transform
	rc->pushTransform(getPosition(), getRotation(), getScale());

	// Render based on image type
	switch (m_imageType)
	{
	case ImageType::Simple:
		renderSimple(rc, finalColor);
		break;

	case ImageType::Sliced:
		renderSliced(rc, finalColor);
		break;

	case ImageType::Filled:
		renderFilled(rc, finalColor);
		break;
	}

	// Pop transform
	rc->popTransform();

	// Render children
	Visual::render(rc, parentOpacity);
}

void Image::renderSimple(RenderContext* rc, const Color4f& finalColor)
{
	// Simple textured rectangle
	Vector2 size = getSize();

	// Apply fill mode
	Vector2 renderSize = size;
	Vector2 renderPos(0, 0);

	if (m_fillMode != FillMode::Stretch && m_texture)
	{
		// Get texture size
		// TODO: Need method to get texture dimensions
		// For now, assume we want to use the size as-is
	}

	// Draw textured rectangle
	rc->drawTexturedRect(
		m_texture,
		renderPos,
		renderSize,
		finalColor,
		m_uvMin,
		m_uvMax
	);
}

void Image::renderSliced(RenderContext* rc, const Color4f& finalColor)
{
	// 9-slice rendering: borders don't scale, center stretches
	Vector2 size = getSize();

	float left = m_borders.x();
	float right = m_borders.y();
	float top = m_borders.z();
	float bottom = m_borders.w();

	// Calculate 9 regions
	float centerWidth = size.x - left - right;
	float centerHeight = size.y - top - bottom;

	// Clamp to prevent negative sizes
	if (centerWidth < 0) centerWidth = 0;
	if (centerHeight < 0) centerHeight = 0;

	// UV coordinates for 9 regions
	float uvWidth = m_uvMax.x - m_uvMin.x;
	float uvHeight = m_uvMax.y - m_uvMin.y;

	// Assume borders are specified in UV space (0-1)
	// TODO: This needs proper UV border calculation based on texture size
	float uvLeft = uvWidth * 0.25f;
	float uvRight = uvWidth * 0.25f;
	float uvTop = uvHeight * 0.25f;
	float uvBottom = uvHeight * 0.25f;

	// For simplicity, just render as simple for now
	// Full 9-slice implementation requires 9 draw calls
	renderSimple(rc, finalColor);
}

void Image::renderFilled(RenderContext* rc, const Color4f& finalColor)
{
	// Filled image: only render a portion based on fillAmount
	// Common for health bars, progress bars, etc.

	if (m_fillAmount <= 0.0f)
		return; // Nothing to draw

	Vector2 size = getSize();

	// Simple horizontal fill for now
	Vector2 fillSize = size;
	fillSize.x *= m_fillAmount;

	Vector2 uvMax = m_uvMax;
	uvMax.x = m_uvMin.x + (m_uvMax.x - m_uvMin.x) * m_fillAmount;

	// Draw filled portion
	rc->drawTexturedRect(
		m_texture,
		Vector2(0, 0),
		fillSize,
		finalColor,
		m_uvMin,
		uvMax
	);
}

}
