/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/ScrollBar.h"
#include "Shine/Widgets/Image.h"
#include "Shine/Widgets/Button.h"
#include "Shine/RenderContext.h"
#include <algorithm>

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.ScrollBar", ScrollBar, Interactable)

ScrollBar::ScrollBar(const std::wstring& name)
:	Interactable(name)
{
	createDefaultWidgets();
	updateLayout();
	updateThumbPosition();
}

ScrollBar::~ScrollBar()
{
}

void ScrollBar::setValue(float value)
{
	value = clamp(value, 0.0f, 1.0f);

	if (m_value != value)
	{
		m_value = value;
		updateThumbPosition();

		if (onValueChanged)
			onValueChanged(m_value);
	}
}

void ScrollBar::setContentSize(float size)
{
	if (m_contentSize != size)
	{
		m_contentSize = std::max(0.0f, size);
		updateThumbPosition();
		updateLayout();
	}
}

void ScrollBar::setViewportSize(float size)
{
	if (m_viewportSize != size)
	{
		m_viewportSize = std::max(0.0f, size);
		updateThumbPosition();
		updateLayout();
	}
}

float ScrollBar::getThumbSize() const
{
	if (m_contentSize <= 0.0f)
		return 0.0f;

	// Thumb size proportional to viewport/content ratio
	float ratio = m_viewportSize / m_contentSize;
	ratio = clamp(ratio, 0.1f, 1.0f); // Minimum 10% size

	Vector2 trackSize = m_trackImage ? m_trackImage->getSize() : getSize();
	float trackLength = (m_orientation == Orientation::Horizontal) ? trackSize.x : trackSize.y;

	return trackLength * ratio;
}

void ScrollBar::setOrientation(Orientation orientation)
{
	if (m_orientation != orientation)
	{
		m_orientation = orientation;
		updateLayout();
		updateThumbPosition();
	}
}

void ScrollBar::setShowButtons(bool show)
{
	if (m_showButtons != show)
	{
		m_showButtons = show;

		if (m_decrementButton)
			m_decrementButton->setVisible(show);
		if (m_incrementButton)
			m_incrementButton->setVisible(show);

		updateLayout();
	}
}

bool ScrollBar::onMouseButtonDown(MouseButton button, const Vector2& position)
{
	if (!Interactable::onMouseButtonDown(button, position))
		return false;

	if (button == MouseButton::Left)
	{
		// Check if clicking on thumb
		if (m_thumbImage && m_thumbImage->getWorldBounds().inside(position))
		{
			m_draggingThumb = true;
			m_dragStartPosition = position;
			m_dragStartValue = m_value;
			return true;
		}
		else if (m_trackImage && m_trackImage->getWorldBounds().inside(position))
		{
			// Clicking on track - page scroll
			Vector2 thumbPos = m_thumbImage ? m_thumbImage->getWorldPosition() : Vector2(0, 0);

			if (m_orientation == Orientation::Horizontal)
			{
				if (position.x < thumbPos.x)
					scroll(-0.1f); // Page up
				else
					scroll(0.1f);  // Page down
			}
			else
			{
				if (position.y < thumbPos.y)
					scroll(-0.1f); // Page up
				else
					scroll(0.1f);  // Page down
			}

			return true;
		}
	}

	return false;
}

bool ScrollBar::onMouseButtonUp(MouseButton button, const Vector2& position)
{
	bool handled = Interactable::onMouseButtonUp(button, position);

	if (button == MouseButton::Left && m_draggingThumb)
	{
		m_draggingThumb = false;
		return true;
	}

	return handled;
}

bool ScrollBar::onMouseMove(const Vector2& position)
{
	bool handled = Interactable::onMouseMove(position);

	if (m_draggingThumb)
	{
		setValueFromPosition(position);
		return true;
	}

	return handled;
}

bool ScrollBar::onMouseWheel(float delta)
{
	if (!isInteractable())
		return false;

	// Scroll with wheel
	scroll(-delta * 0.05f); // Negative because wheel up = scroll up (decrease value)
	return true;
}

void ScrollBar::update(float deltaTime)
{
	Interactable::update(deltaTime);
}

void ScrollBar::render(RenderContext* rc, float parentOpacity)
{
	// Let base class handle rendering (will render children)
	Interactable::render(rc, parentOpacity);
}

