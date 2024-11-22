/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "TurboBadgerUi/Types.h"

#include "Core/Ref.h"

#include "tb_renderer.h"

namespace traktor::render
{
    class ITexture;
    class IRenderSystem;
}

namespace traktor::turbobadgerui
{
    class TBRenderer;

    class TBBitmap : public tb::TBBitmap
    {
    public:
        TBBitmap(TBRenderer* renderer, render::IRenderSystem* renderSystem);

        /** Note: Implementations for batched renderers should call TBRenderer::FlushBitmap
      to make sure any active batch is being flushed before the bitmap is deleted. */
        ~TBBitmap() override;

        bool Init(int width, int height, unsigned int* data);

        int Width() override;
        int Height() override;

        /** Update the bitmap with the given data (in BGRA32 format).
          Note: Implementations for batched renderers should call TBRenderer::FlushBitmap
          to make sure any active batch is being flushed before the bitmap is changed. */
        void SetData(unsigned int* data) override;

        inline Ref < render::ITexture > GetTexture() const
        {
            return m_texture;
        }

    private:
        TBRenderer* m_renderer;
        render::IRenderSystem* m_renderSystem;
        int m_width = 0;
        int m_height = 0;
        Ref < render::ITexture > m_texture;
    };
}