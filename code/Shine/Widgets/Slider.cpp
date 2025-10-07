/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/Slider.h"
#include "Shine/Widgets/Image.h"
#include "Shine/RenderContext.h"
#include <algorithm>

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Slider", Slider, Interactable)

Slider::Slider(const std::wstring& name)
:	Interactable(name)
{
	createDefaultWidgets();
	updateHandlePosition();
	updateFillSize();
}

Slider::~Slider()
{
}

void Slider::setValue(float value)
{
	float clampedValue = clampValue(value);
	clampedValue = applyStep(clampedValue);

	if (m_value != clampedValue)
	{
		m_value = clampedValue;
		updateHandlePosition();
		updateFillSize();

		if (onValueChanged)
			onValueChanged(m_value);
	}
}

void Slider::setMinValue(float minValue)
{
	if (m_minValue != minValue)
	{
		m_minValue = minValue;
		setValue(m_value); // Re-clamp
	}
}

void Slider::setMaxValue(float maxValue)
{
	if (m_maxValue != maxValue)
	{
		m_maxValue = maxValue;
		setValue(m_value); // Re-clamp
	}
}

float Slider::getNormalizedValue() const
{
	if (m_maxValue <= m_minValue)
		return 0.0f;

	return (m_value - m_minValue) / (m_maxValue - m_minValue);
}

void Slider::setNormalizedValue(float normalized)
{
	normalized = clamp(normalized, 0.0f, 1.0f);
	float value = m_minValue + normalized * (m_maxValue - m_minValue);
	setValue(value);
}

void Slider::setOrientation(Orientation orientation)
{
	if (m_orientation != orientation)
	{
		m_orientation = orientation;
		updateHandlePosition();
		updateFillSize();
	}
}

bool Slider::onMouseButtonDown(MouseButton button, const Vector2& position)
{
	if (!Interactable::onMouseButtonDown(button, position))
		return false;

	if (button == MouseButton::Left)
	{
		// Check if clicking on handle
		if (m_handleImage && m_handleImage->getWorldBounds().inside(position))
		{
			m_draggingHandle = true;
		}
		else
		{
			// Clicking on track - jump to position
			setValueFromPosition(position);
			m_draggingHandle = true;
		}

		return true;
	}

	return false;
}

bool Slider::onMouseButtonUp(MouseButton button, const Vector2& position)
{
	bool handled = Interactable::onMouseButtonUp(button, position);

	if (button == MouseButton::Left && m_draggingHandle)
	{
		m_draggingHandle = false;
		return true;
	}

	return handled;
}

bool Slider::onMouseMove(const Vector2& position)
{
	bool handled = Interactable::onMouseMove(position);

	if (m_draggingHandle)
	{
		setValueFromPosition(position);
		return true;
	}

	return handled;
}

void Slider::update(float deltaTime)
{
	Interactable::update(deltaTime);
}

void Slider::render(RenderContext* rc, float parentOpacity)
{
	// Let base class handle rendering (will render children)
	Interactable::render(rc, parentOpacity);
}

void Slider::createDefaultWidgets()
{
	Vector2 size = getSize();
	if (size.x < 10) size.x = 200;
	if (size.y < 10) size.y = 20;
	setSize(size);

	// Create track
	Ref<Image> track = new Image(L"SliderTrack");
	if (m_orientation == Orientation::Horizontal)
	{
		track->setPosition(Vector2(0, size.y * 0.5f - 2));
		track->setSize(Vector2(size.x, 4));
	}
	else
	{
		track->setPosition(Vector2(size.x * 0.5f - 2, 0));
		track->setSize(Vector2(4, size.y));
	}
	track->setColor(Color4f(0.3f, 0.3f, 0.3f, 1));
	addChild(track);
	m_trackImage = track;

	// Create fill (filled portion)
	Ref<Image> fill = new Image(L"SliderFill");
	fill->setPosition(track->getPosition());
	fill->setSize(Vector2(0, track->getSize().y));
	fill->setColor(Color4f(0, 0.8f, 1, 1)); // Cyan fill
	addChild(fill);
	m_fillImage = fill;

	// Create handle
	Ref<Image> handle = new Image(L"SliderHandle");
	if (m_orientation == Orientation::Horizontal)
	{
		handle->setSize(Vector2(16, size.y));
	}
	else
	{
		handle->setSize(Vector2(size.x, 16));
	}
	handle->setColor(Color4f(1, 1, 1, 1));
	addChild(handle);
	m_handleImage = handle;
}

void Slider::updateHandlePosition()
{
	if (!m_handleImage)
		return;

	Vector2 size = getSize();
	Vector2 handleSize = m_handleImage->getSize();
	float normalized = getNormalizedValue();

	if (m_orientation == Orientation::Horizontal)
	{
		float trackWidth = size.x - handleSize.x;
		float handleX = trackWidth * normalized;
		m_handleImage->setPosition(Vector2(handleX, 0));
	}
	else
	{
		float trackHeight = size.y - handleSize.y;
		float handleY = trackHeight * (1.0f - normalized); // Inverted for vertical
		m_handleImage->setPosition(Vector2(0, handleY));
	}
}

void Slider::updateFillSize()
{
	if (!m_fillImage || !m_trackImage)
		return;

	float normalized = getNormalizedValue();
	Vector2 trackSize = m_trackImage->getSize();

	if (m_orientation == Orientation::Horizontal)
	{
		m_fillImage->setSize(Vector2(trackSize.x * normalized, trackSize.y));
	}
	else
	{
		float fillHeight = trackSize.y * normalized;
		m_fillImage->setSize(Vector2(trackSize.x, fillHeight));
		// Position at bottom for vertical slider
		Vector2 trackPos = m_trackImage->getPosition();
		m_fillImage->setPosition(Vector2(trackPos.x, trackPos.y + trackSize.y - fillHeight));
	}
}

void Slider::setValueFromPosition(const Vector2& position)
{
	Vector2 localPos = position - getWorldPosition();
	Vector2 size = getSize();
	Vector2 handleSize = m_handleImage ? m_handleImage->getSize() : Vector2(0, 0);

	float normalized = 0.0f;

	if (m_orientation == Orientation::Horizontal)
	{
		float trackWidth = size.x - handleSize.x;
		normalized = (localPos.x - handleSize.x * 0.5f) / trackWidth;
	}
	else
	{
		float trackHeight = size.y - handleSize.y;
		normalized = 1.0f - ((localPos.y - handleSize.y * 0.5f) / trackHeight); // Inverted
	}

	setNormalizedValue(normalized);
}

float Slider::clampValue(float value) const
{
	return clamp(value, m_minValue, m_maxValue);
}

float Slider::applyStep(float value) const
{
	if (m_stepSize <= 0.0f)
		return value;

	// Round to nearest step
	float steps = std::round((value - m_minValue) / m_stepSize);
	return m_minValue + steps * m_stepSize;
}

}
