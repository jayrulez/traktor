/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Shine/Visual.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

/*!
 * \brief RenderControl widget for custom rendering.
 *
 * RenderControl provides a callback-based custom rendering area,
 * useful for embedding game views, miniature scenes, custom visualizations.
 *
 * Usage:
 *   Ref<RenderControl> control = new RenderControl(L"Minimap");
 *   control->onCustomRender = [](RenderContext* rc, const Aabb2& bounds) {
 *       // Custom rendering code here
 *       rc->drawRect(bounds.mn, bounds.getSize(), Color4f(1, 0, 0, 1));
 *   };
 */
class T_DLLCLASS RenderControl : public Visual
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit RenderControl(const std::wstring& name = L"RenderControl");

	/*!
	 * \brief Destructor
	 */
	virtual ~RenderControl();

	// === Background ===

	/*!
	 * \brief Get background color
	 */
	Color4f getBackgroundColor() const { return m_backgroundColor; }

	/*!
	 * \brief Set background color
	 */
	void setBackgroundColor(const Color4f& color) { m_backgroundColor = color; }

	/*!
	 * \brief Get whether to clear background
	 */
	bool getClearBackground() const { return m_clearBackground; }

	/*!
	 * \brief Set whether to clear background
	 */
	void setClearBackground(bool clear) { m_clearBackground = clear; }

	// === Clipping ===

	/*!
	 * \brief Get whether rendering is clipped to bounds
	 */
	bool getClipToBounds() const { return m_clipToBounds; }

	/*!
	 * \brief Set whether rendering is clipped to bounds
	 */
	void setClipToBounds(bool clip) { m_clipToBounds = clip; }

	// === Events ===

	/*!
	 * \brief Custom render callback
	 * \param rc Render context
	 * \param bounds World-space render bounds
	 */
	std::function<void(RenderContext*, const Aabb2&)> onCustomRender;

	// === Render ===

	virtual void render(RenderContext* rc, float parentOpacity) override;

private:
	Color4f m_backgroundColor = Color4f(0, 0, 0, 0);
	bool m_clearBackground = false;
	bool m_clipToBounds = true;
};

}
