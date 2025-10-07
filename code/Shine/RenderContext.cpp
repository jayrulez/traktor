/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/RenderContext.h"
#include "Shine/IRenderGraph.h"
#include "Core/Math/MathUtils.h"
#include <algorithm>

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.RenderContext", RenderContext, Object)

RenderContext::RenderContext(IRenderGraph* renderGraph, float viewportWidth, float viewportHeight)
:	m_renderGraph(renderGraph)
,	m_viewportWidth(viewportWidth)
,	m_viewportHeight(viewportHeight)
{
	// Initialize identity transform
	m_currentTransform.position = Vector2(0.0f, 0.0f);
	m_currentTransform.rotation = 0.0f;
	m_currentTransform.scale = Vector2(1.0f, 1.0f);
}

RenderContext::~RenderContext()
{
}

void RenderContext::pushTransform(const Vector2& position, float rotation, const Vector2& scale)
{
	// Push current transform onto stack
	m_transformStack.push_back(m_currentTransform);

	// Accumulate new transform
	Transform newTransform;

	// Apply current rotation and scale to position offset
	Vector2 offset = position;
	if (m_currentTransform.rotation != 0.0f)
	{
		float rad = deg2rad(m_currentTransform.rotation);
		float cosA = std::cos(rad);
		float sinA = std::sin(rad);
		Vector2 rotated(
			offset.x * cosA - offset.y * sinA,
			offset.x * sinA + offset.y * cosA
		);
		offset = rotated;
	}

	newTransform.position = m_currentTransform.position + offset * m_currentTransform.scale;
	newTransform.rotation = m_currentTransform.rotation + rotation;
	newTransform.scale = m_currentTransform.scale * scale;

	m_currentTransform = newTransform;
}

void RenderContext::popTransform()
{
	if (!m_transformStack.empty())
	{
		m_currentTransform = m_transformStack.back();
		m_transformStack.pop_back();
	}
}

void RenderContext::getCurrentTransform(Vector2& outPosition, float& outRotation, Vector2& outScale) const
{
	outPosition = m_currentTransform.position;
	outRotation = m_currentTransform.rotation;
	outScale = m_currentTransform.scale;
}

void RenderContext::pushClipRect(const Aabb2& rect)
{
	if (m_clipStack.empty())
	{
		m_clipStack.push_back(rect);
	}
	else
	{
		// Intersect with current clip rect
		Aabb2 current = m_clipStack.back();
		Aabb2 intersected;
		intersected.mn.x = std::max(current.mn.x, rect.mn.x);
		intersected.mn.y = std::max(current.mn.y, rect.mn.y);
		intersected.mx.x = std::min(current.mx.x, rect.mx.x);
		intersected.mx.y = std::min(current.mx.y, rect.mx.y);

		// Ensure valid rect (min < max)
		if (intersected.mn.x > intersected.mx.x)
			intersected.mn.x = intersected.mx.x;
		if (intersected.mn.y > intersected.mx.y)
			intersected.mn.y = intersected.mx.y;

		m_clipStack.push_back(intersected);
	}
}

void RenderContext::popClipRect()
{
	if (!m_clipStack.empty())
		m_clipStack.pop_back();
}

Aabb2 RenderContext::getCurrentClipRect() const
{
	if (!m_clipStack.empty())
		return m_clipStack.back();

	// No clipping - return full viewport
	return Aabb2(
		Vector2(0.0f, 0.0f),
		Vector2(m_viewportWidth, m_viewportHeight)
	);
}

void RenderContext::drawRect(const Vector2& position, const Vector2& size, const Color4f& color)
{
	if (!m_renderGraph)
		return;

	// Transform position to world space
	Vector2 worldPos = transformPoint(position);
	Vector2 worldSize = size * m_currentTransform.scale;

	// Apply current alpha
	Color4f finalColor = applyCurrentAlpha(color);

	// Submit to render graph
	m_renderGraph->drawQuad(
		worldPos,
		worldSize,
		finalColor,
		nullptr, // no texture
		Vector2(0, 0),
		Vector2(1, 1)
	);
}

void RenderContext::drawTexturedRect(
	render::ITexture* texture,
	const Vector2& position,
	const Vector2& size,
	const Color4f& color,
	const Vector2& uvMin,
	const Vector2& uvMax,
	float rotation)
{
	if (!m_renderGraph || !texture)
		return;

	// Transform position to world space
	Vector2 worldPos = transformPoint(position);
	Vector2 worldSize = size * m_currentTransform.scale;

	// Apply current alpha
	Color4f finalColor = applyCurrentAlpha(color);

	// Combine rotation with current transform rotation
	float finalRotation = m_currentTransform.rotation + rotation;

	// Submit to render graph
	m_renderGraph->drawQuad(
		worldPos,
		worldSize,
		finalColor,
		texture,
		uvMin,
		uvMax,
		finalRotation
	);
}

