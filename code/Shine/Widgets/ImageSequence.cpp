/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/ImageSequence.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.ImageSequence", ImageSequence, Image)

ImageSequence::ImageSequence(const std::wstring& name)
:	Image(name)
{
}

ImageSequence::~ImageSequence()
{
}

void ImageSequence::addImage(render::ITexture* texture)
{
	if (texture)
	{
		m_images.push_back(texture);

		// If this is the first image, set it as current
		if (m_images.size() == 1)
			updateCurrentImage();
	}
}

void ImageSequence::insertImage(int index, render::ITexture* texture)
{
	if (texture && index >= 0 && index <= (int)m_images.size())
	{
		m_images.insert(m_images.begin() + index, texture);

		// Update current frame if needed
		if (m_currentFrame >= index)
			m_currentFrame++;

		updateCurrentImage();
	}
}

void ImageSequence::removeImage(int index)
{
	if (index >= 0 && index < (int)m_images.size())
	{
		m_images.erase(m_images.begin() + index);

		// Clamp current frame
		if (m_currentFrame >= (int)m_images.size())
			m_currentFrame = std::max(0, (int)m_images.size() - 1);

		updateCurrentImage();
	}
}

void ImageSequence::clearImages()
{
	m_images.clear();
	m_currentFrame = 0;
	m_isPlaying = false;
	m_isPaused = false;
	m_frameTime = 0.0f;
	updateCurrentImage();
}

render::ITexture* ImageSequence::getImage(int index) const
{
	if (index >= 0 && index < (int)m_images.size())
		return m_images[index];
	return nullptr;
}

void ImageSequence::play()
{
	if (m_images.empty())
		return;

	m_isPlaying = true;
	m_isPaused = false;
	m_frameTime = 0.0f;
}

void ImageSequence::stop()
{
	m_isPlaying = false;
	m_isPaused = false;
	m_frameTime = 0.0f;
	setCurrentFrame(0);
}

void ImageSequence::pause()
{
	m_isPaused = true;
}

void ImageSequence::resume()
{
	m_isPaused = false;
}

void ImageSequence::reset()
{
	setCurrentFrame(0);
	m_frameTime = 0.0f;
}

void ImageSequence::setCurrentFrame(int frame)
{
	if (m_images.empty())
		return;

	frame = clamp(frame, 0, (int)m_images.size() - 1);

	if (m_currentFrame != frame)
	{
		m_currentFrame = frame;
		updateCurrentImage();

		if (onFrameChanged)
			onFrameChanged(m_currentFrame);
	}
}

void ImageSequence::update(float deltaTime)
{
	Image::update(deltaTime);

	if (m_isPlaying && !m_isPaused && !m_images.empty())
	{
		m_frameTime += deltaTime;

		// Advance frames
		while (m_frameTime >= m_frameDelay)
		{
			m_frameTime -= m_frameDelay;
			advanceFrame();
		}
	}
}

void ImageSequence::updateCurrentImage()
{
	if (m_currentFrame >= 0 && m_currentFrame < (int)m_images.size())
		setTexture(m_images[m_currentFrame]);
	else
		setTexture(nullptr);
}

void ImageSequence::advanceFrame()
{
	if (m_images.empty())
		return;

	int nextFrame = m_currentFrame + 1;

	if (nextFrame >= (int)m_images.size())
	{
		if (m_looping)
		{
			// Loop to first frame
			setCurrentFrame(0);
		}
		else
		{
			// Stop at last frame
			m_isPlaying = false;

			if (onSequenceComplete)
				onSequenceComplete();
		}
	}
	else
	{
		setCurrentFrame(nextFrame);
	}
}

}
