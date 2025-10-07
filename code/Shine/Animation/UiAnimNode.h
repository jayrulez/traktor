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
#include "Core/Containers/SmallMap.h"
#include "Shine/Animation/IUiAnimTrack.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

class UIElement;

/*!
 * \brief Parameter type that can be animated
 */
struct UiAnimParamType
{
	std::wstring name;        // Property name (e.g., "Position", "Color", "Opacity")
	UiAnimValueType valueType;

	UiAnimParamType() : valueType(UiAnimValueType::Float) {}
	UiAnimParamType(const std::wstring& n, UiAnimValueType vt) : name(n), valueType(vt) {}

	bool operator==(const UiAnimParamType& other) const { return name == other.name; }
	bool operator<(const UiAnimParamType& other) const { return name < other.name; }
};

/*!
 * \brief Animation node - represents an animated UI element
 *
 * Each node can have multiple tracks (one per animated property)
 */
class UiAnimNode : public Object
{
	T_RTTI_CLASS;

public:
	UiAnimNode();
	virtual ~UiAnimNode();

	// === Target Element ===

	UIElement* getTarget() const { return m_target; }
	void setTarget(UIElement* target) { m_target = target; }

	// === Node Info ===

	const std::wstring& getName() const { return m_name; }
	void setName(const std::wstring& name) { m_name = name; }

	// === Track Management ===

	/*!
	 * \brief Create or get track for parameter
	 */
	IUiAnimTrack* createTrack(const UiAnimParamType& paramType);

	/*!
	 * \brief Get track for parameter
	 */
	IUiAnimTrack* getTrack(const UiAnimParamType& paramType) const;

	/*!
	 * \brief Get track by index
	 */
	IUiAnimTrack* getTrackByIndex(int index) const;

	/*!
	 * \brief Get number of tracks
	 */
	int getTrackCount() const { return (int)m_tracks.size(); }

	/*!
	 * \brief Remove track
	 */
	void removeTrack(const UiAnimParamType& paramType);

	// === Animation ===

	/*!
	 * \brief Animate node at given time
	 */
	void animate(float time);

	/*!
	 * \brief Reset node to initial state
	 */
	void reset();

private:
	struct TrackEntry
	{
		UiAnimParamType paramType;
		Ref<IUiAnimTrack> track;
	};

	Ref<UIElement> m_target;
	std::wstring m_name;
	AlignedVector<TrackEntry> m_tracks;

	// Helper methods
	void applyTrackValue(const UiAnimParamType& paramType, IUiAnimTrack* track, float time);
	IUiAnimTrack* createTrackForType(UiAnimValueType valueType);
};

}
