/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Animation/IUiAnimTrack.h"
#include <algorithm>

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.IUiAnimTrack", IUiAnimTrack, Object)
T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.UiAnimTrackFloat", UiAnimTrackFloat, IUiAnimTrack)
T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.UiAnimTrackVector2", UiAnimTrackVector2, IUiAnimTrack)
T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.UiAnimTrackVector4", UiAnimTrackVector4, IUiAnimTrack)
T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.UiAnimTrackBool", UiAnimTrackBool, IUiAnimTrack)

// ============================================================================
// UiAnimTrackFloat
// ============================================================================

void UiAnimTrackFloat::removeKey(int index)
{
	if (index >= 0 && index < (int)m_keys.size())
	{
		m_keys.erase(m_keys.begin() + index);
		m_modified = true;
	}
}

void UiAnimTrackFloat::getKeyTimeRange(float& startTime, float& endTime) const
{
	if (m_keys.empty())
	{
		startTime = endTime = 0.0f;
		return;
	}

	startTime = m_keys.front().time;
	endTime = m_keys.back().time;
}

void UiAnimTrackFloat::getValue(float time, float& value) const
{
	if (m_keys.empty())
	{
		value = 0.0f;
		return;
	}

	if (m_keys.size() == 1 || time <= m_keys.front().time)
	{
		value = m_keys.front().value;
		return;
	}

	if (time >= m_keys.back().time)
	{
		value = m_keys.back().value;
		return;
	}

	// Find surrounding keys
	int keyIndex;
	findKey(time, keyIndex);

	if (keyIndex >= (int)m_keys.size() - 1)
	{
		value = m_keys.back().value;
		return;
	}

	value = interpolate(m_keys[keyIndex], m_keys[keyIndex + 1], time);
}

void UiAnimTrackFloat::setKey(int index, const UiAnimKeyFloat& key)
{
	if (index >= 0 && index < (int)m_keys.size())
	{
		m_keys[index] = key;
		m_modified = true;
	}
}

int UiAnimTrackFloat::addKey(const UiAnimKeyFloat& key)
{
	m_keys.push_back(key);
	m_modified = true;
	return (int)m_keys.size() - 1;
}

void UiAnimTrackFloat::sortKeys()
{
	std::sort(m_keys.begin(), m_keys.end());
	m_modified = true;
}

void UiAnimTrackFloat::findKey(float time, int& keyIndex) const
{
	// Binary search for key
	int left = 0;
	int right = (int)m_keys.size() - 1;

	while (left < right)
	{
		int mid = (left + right + 1) / 2;
		if (m_keys[mid].time <= time)
			left = mid;
		else
			right = mid - 1;
	}

	keyIndex = left;
}

float UiAnimTrackFloat::interpolate(const UiAnimKeyFloat& key1, const UiAnimKeyFloat& key2, float time) const
{
	switch (m_curveType)
	{
	case UiAnimCurveType::Step:
		return key1.value;

	case UiAnimCurveType::Linear:
		{
			float t = (time - key1.time) / (key2.time - key1.time);
			return key1.value + (key2.value - key1.value) * t;
		}

	case UiAnimCurveType::Bezier:
	case UiAnimCurveType::Hermite:
		return interpolateBezier(key1, key2, time);

	default:
		return key1.value;
	}
}

float UiAnimTrackFloat::interpolateBezier(const UiAnimKeyFloat& key1, const UiAnimKeyFloat& key2, float time) const
{
	float dt = key2.time - key1.time;
	if (dt <= 0.0f)
		return key1.value;

	float t = (time - key1.time) / dt;
	t = clamp(t, 0.0f, 1.0f);

	// Hermite interpolation with tangents
	float t2 = t * t;
	float t3 = t2 * t;

	float h1 = 2.0f * t3 - 3.0f * t2 + 1.0f;
	float h2 = -2.0f * t3 + 3.0f * t2;
	float h3 = t3 - 2.0f * t2 + t;
	float h4 = t3 - t2;

	return h1 * key1.value + h2 * key2.value + h3 * key1.outTangent * dt + h4 * key2.inTangent * dt;
}

// ============================================================================
// UiAnimTrackVector2
// ============================================================================

void UiAnimTrackVector2::removeKey(int index)
{
	if (index >= 0 && index < (int)m_keys.size())
	{
		m_keys.erase(m_keys.begin() + index);
		m_modified = true;
	}
}