void ScrollBar::createDefaultWidgets()
{
	Vector2 size = getSize();
	if (m_orientation == Orientation::Horizontal)
	{
		if (size.x < 20) size.x = 200;
		if (size.y < 10) size.y = 16;
	}
	else
	{
		if (size.x < 10) size.x = 16;
		if (size.y < 20) size.y = 200;
	}
	setSize(size);

	float buttonSize = (m_orientation == Orientation::Horizontal) ? size.y : size.x;

	// Create decrement button
	Ref<Button> decBtn = new Button(L"DecrementButton");
	decBtn->setSize(Vector2(buttonSize, buttonSize));
	decBtn->setText(m_orientation == Orientation::Horizontal ? L"<" : L"^");
	decBtn->onClick = [this]() { scroll(-0.05f); };
	addChild(decBtn);
	m_decrementButton = decBtn;

	// Create increment button
	Ref<Button> incBtn = new Button(L"IncrementButton");
	incBtn->setSize(Vector2(buttonSize, buttonSize));
	incBtn->setText(m_orientation == Orientation::Horizontal ? L">" : L"v");
	incBtn->onClick = [this]() { scroll(0.05f); };
	addChild(incBtn);
	m_incrementButton = incBtn;

	// Create track
	Ref<Image> track = new Image(L"ScrollBarTrack");
	track->setColor(Color4f(0.2f, 0.2f, 0.2f, 1));
	addChild(track);
	m_trackImage = track;

	// Create thumb
	Ref<Image> thumb = new Image(L"ScrollBarThumb");
	thumb->setColor(Color4f(0.6f, 0.6f, 0.6f, 1));
	addChild(thumb);
	m_thumbImage = thumb;
}

void ScrollBar::updateThumbPosition()
{
	if (!m_thumbImage || !m_trackImage)
		return;

	Vector2 trackPos = m_trackImage->getPosition();
	Vector2 trackSize = m_trackImage->getSize();
	float thumbSize = getThumbSize();

	if (m_orientation == Orientation::Horizontal)
	{
		float trackLength = trackSize.x;
		float thumbRange = trackLength - thumbSize;
		float thumbX = trackPos.x + thumbRange * m_value;

		m_thumbImage->setPosition(Vector2(thumbX, trackPos.y));
		m_thumbImage->setSize(Vector2(thumbSize, trackSize.y));
	}
	else
	{
		float trackLength = trackSize.y;
		float thumbRange = trackLength - thumbSize;
		float thumbY = trackPos.y + thumbRange * m_value;

		m_thumbImage->setPosition(Vector2(trackPos.x, thumbY));
		m_thumbImage->setSize(Vector2(trackSize.x, thumbSize));
	}
}

void ScrollBar::updateLayout()
{
	if (!m_trackImage)
		return;

	Vector2 size = getSize();
	float buttonSize = (m_orientation == Orientation::Horizontal) ? size.y : size.x;

	if (m_orientation == Orientation::Horizontal)
	{
		// Position buttons
		if (m_decrementButton)
			m_decrementButton->setPosition(Vector2(0, 0));
		if (m_incrementButton)
			m_incrementButton->setPosition(Vector2(size.x - buttonSize, 0));

		// Position track
		float trackX = m_showButtons ? buttonSize : 0;
		float trackWidth = m_showButtons ? (size.x - buttonSize * 2) : size.x;
		m_trackImage->setPosition(Vector2(trackX, 0));
		m_trackImage->setSize(Vector2(trackWidth, size.y));
	}
	else
	{
		// Position buttons
		if (m_decrementButton)
			m_decrementButton->setPosition(Vector2(0, 0));
		if (m_incrementButton)
			m_incrementButton->setPosition(Vector2(0, size.y - buttonSize));

		// Position track
		float trackY = m_showButtons ? buttonSize : 0;
		float trackHeight = m_showButtons ? (size.y - buttonSize * 2) : size.y;
		m_trackImage->setPosition(Vector2(0, trackY));
		m_trackImage->setSize(Vector2(size.x, trackHeight));
	}

	updateThumbPosition();
}

void ScrollBar::setValueFromPosition(const Vector2& position)
{
	if (!m_thumbImage || !m_trackImage)
		return;

	Vector2 delta = position - m_dragStartPosition;
	Vector2 trackSize = m_trackImage->getSize();
	float thumbSize = getThumbSize();

	float deltaValue = 0.0f;

	if (m_orientation == Orientation::Horizontal)
	{
		float trackLength = trackSize.x;
		float thumbRange = trackLength - thumbSize;
		if (thumbRange > 0)
			deltaValue = delta.x / thumbRange;
	}
	else
	{
		float trackLength = trackSize.y;
		float thumbRange = trackLength - thumbSize;
		if (thumbRange > 0)
			deltaValue = delta.y / thumbRange;
	}

	setValue(m_dragStartValue + deltaValue);
}

void ScrollBar::scroll(float delta)
{
	setValue(m_value + delta);
}

}
