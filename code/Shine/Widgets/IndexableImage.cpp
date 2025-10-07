/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/IndexableImage.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.IndexableImage", IndexableImage, Image)

IndexableImage::IndexableImage(const std::wstring& name)
:	Image(name)
{
	updateImageUVs();
}

IndexableImage::~IndexableImage()
{
}

void IndexableImage::setImageIndex(int index)
{
	index = clamp(index, 0, m_imageCount - 1);

	if (m_imageIndex != index)
	{
		m_imageIndex = index;
		updateImageUVs();

		if (onIndexChanged)
			onIndexChanged(m_imageIndex);
	}
}

void IndexableImage::setImageCount(int count)
{
	if (m_imageCount != count)
	{
		m_imageCount = std::max(1, count);

		// Clamp current index
		if (m_imageIndex >= m_imageCount)
			setImageIndex(m_imageCount - 1);

		updateImageUVs();
	}
}

void IndexableImage::setImagesPerRow(int count)
{
	if (m_imagesPerRow != count)
	{
		m_imagesPerRow = std::max(1, count);
		updateImageUVs();
	}
}

void IndexableImage::updateImageUVs()
{
	if (m_imageCount <= 0 || m_imagesPerRow <= 0)
		return;

	// Calculate rows
	int totalRows = (m_imageCount + m_imagesPerRow - 1) / m_imagesPerRow;

	// Calculate image position in grid
	int column = m_imageIndex % m_imagesPerRow;
	int row = m_imageIndex / m_imagesPerRow;

	// Calculate UV coordinates
	float imageWidth = 1.0f / m_imagesPerRow;
	float imageHeight = 1.0f / totalRows;

	Vector2 uvMin(column * imageWidth, row * imageHeight);
	Vector2 uvMax((column + 1) * imageWidth, (row + 1) * imageHeight);

	// Set UV coordinates on Image base class
	setUVRect(uvMin, uvMax);
}

}
