/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Render/Editor/SH/RayLeighPhaseFunction.h"

namespace traktor::render
{

T_IMPLEMENT_RTTI_CLASS(L"RayLeighPhaseFunction", RayLeighPhaseFunction, SHFunction)

Vector4 RayLeighPhaseFunction::evaluate(const Polar& direction) const
{
	const Vector4 c_viewDirection(0.0f, 0.0f, 1.0f, 0.0f);
	const Scalar a = max(dot3(c_viewDirection, direction.toUnitCartesian()) * 2.0_simd, 0.0_simd);
	//return 1.0f - (a * a + 1.0f) * 3.0f / 4.0f;
	return Vector4(a, a, a, 0.0f);
}

}
