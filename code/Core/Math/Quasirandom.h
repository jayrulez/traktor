/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Math/Vector2.h"
#include "Core/Math/Vector4.h"

namespace traktor
{

class Random;

/*! Quasirandom methods.
 * \ingroup Core
 */
class T_DLLCLASS Quasirandom
{
public:
	static Vector2 hammersley(uint32_t i, uint32_t numSamples);

	static Vector2 hammersley(uint32_t i, uint32_t numSamples, Random& rnd);

	static Vector4 uniformSphere(const Vector2& uv);

	static Vector4 uniformHemiSphere(const Vector2& uv, const Vector4& direction);

	static Vector4 uniformCone(const Vector2& uv, const Vector4& direction, float radius);

	/*! Random direction, with Gaussian probability distribution. */
	static Vector4 lambertian(const Vector2& uv, const Vector4& direction);
};

}
