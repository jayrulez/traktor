/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <string>
#include "Core/Math/Vector2.h"
#include "Core/Math/Color4f.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor
{
	namespace render
	{
		class ITexture;
	}
}

namespace traktor::shine
{

/*!
 * \brief Interface for submitting draw calls to the rendering backend.
 *
 * IRenderGraph is the abstraction between the UI system and the actual
 * rendering implementation. It receives batched draw calls from RenderContext
 * and is responsible for efficient rendering.
 */
class T_DLLCLASS IRenderGraph
{
public:
	virtual ~IRenderGraph() = default;

	/*!
	 * \brief Draw a quad (rectangle)
	 * \param position World position
	 * \param size Size in pixels
	 * \param color Color with alpha
	 * \param texture Optional texture (nullptr for solid color)
	 * \param uvMin UV coordinates for top-left corner
	 * \param uvMax UV coordinates for bottom-right corner
	 * \param rotation Rotation in radians
	 */
	virtual void drawQuad(
		const Vector2& position,
		const Vector2& size,
		const Color4f& color,
		render::ITexture* texture = nullptr,
		const Vector2& uvMin = Vector2(0, 0),
		const Vector2& uvMax = Vector2(1, 1),
		float rotation = 0.0f
	) = 0;

	/*!
	 * \brief Draw a line
	 * \param start Start position in world space
	 * \param end End position in world space
	 * \param color Line color
	 * \param thickness Line thickness in pixels
	 */
	virtual void drawLine(
		const Vector2& start,
		const Vector2& end,
		const Color4f& color,
		float thickness = 1.0f
	) = 0;

	/*!
	 * \brief Draw text
	 * \param text Text to draw
	 * \param position Position in world space
	 * \param fontSize Font size in points
	 * \param color Text color
	 */
	virtual void drawText(
		const std::wstring& text,
		const Vector2& position,
		float fontSize,
		const Color4f& color
	) = 0;

	/*!
	 * \brief Measure text size
	 * \param text Text to measure
	 * \param fontSize Font size in points
	 * \return Size of text in pixels
	 */
	virtual Vector2 measureText(
		const std::wstring& text,
		float fontSize
	) const = 0;
};

}
