/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "TurboBadgerUi/Editor/Types.h"

#include "Ui/Widget.h"

 // import/export mechanism.
#undef T_DLLCLASS
#if defined(T_TURBOBADGERUI_EDITOR_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor
{
	namespace db
	{

		class Database;

	}

	namespace editor
	{

		class IEditor;

	}

	namespace resource
	{

		class IResourceManager;

	}

	namespace render
	{

		class IRenderSystem;
		class IRenderTargetSet;
		class IRenderView;
		class RenderContext;
		class RenderGraph;
		class Shader;

	}

	namespace ui
	{
		class Edit;
		class Button;
	}

	namespace turbobadgerui
	{
		class TurboBadgerUiViewResource;
		class TurboBadgerUiRenderer;
		class TurboBadgerUiView;

		class T_DLLCLASS TurboBadgerUiPreviewControl : public ui::Widget
		{
			T_RTTI_CLASS;

		public:
			explicit TurboBadgerUiPreviewControl(editor::IEditor* editor,
				db::Database* database,
				resource::IResourceManager* resourceManager,
				render::IRenderSystem* renderSystem);

			bool create(ui::Widget* parent, TurboBadgerUiViewResource* uiViewResource);

			virtual void destroy() override final;

			virtual ui::Size getPreferredSize(const ui::Size& hint) const override final;

			void setUiViewResource(TurboBadgerUiViewResource* uiViewResource);

		private:
			editor::IEditor* m_editor;
			Ref< ui::EventSubject::IEventHandler > m_idleEventHandler;
			Ref< db::Database > m_database;
			Ref< resource::IResourceManager > m_resourceManager;
			Ref< render::IRenderSystem > m_renderSystem;
			Ref< render::IRenderView > m_renderView;
			Ref< render::RenderContext > m_renderContext;
			Ref< render::RenderGraph > m_renderGraph;
			Ref< turbobadgerui::TurboBadgerUiRenderer > m_renderer;
			Ref< TurboBadgerUiViewResource > m_uiViewResource;
			TurboBadgerUiView* m_view = nullptr;

			void eventSize(ui::SizeEvent* event);

			void eventPaint(ui::PaintEvent* event);

			void eventIdle(ui::IdleEvent* event);

			void eventKey(ui::KeyEvent* event);

			void eventKeyDown(ui::KeyDownEvent* event);

			void eventKeyUp(ui::KeyUpEvent* event);

			void eventButtonDown(ui::MouseButtonDownEvent* event);

			void eventButtonUp(ui::MouseButtonUpEvent* event);

			void eventMouseMove(ui::MouseMoveEvent* event);

			void eventMouseWheel(ui::MouseWheelEvent* event);
		};

	}
}