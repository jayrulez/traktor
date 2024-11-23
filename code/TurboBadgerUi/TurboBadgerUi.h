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

#include "TurboBadgerUi/Backend/TBRenderer.h"

#include "Core/Object.h"
#include "Core/Singleton/ISingleton.h"

 // import/export mechanism.
#undef T_DLLCLASS
#if defined(T_TURBOBADGERUI_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::render
{
	class IRenderSystem;
}

namespace traktor::turbobadgerui
{
	class TurboBadgerUiView;

	class T_DLLCLASS TurboBadgerUi
		: public Object
		, public ISingleton
	{
		T_RTTI_CLASS;

	public:
		TurboBadgerUi() = default;

		static TurboBadgerUi& getInstance();

		void destroy() override;

		bool initialize(render::IRenderSystem* renderSystem);
		
		void shutdown();

		bool isInitialized() const;

		TurboBadgerUiView* createView();

		void destroyView(TurboBadgerUiView* view);

		void update();

		void updateView(TurboBadgerUiView* view);

		void reloadRenderResources();

		void renderView(TurboBadgerUiView* view, uint32_t width, uint32_t height, AlignedVector<TurboBadgerUiBatch>& batches);

	private:
		struct BackendData
		{
			BackendData(render::IRenderSystem* renderSystem);

			TBRenderer renderer;
		};

		BackendData* m_backendData = nullptr;
		bool m_initialized = false;
		AlignedVector<TurboBadgerUiView*> m_views;
	};
}