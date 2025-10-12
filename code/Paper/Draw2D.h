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
#include "Core/RefArray.h"
#include "Core/Containers/AlignedVector.h"
#include "Core/Math/Color4f.h"
#include "Core/Math/Half.h"
#include "Core/Math/Matrix44.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Vector4.h"
#include "Core/Thread/Semaphore.h"
#include "Render/Types.h"
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
 * 2D renderer for Paper UI with batching support and transform stacks.
 * Optimized for 2D rendering without depth testing.
 */
class T_DLLCLASS Draw2D : public Object
{
	T_RTTI_CLASS;

public:
	bool create(
		resource::IResourceManager* resourceManager,
		render::IRenderSystem* renderSystem,
		uint32_t frameCount
	);

	void destroy();

	bool begin(uint32_t frame, const Matrix44& projection);

	void end(uint32_t frame);

	void render(render::IRenderView* renderView, uint32_t frame);

	void pushView(const Matrix44& view);

	void popView();

	void pushWorld(const Matrix44& transform);

	void popWorld();

	void setProjection(const Matrix44& projection);

	void drawTexturedQuad(
		const Vector2& position,
		const Vector2& size,
		const Vector2& uvMin,
		const Vector2& uvMax,
		const Color4f& color,
		render::ITexture* texture
	);

	const Matrix44& getProjection() const { return m_currentFrame->projections.back(); }

	const Matrix44& getView() const { return m_view.back(); }

	const Matrix44& getWorld() const { return m_world.back(); }

private:
	struct Batch
	{
		uint32_t projection;
		Ref< render::Buffer > vertexBuffer;
		Ref< render::ITexture > texture;
		render::Primitives primitives;
	};

	struct Frame
	{
		RefArray< render::Buffer > vertexBuffers;
		AlignedVector< Matrix44 > projections;
		AlignedVector< Batch > batches;
	};

	struct Vertex
	{
		float position[4];
		half_t texCoord[2];
		float color[4];

		void set(const Vector4& pos, const Vector2& uv, const Color4f& col);
	};

	// System
	Ref< render::IRenderSystem > m_renderSystem;
	resource::Proxy< render::Shader > m_shader;
	Ref< const render::IVertexLayout > m_vertexLayout;
	RefArray< render::Buffer > m_freeVertexBuffers;
	Semaphore m_lock;

	// Frame
	AlignedVector< Frame > m_frames;
	Frame* m_currentFrame = nullptr;

	// Assembly state
	AlignedVector< Matrix44 > m_view;
	AlignedVector< Matrix44 > m_world;
	Matrix44 m_worldView;
	Vertex* m_vertexHead = nullptr;
	Vertex* m_vertexTail = nullptr;

	void updateTransforms();

	Vertex* allocBatch(uint32_t vertexCount, render::ITexture* texture);
};

}
