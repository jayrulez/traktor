/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Animation/UiAnimationSystem.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.UiAnimationSystem", UiAnimationSystem, Object)

UiAnimationSystem* UiAnimationSystem::s_instance = nullptr;

UiAnimationSystem::UiAnimationSystem()
:	m_nextSequenceId(1)
{
	s_instance = this;
}

UiAnimationSystem::~UiAnimationSystem()
{
	if (s_instance == this)
		s_instance = nullptr;
}

UiAnimationSystem* UiAnimationSystem::getInstance()
{
	if (!s_instance)
		s_instance = new UiAnimationSystem();
	return s_instance;
}

UiAnimSequence* UiAnimationSystem::createSequence(const std::wstring& name)
{
	Ref<UiAnimSequence> sequence = new UiAnimSequence();
	sequence->setName(name);
	sequence->setId(m_nextSequenceId++);

	m_sequences.push_back(sequence);
	return sequence;
}

void UiAnimationSystem::addSequence(UiAnimSequence* sequence)
{
	if (!sequence)
		return;

	// Assign ID if not set
	if (sequence->getId() == 0)
		sequence->setId(m_nextSequenceId++);

	m_sequences.push_back(sequence);
}

void UiAnimationSystem::removeSequence(UiAnimSequence* sequence)
{
	// Stop if playing
	stopSequence(sequence);

	// Remove from list
	for (auto it = m_sequences.begin(); it != m_sequences.end(); ++it)
	{
		if (*it == sequence)
		{
			m_sequences.erase(it);
			return;
		}
	}
}

UiAnimSequence* UiAnimationSystem::findSequence(const std::wstring& name) const
{
	for (const auto& seq : m_sequences)
	{
		if (seq->getName() == name)
			return seq;
	}
	return nullptr;
}

UiAnimSequence* UiAnimationSystem::findSequenceById(uint32_t id) const
{
	for (const auto& seq : m_sequences)
	{
		if (seq->getId() == id)
			return seq;
	}
	return nullptr;
}

UiAnimSequence* UiAnimationSystem::getSequence(int index) const
{
	if (index >= 0 && index < (int)m_sequences.size())
		return m_sequences[index];
	return nullptr;
}

void UiAnimationSystem::removeAllSequences()
{
	stopAllSequences();
	m_sequences.clear();
}

void UiAnimationSystem::playSequence(UiAnimSequence* sequence, bool looping, float speed)
{
	if (!sequence)
		return;

	// Stop if already playing
	stopSequence(sequence);

	// Create playing state
	PlayingUiAnimSequence playing;
	playing.sequence = sequence;
	playing.startTime = sequence->getStartTime();
	playing.endTime = sequence->getEndTime();
	playing.currentTime = playing.startTime;
	playing.speed = speed;
	playing.looping = looping;
	playing.paused = false;

	m_playingSequences.push_back(playing);

	// Trigger start event
	if (sequence->onSequenceStart)
		sequence->onSequenceStart();

	// Animate at start time
	sequence->animate(playing.currentTime);
}

void UiAnimationSystem::playSequence(const std::wstring& name, bool looping, float speed)
{
	UiAnimSequence* sequence = findSequence(name);
	if (sequence)
		playSequence(sequence, looping, speed);
}

void UiAnimationSystem::stopSequence(UiAnimSequence* sequence)
{
	for (auto it = m_playingSequences.begin(); it != m_playingSequences.end(); ++it)
	{
		if (it->sequence == sequence)
		{
			// Trigger end event
			if (sequence->onSequenceEnd)
				sequence->onSequenceEnd();

			m_playingSequences.erase(it);
			return;
		}
	}
}

void UiAnimationSystem::stopSequence(const std::wstring& name)
{
	UiAnimSequence* sequence = findSequence(name);
	if (sequence)
		stopSequence(sequence);
}

void UiAnimationSystem::stopAllSequences()
{
	// Trigger end events
	for (const auto& playing : m_playingSequences)
	{
		if (playing.sequence->onSequenceEnd)
			playing.sequence->onSequenceEnd();
	}

	m_playingSequences.clear();
}

void UiAnimationSystem::pauseSequence(UiAnimSequence* sequence)
{
	PlayingUiAnimSequence* playing = findPlayingSequence(sequence);
	if (playing)
		playing->paused = true;
}

void UiAnimationSystem::resumeSequence(UiAnimSequence* sequence)
{
	PlayingUiAnimSequence* playing = findPlayingSequence(sequence);
	if (playing)
		playing->paused = false;
}

bool UiAnimationSystem::isPlaying(UiAnimSequence* sequence) const
{
	return findPlayingSequence(sequence) != nullptr;
}

float UiAnimationSystem::getPlayingTime(UiAnimSequence* sequence) const
{
	const PlayingUiAnimSequence* playing = findPlayingSequence(sequence);
	return playing ? playing->currentTime : 0.0f;
}

void UiAnimationSystem::setPlayingTime(UiAnimSequence* sequence, float time)
{
	PlayingUiAnimSequence* playing = findPlayingSequence(sequence);
	if (playing)
	{
		playing->currentTime = clamp(time, playing->startTime, playing->endTime);
		sequence->animate(playing->currentTime);
	}
}

float UiAnimationSystem::getPlayingSpeed(UiAnimSequence* sequence) const
{
	const PlayingUiAnimSequence* playing = findPlayingSequence(sequence);
	return playing ? playing->speed : 1.0f;
}

void UiAnimationSystem::setPlayingSpeed(UiAnimSequence* sequence, float speed)
{
	PlayingUiAnimSequence* playing = findPlayingSequence(sequence);
	if (playing)
		playing->speed = speed;
}

void UiAnimationSystem::update(float deltaTime)
{
	// Update all playing sequences
	for (auto it = m_playingSequences.begin(); it != m_playingSequences.end(); )
	{
		PlayingUiAnimSequence& playing = *it;

		if (playing.paused)
		{
			++it;
			continue;
		}

		// Advance time
		playing.currentTime += deltaTime * playing.speed;

		// Check if sequence finished
		if (playing.currentTime >= playing.endTime)
		{
			if (playing.looping)
			{
				// Loop back to start
				playing.currentTime = playing.startTime + (playing.currentTime - playing.endTime);
				playing.sequence->animate(playing.currentTime);
			}
			else
			{
				// Stop sequence
				playing.sequence->animate(playing.endTime);

				// Trigger end event
				if (playing.sequence->onSequenceEnd)
					playing.sequence->onSequenceEnd();

				it = m_playingSequences.erase(it);
				continue;
			}
		}
		else
		{
			// Animate at current time
			playing.sequence->animate(playing.currentTime);
		}

		++it;
	}
}

PlayingUiAnimSequence* UiAnimationSystem::findPlayingSequence(UiAnimSequence* sequence)
{
	for (auto& playing : m_playingSequences)
	{
		if (playing.sequence == sequence)
			return &playing;
	}
	return nullptr;
}

const PlayingUiAnimSequence* UiAnimationSystem::findPlayingSequence(UiAnimSequence* sequence) const
{
	for (const auto& playing : m_playingSequences)
	{
		if (playing.sequence == sequence)
			return &playing;
	}
	return nullptr;
}

}
