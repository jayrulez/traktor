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
#include "Ui/Size.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SCENE_EDITOR_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::render
{
class PrimitiveRenderer;
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

	bool create(SceneEditorContext* context);

	void destroy();

	/*! Update and render the camera preview if a camera is selected */
	void render(render::PrimitiveRenderer* primitiveRenderer, const ui::Size& viewportSize);

private:
	SceneEditorContext* m_context;
	RefArray<EntityAdapter> m_cameraEntities;

	EntityAdapter* getSelectedCameraEntity();
};

}