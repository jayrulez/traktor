/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "TurboBadgerUi/TurboBadgerUiRenderer.h"
#include "TurboBadgerUi/Backend/TBRenderer.h"
#include "TurboBadgerUi/Backend/TBBitmap.h"

#include "Core/Misc//SafeDestroy.h"
#include "Render/IRenderSystem.h"
#include "Render/Buffer.h"

namespace traktor::turbobadgerui
{
	TBRenderer::TBRenderer(render::IRenderSystem* renderSystem)
		: m_renderSystem(renderSystem)
	{
	}

	TBRenderer::~TBRenderer()
	{
		//FlushAllInternal();
	}

	tb::TBBitmap* TBRenderer::CreateBitmap(int width, int height, uint32_t* data)
	{
		TBBitmap* bitmap = new TBBitmap(this, m_renderSystem);
		if (!bitmap->Init(width, height, data))
		{
			delete bitmap;
			return nullptr;
		}

		return bitmap;
	}

	void TBRenderer::RenderBatch(Batch* tbBatch)
	{
		Ref <render::ITexture> texture = nullptr;
		TBBitmap* bitmap = (TBBitmap*)tbBatch->bitmap;
		if (bitmap)
			texture = bitmap->GetTexture();

		TurboBadgerUiBatch batch(texture);
		batch.clipRect = m_clipRect;

		AlignedVector<TurboBadgerUiVertex> vertices;
		for (int i = 0; i < tbBatch->vertex_count; i++)
		{
			tb::TBRendererBatcher::Vertex tbVertex = tbBatch->vertex[i];
			TurboBadgerUiVertex vertex = {
				.position = {tbVertex.x, tbVertex.y},
				.texCoord = {tbVertex.u, tbVertex.v},
				.color = {
					.r = tbVertex.r,
					.g = tbVertex.g,
					.b = tbVertex.b,
					.a = tbVertex.a,
				},
			};

			vertices.push_back(vertex);
		}

		uint32_t vertexBufferSize = (uint32_t)(sizeof(TurboBadgerUiVertex) * vertices.size());
		batch.vertexBuffer = m_renderSystem->createBuffer(render::BufferUsage::BuVertex, vertexBufferSize, true);
		std::memcpy(batch.vertexBuffer->lock(), vertices.c_ptr(), vertexBufferSize);
		batch.vertexBuffer->unlock();
		batch.triangleCount = (uint32_t)(vertices.size() / 3);

		m_batches.push_back(batch);
	}

	void TBRenderer::SetClipRect(const tb::TBRect& rect)
	{
		m_clipRect = render::Rectangle(rect.x, rect.y, rect.w, rect.h);
	}

	AlignedVector<TurboBadgerUiBatch> TBRenderer::renderView(TurboBadgerUiView* view, uint32_t width, uint32_t height)
	{
		m_clipRect = {0, 0, (int32_t)width, (int32_t)height};
		for (auto& batch : m_batches)
		{
			safeDestroy(batch.vertexBuffer);
		}
		m_batches.clear();

		BeginPaint(width, height);

		view->InvokePaint(tb::TBWidget::PaintProps());

		EndPaint();

		return m_batches;
	}
}