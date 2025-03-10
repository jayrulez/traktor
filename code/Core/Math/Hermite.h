/*
 * TRAKTOR
 * Copyright (c) 2022-2025 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Config.h"
#include "Core/Math/Const.h"
#include "Core/Math/ISpline.h"
#include "Core/Math/SplineControl.h"

namespace traktor
{

/*! Default element accessor.
 * \ingroup Core
 */
template <
	typename Key,
	typename Value >
struct HermiteAccessor
{
	static inline float time(const Key* keys, size_t nkeys, const Key& key)
	{
		return key.T;
	}

	static inline Value value(const Key& key)
	{
		return Value(key.value);
	}

	static inline Value combine(
		float t,
		const Value& v0, float w0,
		const Value& v1, float w1,
		const Value& v2, float w2,
		const Value& v3, float w3)
	{
		return Value(v0 * w0 + v1 * w1 + v2 * w2 + v3 * w3);
	}
};

/*! Hermite spline evaluator.
 * \ingroup Core
 */
template <
	typename Key,
	typename Value,
	typename Accessor = HermiteAccessor< Key, Value >,
	typename TimeControl = ClampTime >
class Hermite : public ISpline< Value >
{
public:
	explicit Hermite(const Key* keys, size_t nkeys, float Tend = -1.0f, float stiffness = 0.5f)
		: m_keys(keys)
		, m_nkeys(nkeys)
		, m_Tend(Tend)
		, m_stiffness(stiffness)
	{
	}

	virtual Value evaluate(float T) const
	{
		T_ASSERT(m_nkeys >= 2);

		const float Tfirst = Accessor::time(m_keys, m_nkeys, m_keys[0]);
		const float Tlast = Accessor::time(m_keys, m_nkeys, m_keys[m_nkeys - 1]);
		const float Tcurr = TimeControl::t(T, Tfirst, Tlast, m_Tend > 0.0f ? m_Tend : Tlast);

		// Binary search for key.
		int32_t index = 0;
		if (Tcurr < Tlast)
		{
			int32_t index0 = 0;
			int32_t index1 = int32_t(m_nkeys - 2);
			while (index0 < index1)
			{
				index = (index0 + index1) / 2;

				const float Tkey0 = Accessor::time(m_keys, m_nkeys, m_keys[index]);
				const float Tkey1 = Accessor::time(m_keys, m_nkeys, m_keys[index + 1]);

				if (Tcurr < Tkey0)
					index1 = index - 1;
				else if (Tcurr > Tkey1)
					index0 = index + 1;
				else
					break;
			}
		}
		else
			index = int32_t(m_nkeys - 1);

		const int32_t index_n1 = TimeControl::index(index - 1, int32_t(m_nkeys - 1));
		const int32_t index_1 = TimeControl::index(index + 1, int32_t(m_nkeys - 1));
		const int32_t index_2 = TimeControl::index(index + 2, int32_t(m_nkeys - 1));

		const Key& cp0 = m_keys[index];
		const Key& cp1 = m_keys[index_1];

		const Value v0 = Accessor::value(cp0);
		const Value v1 = Accessor::value(cp1);

		float t0 = Accessor::time(m_keys, m_nkeys, cp0);
		float t1 = Accessor::time(m_keys, m_nkeys, cp1);

		if (t0 >= t1)
			t1 = m_Tend;

		const Key& cpp = m_keys[index_n1];
		const Key& cpn = m_keys[index_2];

		const Value vp = Accessor::value(cpp);
		const Value vn = Accessor::value(cpn);

		const float t = (Tcurr - t0) / (t1 - t0);
		const float t2 = t * t;
		const float t3 = t2 * t;

		const float h2 = 3.0f * t2 - t3 - t3;
		const float h1 = 1.0f - h2;
		float h4 = t3 - t2;
		float h3 = h4 - t2 + t;

		h3 *= m_stiffness;
		h4 *= m_stiffness;

		return Accessor::combine(
			t,
			v0,
			h1 - h4,
			v1,
			h2 + h3,
			vp,
			-h3,
			vn,
			h4);
	}

private:
	const Key* m_keys;
	size_t m_nkeys;
	float m_Tend;
	float m_stiffness;
};

}