void RenderContext::drawRectOutline(const Vector2& position, const Vector2& size, const Color4f& color, float thickness)
{
	if (!m_renderGraph)
		return;

	// Draw four lines for rectangle outline
	Vector2 topLeft = position;
	Vector2 topRight = position + Vector2(size.x, 0);
	Vector2 bottomLeft = position + Vector2(0, size.y);
	Vector2 bottomRight = position + size;

	drawLine(topLeft, topRight, color, thickness);
	drawLine(topRight, bottomRight, color, thickness);
	drawLine(bottomRight, bottomLeft, color, thickness);
	drawLine(bottomLeft, topLeft, color, thickness);
}

void RenderContext::drawRoundedRect(const Vector2& position, const Vector2& size, const Color4f& color, float cornerRadius)
{
	// TODO: Implement rounded rectangle rendering
	// For now, just draw regular rectangle
	drawRect(position, size, color);
}

void RenderContext::drawText(const std::wstring& text, const Vector2& position, float fontSize, const Color4f& color)
{
	// TODO: Implement text rendering using font system
	// This requires integration with the font system from the ECS implementation
	// For now, this is a stub
}

Vector2 RenderContext::measureText(const std::wstring& text, float fontSize) const
{
	// TODO: Implement text measurement using font system
	// For now, return rough estimate (monospace)
	float charWidth = fontSize * 0.6f;
	return Vector2(text.length() * charWidth, fontSize);
}

void RenderContext::drawLine(const Vector2& start, const Vector2& end, const Color4f& color, float thickness)
{
	if (!m_renderGraph)
		return;

	// Transform points to world space
	Vector2 worldStart = transformPoint(start);
	Vector2 worldEnd = transformPoint(end);

	// Apply current alpha
	Color4f finalColor = applyCurrentAlpha(color);

	// Submit to render graph
	m_renderGraph->drawLine(worldStart, worldEnd, finalColor, thickness);
}

void RenderContext::drawCircle(const Vector2& center, float radius, const Color4f& color, int segments)
{
	// Draw filled circle as triangle fan
	// For simplicity, approximate with polygon
	// TODO: Optimize with dedicated circle rendering

	// Just draw as many small rectangles for now (stub implementation)
	drawRect(center - Vector2(radius, radius), Vector2(radius * 2, radius * 2), color);
}

void RenderContext::drawCircleOutline(const Vector2& center, float radius, const Color4f& color, float thickness, int segments)
{
	if (segments < 3)
		segments = 3;

	// Draw circle outline as line segments
	float angleStep = 360.0f / segments;
	for (int i = 0; i < segments; ++i)
	{
		float angle1 = deg2rad(i * angleStep);
		float angle2 = deg2rad((i + 1) * angleStep);

		Vector2 p1 = center + Vector2(std::cos(angle1), std::sin(angle1)) * radius;
		Vector2 p2 = center + Vector2(std::cos(angle2), std::sin(angle2)) * radius;

		drawLine(p1, p2, color, thickness);
	}
}

void RenderContext::setBlendMode(BlendMode mode)
{
	m_blendMode = mode;
	// TODO: Apply blend mode to render graph state
}

void RenderContext::beginFrame()
{
	// Clear stacks
	m_transformStack.clear();
	m_clipStack.clear();

	// Reset transform
	m_currentTransform.position = Vector2(0.0f, 0.0f);
	m_currentTransform.rotation = 0.0f;
	m_currentTransform.scale = Vector2(1.0f, 1.0f);

	// Reset blend mode
	m_blendMode = BlendMode::Normal;
}

void RenderContext::endFrame()
{
	// Submit all queued draw calls to render graph
	// The render graph handles batching and submission
}

Vector2 RenderContext::transformPoint(const Vector2& point) const
{
	Vector2 result = point;

	// Apply scale
	result = result * m_currentTransform.scale;

	// Apply rotation
	if (m_currentTransform.rotation != 0.0f)
	{
		float rad = deg2rad(m_currentTransform.rotation);
		float cosA = std::cos(rad);
		float sinA = std::sin(rad);
		Vector2 rotated(
			result.x * cosA - result.y * sinA,
			result.x * sinA + result.y * cosA
		);
		result = rotated;
	}

	// Apply translation
	result = result + m_currentTransform.position;

	return result;
}

Color4f RenderContext::applyCurrentAlpha(const Color4f& color) const
{
	// Currently we don't track alpha in transform stack
	// This could be extended to support parent opacity
	return color;
}

}
