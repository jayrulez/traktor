/*
 * TRAKTOR
 * Copyright (c) 2022-2023 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/RefArray.h"
#include "Core/Containers/SmallMap.h"
#include "Render/Types.h"
#include "RmlUi/Core/Context.h"

 // import/export mechanism.
#undef T_DLLCLASS
#if defined(T_RMLUI_EXPORT)
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
	class IRenderTargetSet;
	class RenderGraph;
	class RenderPass;

}

namespace traktor::rmlui
{
	class RmlUiResources;

	/*! RmlUi renderer.
	 * \ingroup RmlUi
	 *
	 * This RmlUi renderer uses the render system
	 * in order to accelerate rendering.
	 */
	class T_DLLCLASS RmlUiRenderer : public Object
	{
		T_RTTI_CLASS;

	public:
		RmlUiRenderer();

		virtual ~RmlUiRenderer();

		bool create(
			resource::IResourceManager* resourceManager,
			render::IRenderSystem* renderSystem,
			uint32_t frameCount,
			bool clearBackground
		);

		void destroy();

		void beginSetup(render::RenderGraph* renderGraph);

		void endSetup();

		void setClearBackground(bool clearBackground);

		// \name RmlUiRenderer
		// \{

		void render(Rml::Context* context, int32_t width, int32_t height);

		// \}

	private:
		resource::IResourceManager* m_resourceManager;
		render::IRenderSystem* m_renderSystem;
		Ref< render::RenderGraph > m_renderGraph;
		Ref< render::RenderPass > m_renderPassOutput;
		Ref< RmlUiResources > m_shapeResources;
		Ref< const render::IVertexLayout > m_vertexLayout;
		bool m_clearBackground;
	};

}