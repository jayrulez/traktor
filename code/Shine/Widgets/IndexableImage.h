/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Shine/Widgets/Image.h"

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
 * \brief IndexableImage widget for displaying sprites by index.
 *
 * IndexableImage displays a specific sprite from a sprite sheet by index,
 * useful for static sprite selection (numbers, icons, state indicators).
 * Unlike FlipbookAnimation, this does not animate - it simply displays
 * one sprite at a time based on an index.
 *
 * Usage:
 *   Ref<IndexableImage> icon = new IndexableImage(L"ItemIcon");
 *   icon->setImageCount(16);      // 16 sprites in sheet
 *   icon->setImagesPerRow(4);     // 4x4 grid
 *   icon->setImageIndex(5);       // Show 6th sprite
 */
class T_DLLCLASS IndexableImage : public Image
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit IndexableImage(const std::wstring& name = L"IndexableImage");

	/*!
	 * \brief Destructor
	 */
	virtual ~IndexableImage();

	// === Index Management ===

	/*!
	 * \brief Get current image index
	 */
	int getImageIndex() const { return m_imageIndex; }

	/*!
	 * \brief Set image index
	 */
	void setImageIndex(int index);

	// === Sprite Sheet Layout ===

	/*!
	 * \brief Get total image count in sprite sheet
	 */
	int getImageCount() const { return m_imageCount; }

	/*!
	 * \brief Set image count
	 */
	void setImageCount(int count);

	/*!
	 * \brief Get images per row in sprite sheet
	 */
	int getImagesPerRow() const { return m_imagesPerRow; }

	/*!
	 * \brief Set images per row
	 */
	void setImagesPerRow(int count);

	// === Events ===

	std::function<void(int)> onIndexChanged;

private:
	int m_imageIndex = 0;
	int m_imageCount = 1;
	int m_imagesPerRow = 1;

	// Helper methods
	void updateImageUVs();
};

}
