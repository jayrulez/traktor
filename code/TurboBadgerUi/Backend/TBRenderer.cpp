/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/Backend/TBRenderer.h"
#include "TurboBadgerUi/Backend/TBBitmap.h"
#include "TurboBadgerUi/Backend/TBVertex.h"

#include "Render/IRenderSystem.h"

namespace traktor::turbobadgerui
{
    TBRenderer::TBRenderer(render::IRenderSystem* renderSystem)
        : m_renderSystem(renderSystem)
    {
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

        TBBatch batch(texture);
        batch.clipRect = m_clipRect;

        for (int i = 0; i < tbBatch->vertex_count; i++)
        {
            tb::TBRendererBatcher::Vertex tbVertex = tbBatch->vertex[i];
            TBVertex vertex = {
                .position = {tbVertex.x, tbVertex.y},
                .texCoord = {tbVertex.u, tbVertex.v},
                //.color = {.v = tbVertex.col},
            };

            batch.vertices.push_back(vertex);
        }

        m_batches.push_back(batch);
    }

    void TBRenderer::SetClipRect(const tb::TBRect& rect)
    {
        m_clipRect = render::Rectangle(rect.x, rect.y, rect.w, rect.h);
    }

    void TBRenderer::BeginBatch()
    {
        m_batches.clear();
    }

    const AlignedVector<TBBatch>& TBRenderer::EndBatch()
    {
        return m_batches;
    }
}