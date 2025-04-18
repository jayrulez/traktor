/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Config.h"

namespace traktor::mesh
{

struct MeshMaterialTechnique
{
	std::wstring worldTechnique;	//< World render technique, e.g. "Default", "Depth", "Velocity" etc.
	std::wstring shaderTechnique;	//< Shader, render, technique. e.g. "Mnnnnnnnn".
	uint32_t hash;					//< Shader technique hash.
};

}
