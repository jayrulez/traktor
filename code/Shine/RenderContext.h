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
#include "Core/Object.h"
#include "Core/Ref.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Aabb2.h"
#include "Core/Math/Color4f.h"
#include "Core/Containers/AlignedVector.h"

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

class IRenderGraph;

/*!
 * \brief Blend mode for rendering
 */
enum class BlendMode
{
	Normal,        // Standard alpha blending
	Additive,      // Additive blending
	Multiply,      // Multiplicative blending
	Screen,        // Screen blending
	Opaque         // No blending (fully opaque)
};

/*!
 * \brief Rendering context for UI drawing operations.
 *
 * RenderContext provides a high-level API for drawing UI elements:
 * - Rectangles (solid and textured)
 * - Text
 * - Lines
 * - Circles and rounded rectangles
 * - Transform stack (push/pop)
 * - Clip rectangle stack
 * - Blend modes
 *
 * This abstracts the underlying Traktor rendering system.
 */
class T_DLLCLASS RenderContext : public Object
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param renderGraph Traktor render graph for submitting primitives
	 * \param viewportWidth Viewport width in pixels
	 * \param viewportHeight Viewport height in pixels
	 */
	RenderContext(IRenderGraph* renderGraph, float viewportWidth, float viewportHeight);

	/*!
	 * \brief Destructor
	 */
	virtual ~RenderContext();

	// === Viewport ===

	/*!
	 * \brief Get viewport width
	 */
	float getViewportWidth() const { return m_viewportWidth; }

	/*!
	 * \brief Get viewport height
	 */
	float getViewportHeight() const { return m_viewportHeight; }

	// === Transform Stack ===

	/*!
	 * \brief Push transform onto stack
	 * \param position Local position offset
	 * \param rotation Local rotation in degrees
	 * \param scale Local scale factor
	 */
	void pushTransform(const Vector2& position, float rotation = 0.0f, const Vector2& scale = Vector2(1.0f, 1.0f));

	/*!
	 * \brief Pop transform from stack
	 */
	void popTransform();

	/*!
	 * \brief Get current world transform
	 */
	void getCurrentTransform(Vector2& outPosition, float& outRotation, Vector2& outScale) const;

	// === Clip Rectangle Stack ===

	/*!
	 * \brief Push clip rectangle onto stack
	 * \param rect Clip rectangle in world space
	 */
	void pushClipRect(const Aabb2& rect);

	/*!
	 * \brief Pop clip rectangle from stack
	 */
	void popClipRect();

	/*!
	 * \brief Get current clip rectangle (intersection of all pushed rects)
	 */
	Aabb2 getCurrentClipRect() const;

	/*!
	 * \brief Check if clipping is active
	 */
	bool isClipping() const { return !m_clipStack.empty(); }

	// === Drawing - Rectangles ===

	/*!
	 * \brief Draw solid color rectangle
	 * \param position Top-left position (local space)
	 * \param size Rectangle size
	 * \param color Fill color (with alpha)
	 */
	void drawRect(const Vector2& position, const Vector2& size, const Color4f& color);

	/*!
	 * \brief Draw textured rectangle
	 * \param texture Texture to draw
	 * \param position Top-left position (local space)
	 * \param size Rectangle size
	 * \param color Tint color (with alpha)
	 * \param uvMin Minimum UV coordinates (default 0,0)
	 * \param uvMax Maximum UV coordinates (default 1,1)
	 * \param rotation Rotation angle in radians (default 0)
	 */
	void drawTexturedRect(
		render::ITexture* texture,
		const Vector2& position,
		const Vector2& size,
		const Color4f& color = Color4f(1, 1, 1, 1),
		const Vector2& uvMin = Vector2(0, 0),
		const Vector2& uvMax = Vector2(1, 1),
		float rotation = 0.0f
	);

	/*!
	 * \brief Draw rectangle outline
	 * \param position Top-left position (local space)
	 * \param size Rectangle size
	 * \param color Line color
	 * \param thickness Line thickness in pixels
	 */
	void drawRectOutline(const Vector2& position, const Vector2& size, const Color4f& color, float thickness = 1.0f);

	/*!
	 * \brief Draw rounded rectangle
	 * \param position Top-left position (local space)
	 * \param size Rectangle size
	 * \param color Fill color
	 * \param cornerRadius Corner radius in pixels
	 */
	void drawRoundedRect(const Vector2& position, const Vector2& size, const Color4f& color, float cornerRadius);

	// === Drawing - Text ===

	/*!
	 * \brief Draw text
	 * \param text Text to draw
	 * \param position Position (local space)
	 * \param fontSize Font size in points
	 * \param color Text color
	 */
	void drawText(const std::wstring& text, const Vector2& position, float fontSize, const Color4f& color);

	/*!
	 * \brief Measure text size
	 * \param text Text to measure
	 * \param fontSize Font size in points
	 * \return Size of text in pixels
	 */
	Vector2 measureText(const std::wstring& text, float fontSize) const;

	// === Drawing - Lines ===

	/*!
	 * \brief Draw line
	 * \param start Start position (local space)
	 * \param end End position (local space)
	 * \param color Line color
	 * \param thickness Line thickness in pixels
	 */
	void drawLine(const Vector2& start, const Vector2& end, const Color4f& color, float thickness = 1.0f);

	// === Drawing - Circles ===

	/*!
	 * \brief Draw filled circle
	 * \param center Center position (local space)
	 * \param radius Circle radius
	 * \param color Fill color
	 * \param segments Number of segments (default 32)
	 */
	void drawCircle(const Vector2& center, float radius, const Color4f& color, int segments = 32);

	/*!
	 * \brief Draw circle outline
	 * \param center Center position (local space)
	 * \param radius Circle radius
	 * \param color Line color
	 * \param thickness Line thickness in pixels
	 * \param segments Number of segments (default 32)
	 */
	void drawCircleOutline(const Vector2& center, float radius, const Color4f& color, float thickness = 1.0f, int segments = 32);

	// === State ===

	/*!
	 * \brief Set blend mode
	 */
	void setBlendMode(BlendMode mode);

	/*!
	 * \brief Get current blend mode
	 */
	BlendMode getBlendMode() const { return m_blendMode; }

	/*!
	 * \brief Begin rendering frame
	 */
	void beginFrame();

	/*!
	 * \brief End rendering frame (submit all draw calls)
	 */
	void endFrame();

private:
	struct Transform
	{
		Vector2 position;
		float rotation;
		Vector2 scale;
	};

	IRenderGraph* m_renderGraph;
	float m_viewportWidth;
	float m_viewportHeight;

	// Transform stack
	AlignedVector<Transform> m_transformStack;
	Transform m_currentTransform;

	// Clip stack
	AlignedVector<Aabb2> m_clipStack;

	// State
	BlendMode m_blendMode = BlendMode::Normal;

	// Helper to transform point from local to world space
	Vector2 transformPoint(const Vector2& point) const;

	// Helper to get final color with current alpha
	Color4f applyCurrentAlpha(const Color4f& color) const;
};

}
