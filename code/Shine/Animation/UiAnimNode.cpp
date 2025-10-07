/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Animation/UiAnimNode.h"
#include "Shine/UIElement.h"
#include "Shine/Visual.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.UiAnimNode", UiAnimNode, Object)

UiAnimNode::UiAnimNode()
:	m_target(nullptr)
{
}

UiAnimNode::~UiAnimNode()
{
}

IUiAnimTrack* UiAnimNode::createTrack(const UiAnimParamType& paramType)
{
	// Check if track already exists
	for (auto& entry : m_tracks)
	{
		if (entry.paramType == paramType)
			return entry.track;
	}

	// Create new track
	Ref<IUiAnimTrack> track = createTrackForType(paramType.valueType);
	if (!track)
		return nullptr;

	TrackEntry entry;
	entry.paramType = paramType;
	entry.track = track;
	m_tracks.push_back(entry);

	return track;
}

IUiAnimTrack* UiAnimNode::getTrack(const UiAnimParamType& paramType) const
{
	for (const auto& entry : m_tracks)
	{
		if (entry.paramType == paramType)
			return entry.track;
	}
	return nullptr;
}

IUiAnimTrack* UiAnimNode::getTrackByIndex(int index) const
{
	if (index >= 0 && index < (int)m_tracks.size())
		return m_tracks[index].track;
	return nullptr;
}

void UiAnimNode::removeTrack(const UiAnimParamType& paramType)
{
	for (auto it = m_tracks.begin(); it != m_tracks.end(); ++it)
	{
		if (it->paramType == paramType)
		{
			m_tracks.erase(it);
			return;
		}
	}
}

void UiAnimNode::animate(float time)
{
	if (!m_target)
		return;

	// Apply all tracks
	for (const auto& entry : m_tracks)
	{
		applyTrackValue(entry.paramType, entry.track, time);
	}
}

void UiAnimNode::reset()
{
	// Reset to initial values (time = 0)
	animate(0.0f);
}

void UiAnimNode::applyTrackValue(const UiAnimParamType& paramType, IUiAnimTrack* track, float time)
{
	if (!track || !m_target)
		return;

	const std::wstring& propName = paramType.name;

	// Get value from track
	switch (paramType.valueType)
	{
	case UiAnimValueType::Float:
		{
			float value;
			track->getValue(time, value);

			// Apply to property
			if (propName == L"Opacity")
			{
				if (Visual* visual = dynamic_type_cast<Visual*>(m_target.ptr()))
					visual->setOpacity(value);
			}
			else if (propName == L"Rotation")
			{
				m_target->setRotation(value);
			}
			else if (propName == L"ScaleX")
			{
				Vector2 scale = m_target->getScale();
				scale.x = value;
				m_target->setScale(scale);
			}
			else if (propName == L"ScaleY")
			{
				Vector2 scale = m_target->getScale();
				scale.y = value;
				m_target->setScale(scale);
			}
		}
		break;

	case UiAnimValueType::Vector2:
		{
			Vector2 value;
			track->getValue(time, value);

			if (propName == L"Position")
			{
				m_target->setPosition(value);
			}
			else if (propName == L"Size")
			{
				m_target->setSize(value);
			}
			else if (propName == L"Scale")
			{
				m_target->setScale(value);
			}
			else if (propName == L"AnchorMin")
			{
				m_target->setAnchorMin(value);
			}
			else if (propName == L"AnchorMax")
			{
				m_target->setAnchorMax(value);
			}
			else if (propName == L"Pivot")
			{
				m_target->setPivot(value);
			}
		}
		break;

	case UiAnimValueType::Color:
		{
			Color4f value;
			track->getValue(time, value);

			if (propName == L"Color")
			{
				if (Visual* visual = dynamic_type_cast<Visual*>(m_target.ptr()))
					visual->setColor(value);
			}
		}
		break;

	case UiAnimValueType::Bool:
		{
			bool value;
			track->getValue(time, value);

			if (propName == L"Enabled")
			{
				m_target->setEnabled(value);
			}
			else if (propName == L"Visible")
			{
				m_target->setVisible(value);
			}
		}
		break;

	default:
		break;
	}
}

IUiAnimTrack* UiAnimNode::createTrackForType(UiAnimValueType valueType)
{
	switch (valueType)
	{
	case UiAnimValueType::Float:
		return new UiAnimTrackFloat();

	case UiAnimValueType::Vector2:
		return new UiAnimTrackVector2();

	case UiAnimValueType::Vector3:
	case UiAnimValueType::Vector4:
		return new UiAnimTrackVector4(valueType);

	case UiAnimValueType::Color:
		return new UiAnimTrackVector4(UiAnimValueType::Color);

	case UiAnimValueType::Bool:
		return new UiAnimTrackBool();

	default:
		return nullptr;
	}
}

}
