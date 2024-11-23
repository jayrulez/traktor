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
#include "TurboBadgerUi/TurboBadgerUiRenderer.h"
#include "TurboBadgerUi/TurboBadgerUiView.h"

#include "Core/Containers/AlignedVector.h"
#include "Render/Types.h"

#include "renderers/tb_renderer_batcher.h"

namespace traktor::render
{
	class IRenderSystem;
}

namespace traktor::turbobadgerui
{
	class TBRenderer : public tb::TBRendererBatcher
	{
	public:
		TBRenderer(render::IRenderSystem* renderSystem);
		~TBRenderer();

		tb::TBBitmap* CreateBitmap(int width, int height, uint32_t* data) override;
		void RenderBatch(Batch* batch) override;
		void SetClipRect(const tb::TBRect& rect) override;

	public:
		AlignedVector<TurboBadgerUiBatch> renderView(TurboBadgerUiView* view, uint32_t width, uint32_t height);

	private:
		render::IRenderSystem* m_renderSystem;
		render::Rectangle m_clipRect;
		AlignedVector<TurboBadgerUiBatch> m_batches;
	};
}