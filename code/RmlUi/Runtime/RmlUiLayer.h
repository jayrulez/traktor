/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Runtime/Engine/Layer.h"
#include "Core/Class/Any.h"
#include "Core/Containers/SmallMap.h"
#include "Core/Math/Vector2.h"
#include "Resource/Id.h"
#include "Resource/Proxy.h"

 // import/export mechanism.
#undef T_DLLCLASS
#if defined(T_RMLUI_RUNTIME_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::runtime
{

	class IEnvironment;

}

namespace traktor::render
{

	class IRenderTargetSet;

}

namespace traktor::rmlui
{
	class RmlDocument;

	/*! Stage RmlUi layer.
	 * \ingroup RmlUi
	 */
	class T_DLLCLASS RmlUiLayer : public runtime::Layer
	{
		T_RTTI_CLASS;

	public:
		explicit RmlUiLayer(
			runtime::Stage* stage,
			const std::wstring& name,
			bool permitTransition,
			runtime::IEnvironment* environment,
			const resource::Proxy< RmlDocument >& rmlDocument,
			bool clearBackground
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

		/*! Set if RmlUi should be rendererd.
		 *
		 * \param visible True if RmlUi should be rendered.
		 */
		void setVisible(bool visible) { m_visible = visible; }

		/*! Check if RmlUi is being rendered.
		 *
		 * \return True if RmlUi is being rendered.
		 */
		bool isVisible() const { return m_visible; }

	private:
		struct LastMouseState
		{
			int32_t button = 0;
			int32_t wheel = 0;
		};

		Ref< runtime::IEnvironment > m_environment;
		resource::Proxy< RmlDocument > m_rmlDocument;
		bool m_clearBackground;
		bool m_visible;
		Vector2 m_offset;
		float m_scale;
		bool m_lastUpValue;
		bool m_lastDownValue;
		bool m_lastConfirmValue;
		bool m_lastEscapeValue;
		LastMouseState m_lastMouse[8];
		int32_t m_lastMouseX;
		int32_t m_lastMouseY;
	};

}