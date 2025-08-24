/*
 * TRAKTOR
 * Copyright (c) 2025 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Scene/Editor/CameraPreviewControl.h"

#include "Render/PrimitiveRenderer.h"
#include "Scene/Editor/EntityAdapter.h"
#include "Scene/Editor/SceneEditorContext.h"
#include "World/Entity/CameraComponent.h"

namespace traktor::scene
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.scene.CameraPreviewControl", CameraPreviewControl, Object)

CameraPreviewControl::CameraPreviewControl()
{
}

bool CameraPreviewControl::create(SceneEditorContext* context)
{
	m_context = context;
	return true;
}

void CameraPreviewControl::destroy()
{
}

void CameraPreviewControl::render(render::PrimitiveRenderer* primitiveRenderer, const ui::Size& viewportSize)
{
	EntityAdapter* cameraEntity = getSelectedCameraEntity();
	if (!cameraEntity)
		return;

	world::CameraComponent* cameraComponent = cameraEntity->getComponent<world::CameraComponent>();
	if (!cameraComponent)
		return;

	// Preview dimensions and position
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

	// Draw semi-transparent background
	primitiveRenderer->drawSolidQuad(
		Vector4(ndcLeft, ndcTop, 0.0f, 1.0f),
		Vector4(ndcRight, ndcTop, 0.0f, 1.0f),
		Vector4(ndcRight, ndcBottom, 0.0f, 1.0f),
		Vector4(ndcLeft, ndcBottom, 0.0f, 1.0f),
		Color4ub(32, 32, 32, 192));

	// Draw border around preview
	primitiveRenderer->drawWireQuad(
		Vector4(ndcLeft, ndcTop, 0.0f, 1.0f),
		Vector4(ndcRight, ndcTop, 0.0f, 1.0f),
		Vector4(ndcRight, ndcBottom, 0.0f, 1.0f),
		Vector4(ndcLeft, ndcBottom, 0.0f, 1.0f),
		Color4ub(255, 255, 255, 255));

	// Draw camera icon/text in the center as placeholder
	const std::wstring cameraName = cameraEntity->getName();
	// Note: For now just shows a bordered box. Future enhancement would render the actual camera view

	primitiveRenderer->popDepthState();
	primitiveRenderer->popView();
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