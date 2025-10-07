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
 * \brief Mask widget for clipping/masking children.
 *
 * Mask uses a stencil buffer or scissor rectangle to clip children to a
 * specific shape or region.
 *
 * Usage:
 *   Ref<Mask> mask = new Mask(L"CircleMask");
 *   mask->setMaskType(Mask::MaskType::Rectangle);
 *   mask->addChild(new Image(L"ClippedImage"));
 */
class T_DLLCLASS Mask : public Visual
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Mask type
	 */
	enum class MaskType
	{
		Rectangle,     // Rectangular mask (scissor test)
		Circle,        // Circular mask (stencil)
		Texture        // Texture-based mask (stencil)
	};

	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit Mask(const std::wstring& name = L"Mask");

	/*!
	 * \brief Destructor
	 */
	virtual ~Mask();

	// === Mask Configuration ===

	/*!
	 * \brief Get mask type
	 */
	MaskType getMaskType() const { return m_maskType; }

	/*!
	 * \brief Set mask type
	 */
	void setMaskType(MaskType type);

	/*!
	 * \brief Get whether to show mask graphic
	 */
	bool getShowMaskGraphic() const { return m_showMaskGraphic; }

	/*!
	 * \brief Set whether to show mask graphic
	 */
	void setShowMaskGraphic(bool show) { m_showMaskGraphic = show; }

	// === Rendering ===

	virtual void render(RenderContext* rc, float parentOpacity) override;

private:
	MaskType m_maskType = MaskType::Rectangle;
	bool m_showMaskGraphic = false;
};

}
