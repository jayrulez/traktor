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
#include "Core/Ref.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::render
{
	class ITexture;
}

namespace traktor::shine
{

/*!
 * \brief Image widget for displaying textures.
 *
 * Image displays a textured rectangle with support for:
 * - Texture with color tint
 * - UV coordinates for texture atlases
 * - Fill modes (stretch, fit, fill, tile)
 * - Image types (simple, sliced, filled)
 */
class T_DLLCLASS Image : public Visual
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Image fill mode
	 */
	enum class FillMode
	{
		Stretch,    // Stretch to fill size
		Fit,        // Fit inside size (maintain aspect)
		Fill,       // Fill size (maintain aspect, may crop)
		Tile        // Tile texture
	};

	/*!
	 * \brief Image type
	 */
	enum class ImageType
	{
		Simple,     // Standard textured rectangle
		Sliced,     // 9-slice (borders don't scale)
		Filled      // Filled (radial/linear fill)
	};

	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit Image(const std::wstring& name = L"Image");

	/*!
	 * \brief Destructor
	 */
	virtual ~Image();

	// === Texture ===

	/*!
	 * \brief Get texture
	 */
	render::ITexture* getTexture() const { return m_texture; }

	/*!
	 * \brief Set texture
	 */
	void setTexture(render::ITexture* texture);

	// === UV Coordinates ===

	/*!
	 * \brief Get UV minimum (top-left)
	 */
	Vector2 getUVMin() const { return m_uvMin; }

	/*!
	 * \brief Set UV minimum
	 */
	void setUVMin(const Vector2& uvMin) { m_uvMin = uvMin; }

	/*!
	 * \brief Get UV maximum (bottom-right)
	 */
	Vector2 getUVMax() const { return m_uvMax; }

	/*!
	 * \brief Set UV maximum
	 */
	void setUVMax(const Vector2& uvMax) { m_uvMax = uvMax; }

	/*!
	 * \brief Set UV rectangle (convenience)
	 */
	void setUVRect(const Vector2& min, const Vector2& max)
	{
		m_uvMin = min;
		m_uvMax = max;
	}

	// === Fill Mode ===

	/*!
	 * \brief Get fill mode
	 */
	FillMode getFillMode() const { return m_fillMode; }

	/*!
	 * \brief Set fill mode
	 */
	void setFillMode(FillMode mode) { m_fillMode = mode; }

	// === Image Type ===

	/*!
	 * \brief Get image type
	 */
	ImageType getImageType() const { return m_imageType; }

	/*!
	 * \brief Set image type
	 */
	void setImageType(ImageType type) { m_imageType = type; }

	// === 9-Slice Borders (for Sliced type) ===

	/*!
	 * \brief Get border sizes (left, right, top, bottom)
	 */
	Vector4 getBorders() const { return m_borders; }

	/*!
	 * \brief Set border sizes
	 */
	void setBorders(const Vector4& borders) { m_borders = borders; }

	// === Fill Amount (for Filled type) ===

	/*!
	 * \brief Get fill amount (0-1)
	 */
	float getFillAmount() const { return m_fillAmount; }

	/*!
	 * \brief Set fill amount (0-1)
	 */
	void setFillAmount(float amount) { m_fillAmount = clamp(amount, 0.0f, 1.0f); }

	// === Rendering ===

	virtual void render(RenderContext* rc, float parentOpacity) override;

private:
	Ref<render::ITexture> m_texture;
	Vector2 m_uvMin = Vector2(0, 0);
	Vector2 m_uvMax = Vector2(1, 1);
	FillMode m_fillMode = FillMode::Stretch;
	ImageType m_imageType = ImageType::Simple;
	Vector4 m_borders = Vector4(0, 0, 0, 0); // left, right, top, bottom
	float m_fillAmount = 1.0f;

	// Helper rendering methods
	void renderSimple(RenderContext* rc, const Color4f& finalColor);
	void renderSliced(RenderContext* rc, const Color4f& finalColor);
	void renderFilled(RenderContext* rc, const Color4f& finalColor);
};

}
