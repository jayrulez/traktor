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
 * \brief Fader widget for fade in/out transitions.
 *
 * Fader controls the opacity of its children over time, useful for
 * fade transitions and timed visibility effects.
 *
 * Usage:
 *   Ref<Fader> fader = new Fader(L"MenuFader");
 *   fader->fadeIn(1.0f);  // Fade in over 1 second
 *   fader->fadeOut(0.5f); // Fade out over 0.5 seconds
 *   fader->onFadeComplete = []() { nextScreen(); };
 */
class T_DLLCLASS Fader : public Visual
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Fade state
	 */
	enum class FadeState
	{
		Idle,      // Not fading
		FadingIn,  // Fading in (opacity 0 -> 1)
		FadingOut  // Fading out (opacity 1 -> 0)
	};

	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit Fader(const std::wstring& name = L"Fader");

	/*!
	 * \brief Destructor
	 */
	virtual ~Fader();

	// === Fade Control ===

	/*!
	 * \brief Fade in over duration
	 */
	void fadeIn(float duration);

	/*!
	 * \brief Fade out over duration
	 */
	void fadeOut(float duration);

	/*!
	 * \brief Stop fading
	 */
	void stopFade();

	/*!
	 * \brief Get current fade state
	 */
	FadeState getFadeState() const { return m_fadeState; }

	/*!
	 * \brief Get fade progress (0-1)
	 */
	float getFadeProgress() const;

	// === Configuration ===

	/*!
	 * \brief Get initial opacity (for fade in)
	 */
	float getInitialOpacity() const { return m_initialOpacity; }

	/*!
	 * \brief Set initial opacity
	 */
	void setInitialOpacity(float opacity) { m_initialOpacity = opacity; }

	/*!
	 * \brief Get target opacity (for fade out)
	 */
	float getTargetOpacity() const { return m_targetOpacity; }

	/*!
	 * \brief Set target opacity
	 */
	void setTargetOpacity(float opacity) { m_targetOpacity = opacity; }

	// === Events ===

	std::function<void()> onFadeComplete;

	// === Update ===

	virtual void update(float deltaTime) override;

private:
	FadeState m_fadeState = FadeState::Idle;
	float m_fadeTime = 0.0f;
	float m_fadeDuration = 1.0f;
	float m_initialOpacity = 0.0f;
	float m_targetOpacity = 1.0f;

	// Helper methods
	void updateFade(float deltaTime);
};

}
