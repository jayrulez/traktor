/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/Scroller.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Scroller", Scroller, UIElement)

Scroller::Scroller(const std::wstring& name)
:	UIElement(name)
{
}

Scroller::~Scroller()
{
}

void Scroller::play()
{
	m_isPlaying = true;
	m_isPaused = false;
}

void Scroller::stop()
{
	m_isPlaying = false;
	m_isPaused = false;
	reset();
}

void Scroller::pause()
{
	m_isPaused = true;
}

void Scroller::resume()
{
	m_isPaused = false;
}

void Scroller::reset()
{
	m_scrollOffset = Vector2(0.0f, 0.0f);
	updateChildPositions();
}

void Scroller::setScrollOffset(const Vector2& offset)
{
	m_scrollOffset = offset;
	updateChildPositions();
}

void Scroller::update(float deltaTime)
{
	UIElement::update(deltaTime);

	if (m_isPlaying && !m_isPaused)
	{
		// Update scroll offset
		m_scrollOffset += m_scrollSpeed * deltaTime;

		// Check loop bounds
		checkLoopBounds();

		// Update child positions
		updateChildPositions();
	}
}

void Scroller::updateChildPositions()
{
	// Cache original positions if not done yet
	if (!m_childPositionsCached)
	{
		cacheChildPositions();
	}

	// Apply scroll offset to all children based on their original positions
	const auto& children = getChildren();
	for (size_t i = 0; i < children.size(); ++i)
	{
		if (i < m_originalChildPositions.size())
		{
			children[i]->setPosition(m_originalChildPositions[i] + m_scrollOffset);
		}
	}
}

void Scroller::cacheChildPositions()
{
	m_originalChildPositions.clear();
	for (const auto& child : getChildren())
	{
		m_originalChildPositions.push_back(child->getPosition());
	}
	m_childPositionsCached = true;
}

void Scroller::checkLoopBounds()
{
	bool looped = false;

	// Check horizontal loop
	if (m_loopBounds.x > 0.0f)
	{
		if (m_scrollSpeed.x > 0.0f && m_scrollOffset.x >= m_loopBounds.x)
		{
			m_scrollOffset.x = 0.0f;
			looped = true;
		}
		else if (m_scrollSpeed.x < 0.0f && m_scrollOffset.x <= -m_loopBounds.x)
		{
			m_scrollOffset.x = 0.0f;
			looped = true;
		}
	}

	// Check vertical loop
	if (m_loopBounds.y > 0.0f)
	{
		if (m_scrollSpeed.y > 0.0f && m_scrollOffset.y >= m_loopBounds.y)
		{
			m_scrollOffset.y = 0.0f;
			looped = true;
		}
		else if (m_scrollSpeed.y < 0.0f && m_scrollOffset.y <= -m_loopBounds.y)
		{
			m_scrollOffset.y = 0.0f;
			looped = true;
		}
	}

	if (looped)
	{
		if (m_looping)
		{
			if (onLoop)
				onLoop();
		}
		else
		{
			m_isPlaying = false;
			if (onScrollComplete)
				onScrollComplete();
		}
	}
}

}
