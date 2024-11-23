/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "TurboBadgerUi/Runtime/Types.h"

#include "Resource/Proxy.h"
#include "Runtime/Engine/Layer.h"

 // import/export mechanism.
#undef T_DLLCLASS
#if defined(T_TURBOBADGERUI_RUNTIME_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::runtime
{
	class IEnvironment;
}

namespace traktor::turbobadgerui
{
	class TurboBadgerUiViewResource;

	/*! Stage TurboBadgerUi layer.
	 * \ingroup TurboBadgerUi
	 */
	class T_DLLCLASS TurboBadgerUiLayer : public runtime::Layer
	{
		T_RTTI_CLASS;

	public:
		explicit TurboBadgerUiLayer(
			runtime::Stage* stage,
			const std::wstring& name,
			bool permitTransition,
			runtime::IEnvironment* environment,
			const resource::Proxy< TurboBadgerUiViewResource >& uiViewResource
		);

		virtual void destroy() override;

		virtual void transition(Layer* fromLayer) override final;

		virtual void preUpdate(const runtime::UpdateInfo& info) override final;

		virtual void update(const runtime::UpdateInfo& info) override final;

		virtual void postUpdate(const runtime::UpdateInfo& info) override final;

		virtual void preSetup(const runtime::UpdateInfo& info) override final;

		virtual void setup(const runtime::UpdateInfo& info, render::RenderGraph& renderGraph) override final;

		virtual void preReconfigured() override final;

		virtual void postReconfigured() override final;

		virtual void suspend() override final;

		virtual void resume() override final;

		virtual void hotReload() override final;

	private:

		Ref< runtime::IEnvironment > m_environment;
		resource::Proxy< TurboBadgerUiViewResource > m_uiViewResource;
	};
}