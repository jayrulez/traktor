/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

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
 * \brief Advanced FlipbookAnimation widget for sprite sheet frame animation.
 *
 * FlipbookAnimation plays through frames in a sprite sheet with support for:
 * - Multiple loop types (None, Linear, PingPong)
 * - Intro sequences (different start frame from loop start)
 * - Configurable framerate (FPS or seconds per frame)
 * - Start, loop, and reverse delays
 * - Auto-play option
 *
 * Usage:
 *   Ref<FlipbookAnimation> anim = new FlipbookAnimation(L"Explosion");
 *   anim->setStartFrame(0);
 *   anim->setEndFrame(15);
 *   anim->setLoopStartFrame(4);  // Frames 0-3 are intro
 *   anim->setLoopType(FlipbookAnimation::LoopType::Linear);
 *   anim->setFramerate(20.0f);  // 20 FPS
 *   anim->setAutoPlay(true);
 */
class T_DLLCLASS FlipbookAnimation : public Image
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Loop type
	 */
	enum class LoopType
	{
		None,      // Play once and stop
		Linear,    // Loop from loop start to end
		PingPong   // Play forward then backward
	};

	/*!
	 * \brief Framerate units
	 */
	enum class FramerateUnit
	{
		FPS,                 // Frames per second
		SecondsPerFrame      // Seconds to wait per frame
	};

	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit FlipbookAnimation(const std::wstring& name = L"FlipbookAnimation");

	/*!
	 * \brief Destructor
	 */
	virtual ~FlipbookAnimation();

	// === Animation Control ===

	/*!
	 * \brief Start playing animation
	 */
	void start();

	/*!
	 * \brief Stop playing animation
	 */
	void stop();

	/*!
	 * \brief Get whether animation is playing
	 */
	bool isPlaying() const { return m_isPlaying; }

	// === Frame Configuration ===

	/*!
	 * \brief Get start frame (first frame of animation)
	 */
	uint32_t getStartFrame() const { return m_startFrame; }

	/*!
	 * \brief Set start frame
	 */
	void setStartFrame(uint32_t frame);

	/*!
	 * \brief Get end frame (last frame of animation)
	 */
	uint32_t getEndFrame() const { return m_endFrame; }

	/*!
	 * \brief Set end frame
	 */
	void setEndFrame(uint32_t frame);

	/*!
	 * \brief Get current frame
	 */
	uint32_t getCurrentFrame() const { return m_currentFrame; }

	/*!
	 * \brief Set current frame
	 */
	void setCurrentFrame(uint32_t frame);

	/*!
	 * \brief Get loop start frame (for looping animations)
	 */
	uint32_t getLoopStartFrame() const { return m_loopStartFrame; }

	/*!
	 * \brief Set loop start frame
	 */
	void setLoopStartFrame(uint32_t frame);

	// === Loop Configuration ===

	/*!
	 * \brief Get loop type
	 */
	LoopType getLoopType() const { return m_loopType; }

	/*!
	 * \brief Set loop type
	 */
	void setLoopType(LoopType type) { m_loopType = type; }

	// === Framerate Configuration ===

	/*!
	 * \brief Get framerate
	 */
	float getFramerate() const { return m_framerate; }

	/*!
	 * \brief Set framerate
	 */
	void setFramerate(float framerate);

	/*!
	 * \brief Get framerate unit
	 */
	FramerateUnit getFramerateUnit() const { return m_framerateUnit; }

	/*!
	 * \brief Set framerate unit
	 */
	void setFramerateUnit(FramerateUnit unit) { m_framerateUnit = unit; }

	// === Delay Configuration ===

	/*!
	 * \brief Get start delay (delay before playing)
	 */
	float getStartDelay() const { return m_startDelay; }

	/*!
	 * \brief Set start delay
	 */
	void setStartDelay(float delay);

	/*!
	 * \brief Get loop delay (delay before looping)
	 */
	float getLoopDelay() const { return m_loopDelay; }

	/*!
	 * \brief Set loop delay
	 */
	void setLoopDelay(float delay);

	/*!
	 * \brief Get reverse delay (delay before reversing in PingPong)
	 */
	float getReverseDelay() const { return m_reverseDelay; }

	/*!
	 * \brief Set reverse delay
	 */
	void setReverseDelay(float delay);

	// === Auto-play ===

	/*!
	 * \brief Get whether animation auto-plays
	 */
	bool getAutoPlay() const { return m_isAutoPlay; }

	/*!
	 * \brief Set whether animation auto-plays
	 */
	void setAutoPlay(bool autoPlay) { m_isAutoPlay = autoPlay; }

	// === Sprite Sheet Layout ===

	/*!
	 * \brief Get frames per row in sprite sheet
	 */
	int getFramesPerRow() const { return m_framesPerRow; }

	/*!
	 * \brief Set frames per row
	 */
	void setFramesPerRow(int frames);

	// === Events ===

	std::function<void()> onAnimationComplete;
	std::function<void()> onLoopComplete;

	// === Lifecycle ===

	void onActivate();  // Called when widget is activated

	// === Update ===

	virtual void update(float deltaTime) override;

private:
	// Frame configuration
	uint32_t m_startFrame = 0;           // First frame of animation
	uint32_t m_endFrame = 0;             // Last frame of animation
	uint32_t m_loopStartFrame = 0;       // First frame when looping
	uint32_t m_currentFrame = 0;         // Current frame being displayed
	uint32_t m_prevFrame = 0;            // Previous frame (for intro detection)

	// Loop configuration
	LoopType m_loopType = LoopType::None;
	int32_t m_currentLoopDirection = 1;  // 1 for forward, -1 for reverse (PingPong)

	// Framerate configuration
	float m_framerate = 10.0f;           // Framerate value
	FramerateUnit m_framerateUnit = FramerateUnit::FPS;

	// Delay configuration
	float m_startDelay = 0.0f;           // Delay before starting
	float m_loopDelay = 0.0f;            // Delay before looping
	float m_reverseDelay = 0.0f;         // Delay before reversing (PingPong)

	// Auto-play
	bool m_isAutoPlay = true;

	// Sprite sheet layout
	int m_framesPerRow = 4;

	// Playback state
	bool m_isPlaying = false;
	float m_elapsedTime = 0.0f;
	bool m_useStartDelay = false;

	// Helper methods
	void updateFrameUVs();
	void advanceFrame();
	float calculateFramerateAsSecondsPerFrame() const;
	float calculateLoopDelay() const;
	bool isLoopingType() const;
	bool isPingPongLoopType() const;
};

}
