/*
 * TRAKTOR
 * Copyright (c) 2025 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Scene/Editor/CameraPreviewControl.h"

#include "Core/Misc/SafeDestroy.h"
#include "Core/Serialization/DeepHash.h"
#include "Editor/IEditor.h"
#include "Render/Context/RenderContext.h"
#include "Render/Frame/RenderGraph.h"
#include "Render/Frame/RenderGraphTypes.h"
#include "Render/IRenderSystem.h"
#include "Render/IRenderTargetSet.h"
#include "Render/ITexture.h"
#include "Render/PrimitiveRenderer.h"
#include "Resource/IResourceManager.h"
#include "Scene/Editor/EntityAdapter.h"
#include "Scene/Editor/ISceneEditorProfile.h"
#include "Scene/Editor/SceneEditorContext.h"
#include "Scene/Scene.h"
#include "World/Entity/CameraComponent.h"
#include "World/IWorldRenderer.h"
#include "World/WorldEntityRenderers.h"
#include "World/WorldRenderSettings.h"

namespace traktor::scene
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.scene.CameraPreviewControl", CameraPreviewControl, Object)

CameraPreviewControl::CameraPreviewControl()
:	m_worldRendererType(nullptr)
,	m_worldRendererHash(0)
{
}

bool CameraPreviewControl::create(SceneEditorContext* context, const TypeInfo& worldRendererType)
{
	m_context = context;
	m_worldRendererType = &worldRendererType;

	// Create render target for preview (200x150 pixels)
	render::RenderTargetSetCreateDesc rtDesc;
	rtDesc.width = 200;
	rtDesc.height = 150;
	rtDesc.createDepthStencil = true;
	rtDesc.targets[0].format = render::TfR8G8B8A8;

	m_renderTarget = m_context->getRenderSystem()->createRenderTargetSet(rtDesc, nullptr, L"CameraPreview");
	if (!m_renderTarget)
		return false;

	m_renderContext = new render::RenderContext(4 * 1024 * 1024);
	m_renderGraph = new render::RenderGraph(m_context->getRenderSystem(), 0);

	m_timer.reset();
	return true;
}

void CameraPreviewControl::destroy()
{
	safeDestroy(m_worldRenderer);
	safeDestroy(m_renderGraph);
	m_renderContext = nullptr;
	m_renderTarget = nullptr;
}

void CameraPreviewControl::render(render::PrimitiveRenderer* primitiveRenderer, const ui::Size& viewportSize, const Matrix44& mainProjection)
{
	EntityAdapter* cameraEntity = getSelectedCameraEntity();
	if (!cameraEntity)
		return;

	Ref<scene::Scene> sceneInstance = m_context->getScene();
	if (!sceneInstance)
		return;

	world::CameraComponent* cameraComponent = cameraEntity->getComponent<world::CameraComponent>();
	if (!cameraComponent)
		return;

	// Lazy create world renderer
	if (!m_worldRenderer)
	{
		updateWorldRenderer();
		if (!m_worldRenderer)
			return;
	}

	// Get the camera's transform to use as view matrix
	const Matrix44 cameraView = cameraEntity->getTransform().inverse().toMatrix44();
	
	// Setup world render view for camera preview
	const world::WorldRenderSettings* worldRenderSettings = sceneInstance->getWorldRenderSettings();
	const double deltaTime = m_timer.getDeltaTime();
	const double scaledTime = m_context->getTime();
	
	if (cameraComponent->getProjection() == world::Projection::Orthographic)
		m_worldRenderView.setOrthogonal(
			cameraComponent->getWidth(),
			cameraComponent->getHeight(),
			worldRenderSettings->viewNearZ,
			worldRenderSettings->viewFarZ);
	else // Projection::Perspective
		m_worldRenderView.setPerspective(
			200.0f,  // width
			150.0f,  // height
			200.0f / 150.0f,  // aspect
			cameraComponent->getFieldOfView(),
			worldRenderSettings->viewNearZ,
			worldRenderSettings->viewFarZ);

	m_worldRenderView.setTimes(scaledTime, deltaTime, 1.0f);
	m_worldRenderView.setView(m_worldRenderView.getView(), cameraView);

	// Use render context to clear and render to our render target
	m_renderContext->flush();
	
	// Set the render target and clear it
	m_renderContext->setRenderTargetSet(m_renderTarget);
	m_renderContext->clear(render::CfColor | render::CfDepth, Color4f(0.2f, 0.4f, 0.8f, 1.0f), 1.0f);
	
	// TODO: Implement actual world rendering to the target
	// For now, just ensure we have something in the render target
	
	// Restore default render target
	m_renderContext->setRenderTargetSet(nullptr);
	
	// Calculate preview window dimensions and position
	const float previewWidth = 200.0f;
	const float previewHeight = 150.0f;
	const float margin = 10.0f;

	// Calculate position in bottom right corner
	const float vpWidth = float(viewportSize.cx);
	const float vpHeight = float(viewportSize.cy);
	const float left = vpWidth - previewWidth - margin;
	const float top = vpHeight - previewHeight - margin;
	const float right = vpWidth - margin;
	const float bottom = vpHeight - margin;

	// Convert to normalized device coordinates (-1 to 1)
	const float ndcLeft = (left / vpWidth) * 2.0f - 1.0f;
	const float ndcRight = (right / vpWidth) * 2.0f - 1.0f;
	const float ndcTop = 1.0f - (top / vpHeight) * 2.0f;
	const float ndcBottom = 1.0f - (bottom / vpHeight) * 2.0f;

	// Set orthographic projection for screen space rendering
	primitiveRenderer->setProjection(orthoLh(-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f));
	primitiveRenderer->pushView(Matrix44::identity());
	primitiveRenderer->pushDepthState(false, false, false);

	// Draw the rendered camera view texture
	if (m_renderTarget && m_renderTarget->getColorTexture(0))
	{
		primitiveRenderer->drawTextureQuad(
			Vector4(ndcLeft, ndcTop, 0.0f, 1.0f),
			Vector2(0.0f, 0.0f),
			Vector4(ndcRight, ndcTop, 0.0f, 1.0f),
			Vector2(1.0f, 0.0f),
			Vector4(ndcRight, ndcBottom, 0.0f, 1.0f),
			Vector2(1.0f, 1.0f),
			Vector4(ndcLeft, ndcBottom, 0.0f, 1.0f),
			Vector2(0.0f, 1.0f),
			Color4ub(255, 255, 255, 255),
			m_renderTarget->getColorTexture(0));
	}
	else
	{
		// Fallback: Draw solid color if texture not available
		primitiveRenderer->drawSolidQuad(
			Vector4(ndcLeft, ndcTop, 0.0f, 1.0f),
			Vector4(ndcRight, ndcTop, 0.0f, 1.0f),
			Vector4(ndcRight, ndcBottom, 0.0f, 1.0f),
			Vector4(ndcLeft, ndcBottom, 0.0f, 1.0f),
			Color4ub(64, 64, 128, 255));
	}
	
	primitiveRenderer->drawWireQuad(
		Vector4(ndcLeft, ndcTop, 0.0f, 1.0f),
		Vector4(ndcRight, ndcTop, 0.0f, 1.0f),
		Vector4(ndcRight, ndcBottom, 0.0f, 1.0f),
		Vector4(ndcLeft, ndcBottom, 0.0f, 1.0f),
		Color4ub(255, 255, 255, 255));

	// Draw camera name label
	const std::wstring cameraName = cameraEntity->getName();
	// Note: Text rendering would require additional support
	
	primitiveRenderer->popDepthState();
	primitiveRenderer->popView();
	
	// Restore the main viewport's projection matrix
	primitiveRenderer->setProjection(mainProjection);
}


void CameraPreviewControl::updateWorldRenderer()
{
	safeDestroy(m_worldRenderer);

	Ref<scene::Scene> sceneInstance = m_context->getScene();
	if (!sceneInstance)
		return;

	// Create entity renderers
	Ref<world::WorldEntityRenderers> worldEntityRenderers = new world::WorldEntityRenderers();
	for (auto profile : m_context->getEditorProfiles())
	{
		RefArray<world::IEntityRenderer> entityRenderers;
		profile->createEntityRenderers(m_context, nullptr, nullptr, *m_worldRendererType, entityRenderers);
		for (auto entityRenderer : entityRenderers)
			worldEntityRenderers->add(entityRenderer);
	}

	Ref<world::IWorldRenderer> worldRenderer = dynamic_type_cast<world::IWorldRenderer*>(m_worldRendererType->createInstance());
	if (!worldRenderer)
		return;

	const world::WorldRenderSettings* worldRenderSettings = sceneInstance->getWorldRenderSettings();
	
	world::WorldCreateDesc wcd;
	wcd.worldRenderSettings = worldRenderSettings;
	wcd.entityRenderers = worldEntityRenderers;
	wcd.quality.motionBlur = world::Quality::Disabled;
	wcd.quality.reflections = world::Quality::Disabled;
	wcd.quality.shadows = world::Quality::Low;  // Low quality for preview
	wcd.quality.ambientOcclusion = world::Quality::Disabled;
	wcd.quality.antiAlias = world::Quality::Disabled;
	wcd.quality.imageProcess = world::Quality::Disabled;
	wcd.multiSample = 0;
	wcd.hdr = false;  // Keep it simple for preview

	if (!worldRenderer->create(
		m_context->getResourceManager(),
		m_context->getRenderSystem(),
		wcd))
		return;

	m_worldRenderer = worldRenderer;
	m_worldRendererHash = DeepHash(sceneInstance->getWorldRenderSettings()).get();
}

EntityAdapter* CameraPreviewControl::getSelectedCameraEntity()
{
	// Find all camera entities
	if (!m_context->findAdaptersOfType(type_of<world::CameraComponent>(), m_cameraEntities))
		return nullptr;

	// Look for a selected camera entity
	for (auto cameraEntity : m_cameraEntities)
	{
		if (cameraEntity->isSelected())
			return cameraEntity;
	}

	return nullptr;
}

}