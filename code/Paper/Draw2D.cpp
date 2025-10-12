/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Misc/String.h"
#include "Core/Math/Half.h"
#include "Core/Thread/Acquire.h"
#include "Paper/Draw2D.h"
#include "Render/Buffer.h"
#include "Render/IProgram.h"
#include "Render/IRenderSystem.h"
#include "Render/IRenderView.h"
#include "Render/ITexture.h"
#include "Render/Shader.h"
#include "Render/Types.h"
#include "Render/VertexElement.h"
#include "Resource/IResourceManager.h"
#include "Resource/Member.h"

namespace traktor::paper
{
namespace
{

// Shader GUID
const resource::Id< render::Shader > c_idDraw2DShader(Guid(L"{A704DB1C-60E6-9D44-AD1D-F7822568242D}"));

static const render::Handle s_handleProjection(L"Paper_Projection");
static const render::Handle s_handleTexture(L"Paper_Texture");

const int32_t c_bufferCount = 64 * 1024;

}

T_IMPLEMENT_RTTI_CLASS(L"traktor.paper.Draw2D", Draw2D, Object)

void Draw2D::Vertex::set(const Vector4& pos, const Vector2& uv, const Color4f& col)
{
	pos.storeUnaligned(position);
	texCoord[0] = floatToHalf(uv.x);
	texCoord[1] = floatToHalf(uv.y);
	col.storeUnaligned(color);
}

bool Draw2D::create(
	resource::IResourceManager* resourceManager,
	render::IRenderSystem* renderSystem,
	uint32_t frameCount
)
{
	m_renderSystem = renderSystem;

	// Load shader
	if (!resourceManager->bind(c_idDraw2DShader, m_shader))
		return false;

	// Initialize transform stacks
	m_view.push_back(Matrix44::identity());
	m_world.push_back(Matrix44::identity());

	// Create frames
	m_frames.resize(frameCount);

	// Create vertex layout
	AlignedVector< render::VertexElement > vertexElements;
	vertexElements.push_back(render::VertexElement(render::DataUsage::Position, render::DtFloat4, offsetof(Vertex, position), 0));
	vertexElements.push_back(render::VertexElement(render::DataUsage::Custom, render::DtHalf2, offsetof(Vertex, texCoord), 0));
	vertexElements.push_back(render::VertexElement(render::DataUsage::Color, render::DtFloat4, offsetof(Vertex, color), 0));

	m_vertexLayout = m_renderSystem->createVertexLayout(vertexElements);
	if (!m_vertexLayout)
		return false;

	updateTransforms();
	return true;
}

void Draw2D::destroy()
{
	for (auto vertexBuffer : m_freeVertexBuffers)
		vertexBuffer->destroy();
	m_freeVertexBuffers.clear();

	for (auto& frame : m_frames)
	{
		for (auto vertexBuffer : frame.vertexBuffers)
			vertexBuffer->destroy();
	}
	m_frames.clear();
}

bool Draw2D::begin(uint32_t frame, const Matrix44& projection)
{
	m_currentFrame = &m_frames[frame];
	m_currentFrame->vertexBuffers.resize(0);
	m_currentFrame->projections.resize(0);
	m_currentFrame->projections.push_back(projection);
	m_currentFrame->batches.resize(0);

	m_view.resize(1);
	m_view[0] = Matrix44::identity();
	m_world.resize(1);
	m_world[0] = Matrix44::identity();

	updateTransforms();
	return true;
}

void Draw2D::end(uint32_t frame)
{
	T_ASSERT(m_currentFrame == &m_frames[frame]);

	if (m_vertexHead)
	{
		m_currentFrame->vertexBuffers.back()->unlock();
		m_vertexHead =
		m_vertexTail = nullptr;
	}

	m_currentFrame = nullptr;
}

void Draw2D::render(render::IRenderView* renderView, uint32_t frame)
{
	Frame& f = m_frames[frame];

	for (const auto& batch : f.batches)
	{
		render::Shader::Permutation perm;
		render::IProgram* program = m_shader->getProgram(perm).program;
		if (!program)
			continue;

		program->setMatrixParameter(s_handleProjection, f.projections[batch.projection]);

		if (batch.texture)
			program->setTextureParameter(s_handleTexture, batch.texture);

		renderView->draw(
			batch.vertexBuffer->getBufferView(),
			m_vertexLayout,
			nullptr,
			render::IndexType::Void,
			program,
			batch.primitives,
			1
		);
	}

	// Return frame's vertex buffers into free list.
	{
		T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);
		m_freeVertexBuffers.insert(m_freeVertexBuffers.end(), f.vertexBuffers.begin(), f.vertexBuffers.end());
	}

	// Reset frame.
	f.vertexBuffers.resize(0);
	f.projections.resize(0);
	f.batches.resize(0);
}

