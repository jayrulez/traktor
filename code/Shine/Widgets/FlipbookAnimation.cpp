/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/FlipbookAnimation.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.FlipbookAnimation", FlipbookAnimation, Image)

FlipbookAnimation::FlipbookAnimation(const std::wstring& name)
:	Image(name)
{
	updateFrameUVs();
}

FlipbookAnimation::~FlipbookAnimation()
{
}

void FlipbookAnimation::start()
{
	m_isPlaying = true;
	m_currentFrame = m_startFrame;
	m_prevFrame = m_currentFrame;
	m_elapsedTime = 0.0f;
	m_useStartDelay = (m_startDelay > 0.0f);
	m_currentLoopDirection = 1;
	updateFrameUVs();
}

void FlipbookAnimation::stop()
{
	m_isPlaying = false;
	m_currentFrame = m_startFrame;
	m_prevFrame = m_currentFrame;
	m_elapsedTime = 0.0f;
	m_currentLoopDirection = 1;
	updateFrameUVs();
}

void FlipbookAnimation::setStartFrame(uint32_t frame)
{
	m_startFrame = frame;

	// Ensure end frame is at least start frame
	if (m_endFrame < m_startFrame)
		m_endFrame = m_startFrame;

	// Ensure loop start is within range
	if (m_loopStartFrame < m_startFrame)
		m_loopStartFrame = m_startFrame;
	if (m_loopStartFrame > m_endFrame)
		m_loopStartFrame = m_startFrame;
}

void FlipbookAnimation::setEndFrame(uint32_t frame)
{
	m_endFrame = frame;

	// Ensure start frame is at most end frame
	if (m_startFrame > m_endFrame)
		m_startFrame = m_endFrame;

	// Ensure loop start is within range
	if (m_loopStartFrame < m_startFrame)
		m_loopStartFrame = m_startFrame;
	if (m_loopStartFrame > m_endFrame)
		m_loopStartFrame = m_endFrame;
}

void FlipbookAnimation::setCurrentFrame(uint32_t frame)
{
	m_currentFrame = clamp(frame, m_startFrame, m_endFrame);
	updateFrameUVs();
}

void FlipbookAnimation::setLoopStartFrame(uint32_t frame)
{
	m_loopStartFrame = clamp(frame, m_startFrame, m_endFrame);
}

void FlipbookAnimation::setFramerate(float framerate)
{
	m_framerate = std::max(0.0f, framerate);
}

void FlipbookAnimation::setStartDelay(float delay)
{
	m_startDelay = std::max(0.0f, delay);
}

void FlipbookAnimation::setLoopDelay(float delay)
{
	m_loopDelay = std::max(0.0f, delay);
}

void FlipbookAnimation::setReverseDelay(float delay)
{
	m_reverseDelay = std::max(0.0f, delay);
}

void FlipbookAnimation::setFramesPerRow(int frames)
{
	if (m_framesPerRow != frames)
	{
		m_framesPerRow = std::max(1, frames);
		updateFrameUVs();
	}
}

void FlipbookAnimation::onActivate()
{
	if (m_isAutoPlay)
		start();
}

void FlipbookAnimation::update(float deltaTime)
{
	Image::update(deltaTime);

	if (!m_isPlaying)
		return;

	m_elapsedTime += deltaTime;

	// Handle start delay
	if (m_useStartDelay)
	{
		if (m_elapsedTime >= m_startDelay)
		{
			m_elapsedTime -= m_startDelay;
			m_useStartDelay = false;
		}
		else
		{
			return; // Still waiting for start delay
		}
	}

	// Calculate frame time
	float frameTime = calculateFramerateAsSecondsPerFrame();
	if (frameTime <= 0.0f)
		return;

	// Handle loop delay
	float loopDelay = calculateLoopDelay();
	if (loopDelay > 0.0f && m_elapsedTime < loopDelay)
		return;

	// Advance frames
	while (m_elapsedTime >= frameTime)
	{
		m_elapsedTime -= frameTime;
		advanceFrame();
	}
}

void FlipbookAnimation::updateFrameUVs()
{
	if (m_framesPerRow <= 0)
		return;

	// Calculate frame position in grid
	int column = m_currentFrame % m_framesPerRow;
	int row = m_currentFrame / m_framesPerRow;

	// Calculate total rows (assuming equal rows for simplicity)
	int totalFrames = m_endFrame + 1; // Assuming 0-based indexing
	int totalRows = (totalFrames + m_framesPerRow - 1) / m_framesPerRow;

	// Calculate UV coordinates
	float frameWidth = 1.0f / m_framesPerRow;
	float frameHeight = 1.0f / totalRows;

	Vector2 uvMin(column * frameWidth, row * frameHeight);
	Vector2 uvMax((column + 1) * frameWidth, (row + 1) * frameHeight);

	// Set UV coordinates on Image base class
	setUVRect(uvMin, uvMax);
}

void FlipbookAnimation::advanceFrame()
{
	m_prevFrame = m_currentFrame;

	// Advance frame based on direction
	m_currentFrame += m_currentLoopDirection;

	// Handle end of sequence
	if (m_currentLoopDirection > 0 && m_currentFrame > m_endFrame)
	{
		// Forward direction hit end
		if (isPingPongLoopType())
		{
			// Reverse direction
			m_currentLoopDirection = -1;
			m_currentFrame = m_endFrame - 1;
			m_elapsedTime = 0.0f; // Reset for reverse delay
		}
		else if (isLoopingType())
		{
			// Loop back to loop start
			m_currentFrame = m_loopStartFrame;
			m_elapsedTime = 0.0f; // Reset for loop delay

			if (onLoopComplete)
				onLoopComplete();
		}
		else
		{
			// No loop, stop at end
			m_currentFrame = m_endFrame;
			m_isPlaying = false;

			if (onAnimationComplete)
				onAnimationComplete();
		}
	}
	else if (m_currentLoopDirection < 0 && m_currentFrame < m_loopStartFrame)
	{
		// Reverse direction hit start (PingPong)
		if (isPingPongLoopType())
		{
			// Forward direction again
			m_currentLoopDirection = 1;
			m_currentFrame = m_loopStartFrame + 1;
			m_elapsedTime = 0.0f; // Reset for loop delay

			if (onLoopComplete)
				onLoopComplete();
		}
		else
		{
			// Should not happen, but stop at loop start
			m_currentFrame = m_loopStartFrame;
			m_isPlaying = false;
		}
	}

	updateFrameUVs();
}

float FlipbookAnimation::calculateFramerateAsSecondsPerFrame() const
{
	if (m_framerateUnit == FramerateUnit::FPS && m_framerate > 0.0f)
		return 1.0f / m_framerate;
	else
		return m_framerate;
}

float FlipbookAnimation::calculateLoopDelay() const
{
	if (m_currentLoopDirection > 0)
		return m_loopDelay;
	else
		return m_reverseDelay;
}

bool FlipbookAnimation::isLoopingType() const
{
	return m_loopType == LoopType::Linear || m_loopType == LoopType::PingPong;
}

bool FlipbookAnimation::isPingPongLoopType() const
{
	return m_loopType == LoopType::PingPong;
}

}
