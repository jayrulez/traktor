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

#include "Render/Types.h"

#include "tb_config.h"

namespace traktor::render
{
	class ITexture;
	class Buffer;
}

namespace traktor::turbobadgerui
{
#pragma pack(push, 1)  // Set alignment to 1 byte

	struct TurboBadgerUiVertexColor
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

	struct TurboBadgerUiVertex
	{
		float position[2];
		float texCoord[2];
		TurboBadgerUiVertexColor color;
	};
#pragma pack(pop)  // Restore the previous packing alignment

	struct TurboBadgerUiBatch
	{
		Ref < render::ITexture > texture;
		Ref < render::Buffer > vertexBuffer;
		uint32_t triangleCount;
		render::Rectangle clipRect;
	};
}