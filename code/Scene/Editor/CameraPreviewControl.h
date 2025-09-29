/*
 * TRAKTOR
 * Copyright (c) 2025 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/RefArray.h"
#include "Core/Object.h"
#include "Core/Math/Matrix44.h"
#include "Core/Timer/Timer.h"
#include "Ui/Size.h"
#include "World/WorldRenderView.h"
#include "Render/Frame/RenderGraphTypes.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SCENE_EDITOR_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::render
{
class IRenderTargetSet;
class ITexture;
class PrimitiveRenderer;
class RenderContext;
class RenderGraph;
}

namespace traktor::world
{
class IWorldRenderer;
}

namespace traktor::scene
{

class EntityAdapter;
class SceneEditorContext;

/*! Camera preview control.
 * 
 * Renders a small preview window in the bottom right of the scene editor
 * showing the view from any selected camera entity.
 * \ingroup Scene
 */
class T_DLLCLASS CameraPreviewControl : public Object
{
	T_RTTI_CLASS;

public:
	CameraPreviewControl();

	bool create(SceneEditorContext* context, const TypeInfo& worldRendererType);

	void destroy();

	/*! Update and render the camera preview if a camera is selected 
	 * @param primitiveRenderer The primitive renderer to use
	 * @param viewportSize The size of the main viewport
	 * @param mainProjection The main viewport's projection matrix to restore after rendering
	 */
	void render(render::PrimitiveRenderer* primitiveRenderer, const ui::Size& viewportSize, const Matrix44& mainProjection);

private:
	SceneEditorContext* m_context;
	const TypeInfo* m_worldRendererType;
	Ref<render::IRenderTargetSet> m_renderTarget;
	Ref<render::RenderContext> m_renderContext;
	Ref<render::RenderGraph> m_renderGraph;
	Ref<world::IWorldRenderer> m_worldRenderer;
	world::WorldRenderView m_worldRenderView;
	uint32_t m_worldRendererHash;
	RefArray<EntityAdapter> m_cameraEntities;
	Timer m_timer;

	void updateWorldRenderer();
	EntityAdapter* getSelectedCameraEntity();
};

}