void UiAnimTrackVector2::getKeyTimeRange(float& startTime, float& endTime) const
{
	if (m_keys.empty())
	{
		startTime = endTime = 0.0f;
		return;
	}

	startTime = m_keys.front().time;
	endTime = m_keys.back().time;
}

void UiAnimTrackVector2::getValue(float time, Vector2& value) const
{
	if (m_keys.empty())
	{
		value = Vector2::zero();
		return;
	}

	if (m_keys.size() == 1 || time <= m_keys.front().time)
	{
		value = m_keys.front().value;
		return;
	}

	if (time >= m_keys.back().time)
	{
		value = m_keys.back().value;
		return;
	}

	int keyIndex;
	findKey(time, keyIndex);

	if (keyIndex >= (int)m_keys.size() - 1)
	{
		value = m_keys.back().value;
		return;
	}

	value = interpolate(m_keys[keyIndex], m_keys[keyIndex + 1], time);
}

void UiAnimTrackVector2::setKey(int index, const UiAnimKeyVector2& key)
{
	if (index >= 0 && index < (int)m_keys.size())
	{
		m_keys[index] = key;
		m_modified = true;
	}
}

int UiAnimTrackVector2::addKey(const UiAnimKeyVector2& key)
{
	m_keys.push_back(key);
	m_modified = true;
	return (int)m_keys.size() - 1;
}

void UiAnimTrackVector2::sortKeys()
{
	std::sort(m_keys.begin(), m_keys.end());
	m_modified = true;
}

void UiAnimTrackVector2::findKey(float time, int& keyIndex) const
{
	int left = 0;
	int right = (int)m_keys.size() - 1;

	while (left < right)
	{
		int mid = (left + right + 1) / 2;
		if (m_keys[mid].time <= time)
			left = mid;
		else
			right = mid - 1;
	}

	keyIndex = left;
}

Vector2 UiAnimTrackVector2::interpolate(const UiAnimKeyVector2& key1, const UiAnimKeyVector2& key2, float time) const
{
	switch (m_curveType)
	{
	case UiAnimCurveType::Step:
		return key1.value;

	case UiAnimCurveType::Linear:
		{
			float t = (time - key1.time) / (key2.time - key1.time);
			return key1.value + (key2.value - key1.value) * t;
		}

	case UiAnimCurveType::Bezier:
	case UiAnimCurveType::Hermite:
		return interpolateBezier(key1, key2, time);

	default:
		return key1.value;
	}
}

Vector2 UiAnimTrackVector2::interpolateBezier(const UiAnimKeyVector2& key1, const UiAnimKeyVector2& key2, float time) const
{
	float dt = key2.time - key1.time;
	if (dt <= 0.0f)
		return key1.value;

	float t = (time - key1.time) / dt;
	t = clamp(t, 0.0f, 1.0f);

	float t2 = t * t;
	float t3 = t2 * t;

	float h1 = 2.0f * t3 - 3.0f * t2 + 1.0f;
	float h2 = -2.0f * t3 + 3.0f * t2;
	float h3 = t3 - 2.0f * t2 + t;
	float h4 = t3 - t2;

	return key1.value * h1 + key2.value * h2 + key1.outTangent * (h3 * dt) + key2.inTangent * (h4 * dt);
}

// ============================================================================
// UiAnimTrackVector4
// ============================================================================

void UiAnimTrackVector4::removeKey(int index)
{
	if (index >= 0 && index < (int)m_keys.size())
	{
		m_keys.erase(m_keys.begin() + index);
		m_modified = true;
	}
}

void UiAnimTrackVector4::getKeyTimeRange(float& startTime, float& endTime) const
{
	if (m_keys.empty())
	{
		startTime = endTime = 0.0f;
		return;
	}

	startTime = m_keys.front().time;
	endTime = m_keys.back().time;
}

void UiAnimTrackVector4::getValue(float time, Vector4& value) const
{
	if (m_keys.empty())
	{
		value = Vector4::zero();
		return;
	}

	if (m_keys.size() == 1 || time <= m_keys.front().time)
	{
		value = m_keys.front().value;
		return;
	}

	if (time >= m_keys.back().time)
	{
		value = m_keys.back().value;
		return;
	}

	int keyIndex;
	findKey(time, keyIndex);

	if (keyIndex >= (int)m_keys.size() - 1)
	{
		value = m_keys.back().value;
		return;
	}

	value = interpolate(m_keys[keyIndex], m_keys[keyIndex + 1], time);
}