void Draw2D::pushView(const Matrix44& view)
{
	m_view.push_back(view);
	updateTransforms();
}

void Draw2D::popView()
{
	m_view.pop_back();
	T_ASSERT(!m_view.empty());
	updateTransforms();
}

void Draw2D::pushWorld(const Matrix44& transform)
{
	m_world.push_back(transform);
	updateTransforms();
}

void Draw2D::popWorld()
{
	m_world.pop_back();
	T_ASSERT(!m_world.empty());
	updateTransforms();
}

void Draw2D::setProjection(const Matrix44& projection)
{
	m_currentFrame->projections.push_back(projection);
}

void Draw2D::drawTexturedQuad(
	const Vector2& position,
	const Vector2& size,
	const Vector2& uvMin,
	const Vector2& uvMax,
	const Color4f& color,
	render::ITexture* texture
)
{
	Vertex* vertices = allocBatch(6, texture);
	if (!vertices)
		return;

	const float x0 = position.x;
	const float y0 = position.y;
	const float x1 = position.x + size.x;
	const float y1 = position.y + size.y;

	// Transform vertices by world-view matrix
	const Vector4 v0 = m_worldView * Vector4(x0, y0, 0.0f, 1.0f);
	const Vector4 v1 = m_worldView * Vector4(x1, y0, 0.0f, 1.0f);
	const Vector4 v2 = m_worldView * Vector4(x1, y1, 0.0f, 1.0f);
	const Vector4 v3 = m_worldView * Vector4(x0, y1, 0.0f, 1.0f);

	// First triangle
	vertices[0].set(v0, Vector2(uvMin.x, uvMin.y), color);
	vertices[1].set(v1, Vector2(uvMax.x, uvMin.y), color);
	vertices[2].set(v2, Vector2(uvMax.x, uvMax.y), color);

	// Second triangle
	vertices[3].set(v0, Vector2(uvMin.x, uvMin.y), color);
	vertices[4].set(v2, Vector2(uvMax.x, uvMax.y), color);
	vertices[5].set(v3, Vector2(uvMin.x, uvMax.y), color);
}

void Draw2D::updateTransforms()
{
	m_worldView = m_view.back() * m_world.back();
}

Draw2D::Vertex* Draw2D::allocBatch(uint32_t vertexCount, render::ITexture* texture)
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

	if (!vertexCount)
		return nullptr;

	// Check if enough room is available in current vertex buffer.
	if (m_vertexHead)
	{
		const uint32_t vertexCountAvail = c_bufferCount - uint32_t(m_vertexTail - m_vertexHead);
		if (vertexCount > vertexCountAvail)
		{
			// Not enough room, finish off current vertex buffer.
			m_currentFrame->vertexBuffers.back()->unlock();
			m_vertexHead =
			m_vertexTail = nullptr;
		}
	}

	if (!m_vertexHead)
	{
		if (m_freeVertexBuffers.empty())
		{
			// No free buffers; need to allocate a new buffer.
			Ref< render::Buffer > vertexBuffer = m_renderSystem->createBuffer(render::BuVertex, c_bufferCount * sizeof(Vertex), true);
			if (!vertexBuffer)
				return nullptr;

			m_freeVertexBuffers.push_back(vertexBuffer);
		}
		T_ASSERT(!m_freeVertexBuffers.empty());

		// Pick buffer from free list.
		Ref< render::Buffer > vertexBuffer = m_freeVertexBuffers.front();
		m_freeVertexBuffers.pop_front();

		// Lock new buffer.
		m_vertexHead =
		m_vertexTail = (Vertex*)vertexBuffer->lock();
		if (!m_vertexHead)
			return nullptr;

		// Place buffer last in frame's used list.
		m_currentFrame->vertexBuffers.push_back(vertexBuffer);
	}

	// Create new batch if necessary.
	AlignedVector< Batch >& batches = m_currentFrame->batches;
	const uint32_t projection = (uint32_t)m_currentFrame->projections.size() - 1;
	if (
		batches.empty() ||
		batches.back().projection != projection ||
		batches.back().vertexBuffer != m_currentFrame->vertexBuffers.back() ||
		batches.back().texture != texture
	)
	{
		Batch batch;
		batch.projection = projection;
		batch.vertexBuffer = m_currentFrame->vertexBuffers.back();
		batch.texture = texture;
		batch.primitives = render::Primitives::setNonIndexed(render::PrimitiveType::Triangles, (uint32_t)(m_vertexTail - m_vertexHead), 0);
		batches.push_back(batch);
	}
	batches.back().primitives.count += vertexCount / 3;  // Increment by number of triangles

	// Increment vertex pointer, return where caller must write vertices.
	Vertex* vertexPtr = m_vertexTail;
	m_vertexTail += vertexCount;
	return vertexPtr;
}

}
