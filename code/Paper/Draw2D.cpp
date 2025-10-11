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
const resource::Id< render::Shader > c_idDraw2DShader(Guid(L"{669BA9CB-B9A7-BB4E-A775-1CA7904163CF}"));

static const render::Handle s_handleProjection(L"Projection");
static const render::Handle s_handleTexture(L"Texture");

const int32_t c_maxVertices = 4096;

#pragma pack(1)
struct Vertex
{
	float position[4];
	half_t texCoord[2];
	float color[4];

	void set(const Vector4& pos, const Vector2& uv, const Color4f& col)
	{
		pos.storeUnaligned(position);
		texCoord[0] = floatToHalf(uv.x);
		texCoord[1] = floatToHalf(uv.y);
		col.storeUnaligned(color);
	}
};
#pragma pack()

}

T_IMPLEMENT_RTTI_CLASS(L"traktor.paper.Draw2D", Draw2D, Object)

bool Draw2D::create(
	resource::IResourceManager* resourceManager,
	render::IRenderSystem* renderSystem
)
{
	m_renderSystem = renderSystem;

	// Load shader
	if (!resourceManager->bind(c_idDraw2DShader, m_shader))
		return false;

	// Create vertex layout
	AlignedVector< render::VertexElement > vertexElements;
	vertexElements.push_back(render::VertexElement(render::DataUsage::Position, render::DtFloat4, offsetof(Vertex, position), 0));
	vertexElements.push_back(render::VertexElement(render::DataUsage::Custom, render::DtHalf2, offsetof(Vertex, texCoord), 0));
	vertexElements.push_back(render::VertexElement(render::DataUsage::Color, render::DtFloat4, offsetof(Vertex, color), 0));

	m_vertexLayout = m_renderSystem->createVertexLayout(vertexElements);
	if (!m_vertexLayout)
		return false;

	// Create vertex buffer
	m_vertexBuffer = m_renderSystem->createBuffer(render::BuVertex, c_maxVertices * sizeof(Vertex), true);
	if (!m_vertexBuffer)
		return false;

	return true;
}

void Draw2D::destroy()
{
	if (m_vertexBuffer)
	{
		m_vertexBuffer->destroy();
		m_vertexBuffer = nullptr;
	}
	m_vertexLayout = nullptr;
	m_renderSystem = nullptr;
}

void Draw2D::begin(const Matrix44& projection)
{
	m_projection = projection;
	m_vertexCount = 0;
}

void Draw2D::end()
{
	if (m_vertexCount > 0)
		m_vertexBuffer->unlock();
}

void Draw2D::render(render::IRenderView* renderView)
{
	if (m_vertexCount == 0)
		return;

	render::Shader::Permutation perm;
	render::IProgram* program = m_shader->getProgram(perm).program;
	if (!program)
		return;

	program->setMatrixParameter(s_handleProjection, m_projection);
	program->setTextureParameter(s_handleTexture, nullptr); // Will be set per-draw

	renderView->draw(
		m_vertexBuffer->getBufferView(),
		m_vertexLayout,
		nullptr,
		render::IndexType::Void,
		program,
		render::Primitives::setNonIndexed(render::PrimitiveType::Triangles, 0, m_vertexCount / 3),
		1
	);
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
	if (m_vertexCount + 6 > c_maxVertices)
		return;

	// Lock vertex buffer if first draw
	Vertex* vertices = nullptr;
	if (m_vertexCount == 0)
	{
		vertices = static_cast< Vertex* >(m_vertexBuffer->lock());
		if (!vertices)
			return;
	}
	else
	{
		vertices = static_cast< Vertex* >(m_vertexBuffer->lock());
		if (!vertices)
			return;
		vertices += m_vertexCount;
	}

	const float x0 = position.x;
	const float y0 = position.y;
	const float x1 = position.x + size.x;
	const float y1 = position.y + size.y;

	// First triangle
	vertices[0].set(Vector4(x0, y0, 0.0f, 1.0f), Vector2(uvMin.x, uvMin.y), color);
	vertices[1].set(Vector4(x1, y0, 0.0f, 1.0f), Vector2(uvMax.x, uvMin.y), color);
	vertices[2].set(Vector4(x1, y1, 0.0f, 1.0f), Vector2(uvMax.x, uvMax.y), color);

	// Second triangle
	vertices[3].set(Vector4(x0, y0, 0.0f, 1.0f), Vector2(uvMin.x, uvMin.y), color);
	vertices[4].set(Vector4(x1, y1, 0.0f, 1.0f), Vector2(uvMax.x, uvMax.y), color);
	vertices[5].set(Vector4(x0, y1, 0.0f, 1.0f), Vector2(uvMin.x, uvMax.y), color);

	m_vertexCount += 6;
}

}
