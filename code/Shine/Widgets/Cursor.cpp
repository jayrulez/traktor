/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/Cursor.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Cursor", Cursor, Image)

Cursor::Cursor(const std::wstring& name)
:	Image(name)
{
	// Cursor should render on top of everything
	setRenderOrder(10000);
}

Cursor::~Cursor()
{
}

void Cursor::setCursorState(State state)
{
	if (m_cursorState != state)
	{
		m_cursorState = state;
		updateCursorImage();

		if (onStateChanged)
			onStateChanged(state);
	}
}

void Cursor::setNormalImage(render::ITexture* texture)
{
	m_normalImage = texture;
	if (m_cursorState == State::Normal)
		updateCursorImage();
}

void Cursor::setHoverImage(render::ITexture* texture)
{
	m_hoverImage = texture;
	if (m_cursorState == State::Hover)
		updateCursorImage();
}

void Cursor::setClickImage(render::ITexture* texture)
{
	m_clickImage = texture;
	if (m_cursorState == State::Click)
		updateCursorImage();
}

void Cursor::updateToMousePosition(const Vector2& mousePosition)
{
	// Set cursor position to mouse position plus offset
	setPosition(mousePosition + m_offset);
}

void Cursor::updateCursorImage()
{
	render::ITexture* targetTexture = nullptr;

	switch (m_cursorState)
	{
	case State::Normal:
		targetTexture = m_normalImage;
		setVisible(true);
		break;

	case State::Hover:
		targetTexture = m_hoverImage ? m_hoverImage.ptr() : m_normalImage.ptr();
		setVisible(true);
		break;

	case State::Click:
		targetTexture = m_clickImage ? m_clickImage.ptr() : m_normalImage.ptr();
		setVisible(true);
		break;

	case State::Disabled:
		setVisible(false);
		return;
	}

	setTexture(targetTexture);
}

}
