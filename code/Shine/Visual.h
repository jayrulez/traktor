/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Shine/UIElement.h"
#include "Core/Math/Color4f.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

class RenderContext;

/*!
 * \brief Base class for all visual UI elements that can be rendered.
 *
 * Visual extends UIElement with rendering capabilities:
 * - Color and opacity
 * - Render order (Z-index)
 * - Clipping
 * - Virtual render() method
 *
 * This is an abstract base class. Use derived classes like Image, Text, Panel, etc.
 */
class T_DLLCLASS Visual : public UIElement
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit Visual(const std::wstring& name = L"");

	/*!
	 * \brief Destructor
	 */
	virtual ~Visual();

	// === Rendering ===

	/*!
	 * \brief Render this visual element
	 * \param rc Render context for drawing
	 * \param parentOpacity Accumulated opacity from parents (0-1)
	 *
	 * Override this to implement custom rendering.
	 * Remember to call base class to render children!
	 */
	virtual void render(RenderContext* rc, float parentOpacity);

	/*!
	 * \brief Render this element and all children
	 * \param rc Render context
	 *
	 * Handles opacity, visibility, and child rendering.
	 */
	void renderHierarchy(RenderContext* rc);

	// === Visual Properties ===

	/*!
	 * \brief Get color tint (RGBA)
	 */
	Color4f getColor() const { return m_color; }

	/*!
	 * \brief Set color tint
	 */
	void setColor(const Color4f& color);

	/*!
	 * \brief Get opacity (0 = transparent, 1 = opaque)
	 */
	float getOpacity() const { return m_color.getAlpha(); }

	/*!
	 * \brief Set opacity
	 */
	void setOpacity(float opacity);

	/*!
	 * \brief Get render order (higher = drawn later/on top)
	 */
	int getRenderOrder() const { return m_renderOrder; }

	/*!
	 * \brief Set render order (Z-index)
	 */
	void setRenderOrder(int order);

	// === Clipping ===

	/*!
	 * \brief Get whether this element clips its children
	 */
	bool getClipChildren() const { return m_clipChildren; }

	/*!
	 * \brief Set whether to clip children to this element's bounds
	 */
	void setClipChildren(bool clip);

	// === Events ===

	std::function<void()> onColorChanged;

protected:
	/*!
	 * \brief Called before rendering (setup state)
	 */
	virtual void onBeforeRender(RenderContext* rc);

	/*!
	 * \brief Called after rendering (restore state)
	 */
	virtual void onAfterRender(RenderContext* rc);

	/*!
	 * \brief Called when visual properties change
	 */
	virtual void onVisualChanged();

private:
	Color4f m_color = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
	int m_renderOrder = 0;
	bool m_clipChildren = false;
};

}