void UiAnimTrackVector4::getValue(float time, Color4f& value) const
{
	Vector4 v;
	getValue(time, v);
	value = Color4f(v.x(), v.y(), v.z(), v.w());
}

void UiAnimTrackVector4::setKey(int index, const UiAnimKeyVector4& key)
{
	if (index >= 0 && index < (int)m_keys.size())
	{
		m_keys[index] = key;
		m_modified = true;
	}
}

int UiAnimTrackVector4::addKey(const UiAnimKeyVector4& key)
{
	m_keys.push_back(key);
	m_modified = true;
	return (int)m_keys.size() - 1;
}

void UiAnimTrackVector4::sortKeys()
{
	std::sort(m_keys.begin(), m_keys.end());
	m_modified = true;
}

void UiAnimTrackVector4::findKey(float time, int& keyIndex) const
{
	int left = 0;
	int right = (int)m_keys.size() - 1;

	while (left < right)
	{
		int mid = (left + right + 1) / 2;
		if (m_keys[mid].time <= time)
			left = mid;
		else
			right = mid - 1;
	}

	keyIndex = left;
}

Vector4 UiAnimTrackVector4::interpolate(const UiAnimKeyVector4& key1, const UiAnimKeyVector4& key2, float time) const
{
	switch (m_curveType)
	{
	case UiAnimCurveType::Step:
		return key1.value;

	case UiAnimCurveType::Linear:
		{
			float t = (time - key1.time) / (key2.time - key1.time);
			return key1.value + (key2.value - key1.value) * Scalar(t);
		}

	case UiAnimCurveType::Bezier:
	case UiAnimCurveType::Hermite:
		return interpolateBezier(key1, key2, time);

	default:
		return key1.value;
	}
}

Vector4 UiAnimTrackVector4::interpolateBezier(const UiAnimKeyVector4& key1, const UiAnimKeyVector4& key2, float time) const
{
	float dt = key2.time - key1.time;
	if (dt <= 0.0f)
		return key1.value;

	float t = (time - key1.time) / dt;
	t = clamp(t, 0.0f, 1.0f);

	float t2 = t * t;
	float t3 = t2 * t;

	float h1 = 2.0f * t3 - 3.0f * t2 + 1.0f;
	float h2 = -2.0f * t3 + 3.0f * t2;
	float h3 = t3 - 2.0f * t2 + t;
	float h4 = t3 - t2;

	return key1.value * Scalar(h1) + key2.value * Scalar(h2) + key1.outTangent * Scalar(h3 * dt) + key2.inTangent * Scalar(h4 * dt);
}

// ============================================================================
// UiAnimTrackBool
// ============================================================================

void UiAnimTrackBool::removeKey(int index)
{
	if (index >= 0 && index < (int)m_keys.size())
	{
		m_keys.erase(m_keys.begin() + index);
		m_modified = true;
	}
}

void UiAnimTrackBool::getKeyTimeRange(float& startTime, float& endTime) const
{
	if (m_keys.empty())
	{
		startTime = endTime = 0.0f;
		return;
	}

	startTime = m_keys.front().time;
	endTime = m_keys.back().time;
}

void UiAnimTrackBool::getValue(float time, bool& value) const
{
	if (m_keys.empty())
	{
		value = false;
		return;
	}

	if (time <= m_keys.front().time)
	{
		value = m_keys.front().value;
		return;
	}

	if (time >= m_keys.back().time)
	{
		value = m_keys.back().value;
		return;
	}

	int keyIndex;
	findKey(time, keyIndex);

	value = m_keys[keyIndex].value;
}

void UiAnimTrackBool::setKey(int index, const UiAnimKeyBool& key)
{
	if (index >= 0 && index < (int)m_keys.size())
	{
		m_keys[index] = key;
		m_modified = true;
	}
}

int UiAnimTrackBool::addKey(const UiAnimKeyBool& key)
{
	m_keys.push_back(key);
	m_modified = true;
	return (int)m_keys.size() - 1;
}

void UiAnimTrackBool::sortKeys()
{
	std::sort(m_keys.begin(), m_keys.end());
	m_modified = true;
}

void UiAnimTrackBool::findKey(float time, int& keyIndex) const
{
	int left = 0;
	int right = (int)m_keys.size() - 1;

	while (left < right)
	{
		int mid = (left + right + 1) / 2;
		if (m_keys[mid].time <= time)
			left = mid;
		else
			right = mid - 1;
	}

	keyIndex = left;
}

}
