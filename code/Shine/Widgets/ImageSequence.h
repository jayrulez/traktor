/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Containers/AlignedVector.h"
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
 * \brief ImageSequence widget for playing sequences of discrete images.
 *
 * ImageSequence displays a sequence of separate images in order, creating
 * animation from distinct image files (unlike FlipbookAnimation which uses
 * a single sprite sheet).
 *
 * Usage:
 *   Ref<ImageSequence> seq = new ImageSequence(L"Explosion");
 *   seq->addImage(explosionFrame1);
 *   seq->addImage(explosionFrame2);
 *   seq->addImage(explosionFrame3);
 *   seq->setFrameDelay(0.05f);  // 20 FPS
 *   seq->setLooping(true);
 *   seq->play();
 */
class T_DLLCLASS ImageSequence : public Image
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit ImageSequence(const std::wstring& name = L"ImageSequence");

	/*!
	 * \brief Destructor
	 */
	virtual ~ImageSequence();

	// === Sequence Management ===

	/*!
	 * \brief Add image to sequence
	 */
	void addImage(render::ITexture* texture);

	/*!
	 * \brief Insert image at index
	 */
	void insertImage(int index, render::ITexture* texture);

	/*!
	 * \brief Remove image at index
	 */
	void removeImage(int index);

	/*!
	 * \brief Clear all images
	 */
	void clearImages();

	/*!
	 * \brief Get image count
	 */
	int getImageCount() const { return (int)m_images.size(); }

	/*!
	 * \brief Get image at index
	 */
	render::ITexture* getImage(int index) const;

	// === Animation Control ===

	/*!
	 * \brief Start playing sequence
	 */
	void play();

	/*!
	 * \brief Stop playing sequence
	 */
	void stop();

	/*!
	 * \brief Pause sequence
	 */
	void pause();

	/*!
	 * \brief Resume paused sequence
	 */
	void resume();

	/*!
	 * \brief Reset to first image
	 */
	void reset();

	/*!
	 * \brief Get whether sequence is playing
	 */
	bool isPlaying() const { return m_isPlaying && !m_isPaused; }

	/*!
	 * \brief Get whether sequence is paused
	 */
	bool isPaused() const { return m_isPaused; }

	// === Frame Configuration ===

	/*!
	 * \brief Get current frame index
	 */
	int getCurrentFrame() const { return m_currentFrame; }

	/*!
	 * \brief Set current frame
	 */
	void setCurrentFrame(int frame);

	/*!
	 * \brief Get frame delay (seconds per frame)
	 */
	float getFrameDelay() const { return m_frameDelay; }

	/*!
	 * \brief Set frame delay
	 */
	void setFrameDelay(float delay) { m_frameDelay = delay; }

	// === Looping ===

	/*!
	 * \brief Get whether sequence loops
	 */
	bool getLooping() const { return m_looping; }

	/*!
	 * \brief Set whether sequence loops
	 */
	void setLooping(bool looping) { m_looping = looping; }

	// === Events ===

	std::function<void()> onSequenceComplete;
	std::function<void(int)> onFrameChanged;

	// === Update ===

	virtual void update(float deltaTime) override;

private:
	AlignedVector<Ref<render::ITexture>> m_images;
	int m_currentFrame = 0;
	float m_frameDelay = 0.1f; // 10 FPS default
	bool m_looping = true;

	// Playback state
	bool m_isPlaying = false;
	bool m_isPaused = false;
	float m_frameTime = 0.0f;

	// Helper methods
	void updateCurrentImage();
	void advanceFrame();
};

}
