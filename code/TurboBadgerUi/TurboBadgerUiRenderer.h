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

#include "Core/Object.h"
#include "Core/Thread/Mutex.h"

#include "Render/Types.h"
#include "Render/ITexture.h"

 // import/export mechanism.
#undef T_DLLCLASS
#if defined(T_TURBOBADGERUI_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::resource
{
	class IResourceManager;
}

namespace traktor::render
{
	class IRenderSystem;
	class RenderGraph;
	class RenderPass;
	class IVertexLayout;
}

namespace traktor::turbobadgerui
{    
	class TurboBadgerUiRendererResources;
	class TurboBadgerUiView;

	/*! TurboBadgerUi renderer.
	 * \ingroup TurboBadgerUi
	 *
	 * This TurboBadgerUi renderer uses the render system
	 * in order to accelerate rendering.
	 */
	class T_DLLCLASS TurboBadgerUiRenderer : public Object
	{
		T_RTTI_CLASS;

	public:
		TurboBadgerUiRenderer();

		virtual ~TurboBadgerUiRenderer();

		bool create(
			resource::IResourceManager* resourceManager,
			render::IRenderSystem* renderSystem,
			uint32_t frameCount
		);

		void destroy();

		void beginSetup(render::RenderGraph* renderGraph);

		void endSetup();

		void renderView(TurboBadgerUiView* view, uint32_t width, uint32_t height);
		
	private:
		Mutex m_renderMutex;
		resource::IResourceManager* m_resourceManager = nullptr;
		render::IRenderSystem* m_renderSystem = nullptr;
		Ref< render::RenderGraph > m_renderGraph = nullptr;
		Ref< render::RenderPass > m_renderPassOutput = nullptr;
		Ref< TurboBadgerUiRendererResources > m_rendererResources = nullptr;
		Ref< const render::IVertexLayout > m_vertexLayout = nullptr;
	};
}