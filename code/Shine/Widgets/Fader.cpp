/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/Fader.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Fader", Fader, Visual)

Fader::Fader(const std::wstring& name)
:	Visual(name)
{
}

Fader::~Fader()
{
}

void Fader::fadeIn(float duration)
{
	m_fadeState = FadeState::FadingIn;
	m_fadeDuration = duration;
	m_fadeTime = 0.0f;

	// Set initial opacity
	setOpacity(m_initialOpacity);
	setVisible(true);
}

void Fader::fadeOut(float duration)
{
	m_fadeState = FadeState::FadingOut;
	m_fadeDuration = duration;
	m_fadeTime = 0.0f;

	// Ensure visible for fade out
	setVisible(true);
}

void Fader::stopFade()
{
	m_fadeState = FadeState::Idle;
	m_fadeTime = 0.0f;
}

float Fader::getFadeProgress() const
{
	if (m_fadeDuration <= 0.0f)
		return 1.0f;

	return clamp(m_fadeTime / m_fadeDuration, 0.0f, 1.0f);
}

void Fader::update(float deltaTime)
{
	Visual::update(deltaTime);

	if (m_fadeState != FadeState::Idle)
		updateFade(deltaTime);
}

void Fader::updateFade(float deltaTime)
{
	m_fadeTime += deltaTime;

	float progress = getFadeProgress();
	bool fadeComplete = progress >= 1.0f;

	// Update opacity based on fade direction
	if (m_fadeState == FadeState::FadingIn)
	{
		float opacity = m_initialOpacity + (m_targetOpacity - m_initialOpacity) * progress;
		setOpacity(opacity);

		if (fadeComplete)
		{
			setOpacity(m_targetOpacity);
			m_fadeState = FadeState::Idle;

			if (onFadeComplete)
				onFadeComplete();
		}
	}
	else if (m_fadeState == FadeState::FadingOut)
	{
		float opacity = m_targetOpacity + (m_initialOpacity - m_targetOpacity) * (1.0f - progress);
		setOpacity(opacity);

		if (fadeComplete)
		{
			setOpacity(m_initialOpacity);
			setVisible(false); // Hide after fade out
			m_fadeState = FadeState::Idle;

			if (onFadeComplete)
				onFadeComplete();
		}
	}
}

}
