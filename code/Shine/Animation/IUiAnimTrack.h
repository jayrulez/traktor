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
#include "Core/Math/Vector2.h"
#include "Core/Math/Vector4.h"
#include "Core/Math/Color4f.h"

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
 * \brief Animation value types
 */
enum class UiAnimValueType
{
	Float,
	Vector2,
	Vector3,
	Vector4,
	Color,
	Bool,
	Int
};

/*!
 * \brief Animation curve types
 */
enum class UiAnimCurveType
{
	Linear,     // Linear interpolation
	Step,       // No interpolation (instant change)
	Bezier,     // Bezier curve with tangents
	Hermite     // Hermite spline
};

/*!
 * \brief Base animation key
 */
struct UiAnimKey
{
	float time = 0.0f;

	UiAnimKey() = default;
	UiAnimKey(float t) : time(t) {}

	bool operator<(const UiAnimKey& other) const { return time < other.time; }
};

/*!
 * \brief Float key with Bezier tangents
 */
struct UiAnimKeyFloat : public UiAnimKey
{
	float value = 0.0f;
	float inTangent = 0.0f;   // Incoming tangent
	float outTangent = 0.0f;  // Outgoing tangent

	UiAnimKeyFloat() = default;
	UiAnimKeyFloat(float t, float v) : UiAnimKey(t), value(v) {}
};

/*!
 * \brief Vector2 key with Bezier tangents
 */
struct UiAnimKeyVector2 : public UiAnimKey
{
	Vector2 value;
	Vector2 inTangent;
	Vector2 outTangent;

	UiAnimKeyVector2() = default;
	UiAnimKeyVector2(float t, const Vector2& v) : UiAnimKey(t), value(v) {}
};

/*!
 * \brief Vector4 key (also used for Color)
 */
struct UiAnimKeyVector4 : public UiAnimKey
{
	Vector4 value;
	Vector4 inTangent;
	Vector4 outTangent;

	UiAnimKeyVector4() = default;
	UiAnimKeyVector4(float t, const Vector4& v) : UiAnimKey(t), value(v) {}
};

/*!
 * \brief Bool key (stepped, no interpolation)
 */
struct UiAnimKeyBool : public UiAnimKey
{
	bool value = false;

	UiAnimKeyBool() = default;
	UiAnimKeyBool(float t, bool v) : UiAnimKey(t), value(v) {}
};

/*!
 * \brief Base interface for animation tracks
 */
class IUiAnimTrack : public Object
{
	T_RTTI_CLASS;

public:
	virtual ~IUiAnimTrack() = default;

	// === Track Information ===

	virtual UiAnimValueType getValueType() const = 0;
	virtual UiAnimCurveType getCurveType() const = 0;
	virtual void setCurveType(UiAnimCurveType type) = 0;

	// === Keyframe Management ===

	virtual int getKeyCount() const = 0;
	virtual void removeKey(int index) = 0;
	virtual void removeAllKeys() = 0;

	// === Time Range ===

	virtual void getKeyTimeRange(float& startTime, float& endTime) const = 0;

	// === Value Evaluation ===

	virtual void getValue(float time, float& value) const { value = 0.0f; }
	virtual void getValue(float time, Vector2& value) const { value = Vector2::zero(); }
	virtual void getValue(float time, Vector4& value) const { value = Vector4::zero(); }
	virtual void getValue(float time, Color4f& value) const { value = Color4f(0, 0, 0, 0); }
	virtual void getValue(float time, bool& value) const { value = false; }
	virtual void getValue(float time, int& value) const { value = 0; }

	// === Flags ===

	virtual bool isModified() const { return m_modified; }
	virtual void setModified(bool modified) { m_modified = modified; }

protected:
	bool m_modified = false;
};

/*!
 * \brief Float animation track
 */
class UiAnimTrackFloat : public IUiAnimTrack
{
	T_RTTI_CLASS;

public:
	UiAnimTrackFloat() = default;

	// IUiAnimTrack implementation
	virtual UiAnimValueType getValueType() const override { return UiAnimValueType::Float; }
	virtual UiAnimCurveType getCurveType() const override { return m_curveType; }
	virtual void setCurveType(UiAnimCurveType type) override { m_curveType = type; }

	virtual int getKeyCount() const override { return (int)m_keys.size(); }
	virtual void removeKey(int index) override;
	virtual void removeAllKeys() override { m_keys.clear(); m_modified = true; }

	virtual void getKeyTimeRange(float& startTime, float& endTime) const override;
	virtual void getValue(float time, float& value) const override;

	// Float-specific methods
	void setKey(int index, const UiAnimKeyFloat& key);
	int addKey(const UiAnimKeyFloat& key);
	const UiAnimKeyFloat& getKey(int index) const { return m_keys[index]; }
	void sortKeys();

private:
	AlignedVector<UiAnimKeyFloat> m_keys;
	UiAnimCurveType m_curveType = UiAnimCurveType::Bezier;

