/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Object.h"
#include "Core/Ref.h"
#include "Core/Containers/AlignedVector.h"
#include "Shine/Animation/UiAnimSequence.h"

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
 * \brief Playing sequence state
 */
struct PlayingUiAnimSequence
{
	Ref<UiAnimSequence> sequence;
	float startTime;           // Start time of playback range
	float endTime;             // End time of playback range
	float currentTime;         // Current playback time
	float speed;               // Playback speed multiplier
	bool looping;              // Whether sequence loops
	bool paused;               // Whether playback is paused
};

/*!
 * \brief UI Animation System - manages playback of animation sequences
 *
 * This is a global singleton that manages all UI animations.
 */
class UiAnimationSystem : public Object
{
	T_RTTI_CLASS;

public:
	UiAnimationSystem();
	virtual ~UiAnimationSystem();

	// === Singleton Access ===

	static UiAnimationSystem* getInstance();

	// === Sequence Management ===

	/*!
	 * \brief Create a new sequence
	 */
	UiAnimSequence* createSequence(const std::wstring& name);

	/*!
	 * \brief Add sequence to system
	 */
	void addSequence(UiAnimSequence* sequence);

	/*!
	 * \brief Remove sequence from system
	 */
	void removeSequence(UiAnimSequence* sequence);

	/*!
	 * \brief Find sequence by name
	 */
	UiAnimSequence* findSequence(const std::wstring& name) const;

	/*!
	 * \brief Find sequence by ID
	 */
	UiAnimSequence* findSequenceById(uint32_t id) const;

	/*!
	 * \brief Get sequence by index
	 */
	UiAnimSequence* getSequence(int index) const;

	/*!
	 * \brief Get number of sequences
	 */
	int getSequenceCount() const { return (int)m_sequences.size(); }

	/*!
	 * \brief Remove all sequences
	 */
	void removeAllSequences();

	// === Playback Control ===

	/*!
	 * \brief Play sequence
	 */
	void playSequence(UiAnimSequence* sequence, bool looping = false, float speed = 1.0f);

	/*!
	 * \brief Play sequence by name
	 */
	void playSequence(const std::wstring& name, bool looping = false, float speed = 1.0f);

	/*!
	 * \brief Stop sequence
	 */
	void stopSequence(UiAnimSequence* sequence);

	/*!
	 * \brief Stop sequence by name
	 */
	void stopSequence(const std::wstring& name);

	/*!
	 * \brief Stop all sequences
	 */
	void stopAllSequences();

	/*!
	 * \brief Pause sequence
	 */
	void pauseSequence(UiAnimSequence* sequence);

	/*!
	 * \brief Resume sequence
	 */
	void resumeSequence(UiAnimSequence* sequence);

	/*!
	 * \brief Check if sequence is playing
	 */
	bool isPlaying(UiAnimSequence* sequence) const;

	/*!
	 * \brief Get playing time of sequence
	 */
	float getPlayingTime(UiAnimSequence* sequence) const;

	/*!
	 * \brief Set playing time of sequence
	 */
	void setPlayingTime(UiAnimSequence* sequence, float time);

	/*!
	 * \brief Get playback speed of sequence
	 */
	float getPlayingSpeed(UiAnimSequence* sequence) const;

	/*!
	 * \brief Set playback speed of sequence
	 */
	void setPlayingSpeed(UiAnimSequence* sequence, float speed);

	// === Update ===

	/*!
	 * \brief Update all playing sequences (call every frame)
	 */
	void update(float deltaTime);

private:
	static UiAnimationSystem* s_instance;

	AlignedVector<Ref<UiAnimSequence>> m_sequences;
	AlignedVector<PlayingUiAnimSequence> m_playingSequences;
	uint32_t m_nextSequenceId;

	// Helper methods
	PlayingUiAnimSequence* findPlayingSequence(UiAnimSequence* sequence);
	const PlayingUiAnimSequence* findPlayingSequence(UiAnimSequence* sequence) const;
};

}
