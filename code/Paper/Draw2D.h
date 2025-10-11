/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Object.h"
#include "Core/Ref.h"
#include "Core/Math/Color4f.h"
#include "Core/Math/Matrix44.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Vector4.h"
#include "Resource/Proxy.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_PAPER_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::render
{

class Buffer;
class IRenderSystem;
class IRenderView;
class ITexture;
class IVertexLayout;
class Shader;

}

namespace traktor::resource
{

class IResourceManager;

}

namespace traktor::paper
{

/*! Draw2D - Simple 2D renderer
 * \ingroup Paper
 *
 * Simple 2D renderer for Paper UI with support for textured quads.
 */
class T_DLLCLASS Draw2D : public Object
{
	T_RTTI_CLASS;

public:
	bool create(
		resource::IResourceManager* resourceManager,
		render::IRenderSystem* renderSystem
	);

	void destroy();

	void begin(const Matrix44& projection);

	void end();

	void render(render::IRenderView* renderView);

	void drawTexturedQuad(
		const Vector2& position,
		const Vector2& size,
		const Vector2& uvMin,
		const Vector2& uvMax,
		const Color4f& color,
		render::ITexture* texture
	);

private:
	Ref< render::IRenderSystem > m_renderSystem;
	resource::Proxy< render::Shader > m_shader;
	Ref< const render::IVertexLayout > m_vertexLayout;
	Ref< render::Buffer > m_vertexBuffer;
	Matrix44 m_projection;
	uint32_t m_vertexCount;
};

}