	void findKey(float time, int& keyIndex) const;
	float interpolate(const UiAnimKeyFloat& key1, const UiAnimKeyFloat& key2, float time) const;
	float interpolateBezier(const UiAnimKeyFloat& key1, const UiAnimKeyFloat& key2, float time) const;
};

/*!
 * \brief Vector2 animation track
 */
class UiAnimTrackVector2 : public IUiAnimTrack
{
	T_RTTI_CLASS;

public:
	UiAnimTrackVector2() = default;

	virtual UiAnimValueType getValueType() const override { return UiAnimValueType::Vector2; }
	virtual UiAnimCurveType getCurveType() const override { return m_curveType; }
	virtual void setCurveType(UiAnimCurveType type) override { m_curveType = type; }

	virtual int getKeyCount() const override { return (int)m_keys.size(); }
	virtual void removeKey(int index) override;
	virtual void removeAllKeys() override { m_keys.clear(); m_modified = true; }

	virtual void getKeyTimeRange(float& startTime, float& endTime) const override;
	virtual void getValue(float time, Vector2& value) const override;

	void setKey(int index, const UiAnimKeyVector2& key);
	int addKey(const UiAnimKeyVector2& key);
	const UiAnimKeyVector2& getKey(int index) const { return m_keys[index]; }
	void sortKeys();

private:
	AlignedVector<UiAnimKeyVector2> m_keys;
	UiAnimCurveType m_curveType = UiAnimCurveType::Bezier;

	void findKey(float time, int& keyIndex) const;
	Vector2 interpolate(const UiAnimKeyVector2& key1, const UiAnimKeyVector2& key2, float time) const;
	Vector2 interpolateBezier(const UiAnimKeyVector2& key1, const UiAnimKeyVector2& key2, float time) const;
};

/*!
 * \brief Vector4 animation track (also used for Color)
 */
class UiAnimTrackVector4 : public IUiAnimTrack
{
	T_RTTI_CLASS;

public:
	UiAnimTrackVector4(UiAnimValueType valueType = UiAnimValueType::Vector4)
		: m_valueType(valueType) {}

	virtual UiAnimValueType getValueType() const override { return m_valueType; }
	virtual UiAnimCurveType getCurveType() const override { return m_curveType; }
	virtual void setCurveType(UiAnimCurveType type) override { m_curveType = type; }

	virtual int getKeyCount() const override { return (int)m_keys.size(); }
	virtual void removeKey(int index) override;
	virtual void removeAllKeys() override { m_keys.clear(); m_modified = true; }

	virtual void getKeyTimeRange(float& startTime, float& endTime) const override;
	virtual void getValue(float time, Vector4& value) const override;
	virtual void getValue(float time, Color4f& value) const override;

	void setKey(int index, const UiAnimKeyVector4& key);
	int addKey(const UiAnimKeyVector4& key);
	const UiAnimKeyVector4& getKey(int index) const { return m_keys[index]; }
	void sortKeys();

private:
	AlignedVector<UiAnimKeyVector4> m_keys;
	UiAnimCurveType m_curveType = UiAnimCurveType::Bezier;
	UiAnimValueType m_valueType;

	void findKey(float time, int& keyIndex) const;
	Vector4 interpolate(const UiAnimKeyVector4& key1, const UiAnimKeyVector4& key2, float time) const;
	Vector4 interpolateBezier(const UiAnimKeyVector4& key1, const UiAnimKeyVector4& key2, float time) const;
};

/*!
 * \brief Bool animation track (stepped, no interpolation)
 */
class UiAnimTrackBool : public IUiAnimTrack
{
	T_RTTI_CLASS;

public:
	UiAnimTrackBool() = default;

	virtual UiAnimValueType getValueType() const override { return UiAnimValueType::Bool; }
	virtual UiAnimCurveType getCurveType() const override { return UiAnimCurveType::Step; }
	virtual void setCurveType(UiAnimCurveType type) override { /* Always stepped */ }

	virtual int getKeyCount() const override { return (int)m_keys.size(); }
	virtual void removeKey(int index) override;
	virtual void removeAllKeys() override { m_keys.clear(); m_modified = true; }

	virtual void getKeyTimeRange(float& startTime, float& endTime) const override;
	virtual void getValue(float time, bool& value) const override;

	void setKey(int index, const UiAnimKeyBool& key);
	int addKey(const UiAnimKeyBool& key);
	const UiAnimKeyBool& getKey(int index) const { return m_keys[index]; }
	void sortKeys();

private:
	AlignedVector<UiAnimKeyBool> m_keys;

	void findKey(float time, int& keyIndex) const;
};

}
