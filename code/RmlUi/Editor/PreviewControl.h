/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Timer/Timer.h"
#include "Ui/Widget.h"
#include "RmlUi/Core.h"

 // import/export mechanism.
#undef T_DLLCLASS
#if defined(T_RMLUI_EDITOR_EXPORT)
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

	namespace rmlui
	{
		class RmlDocument;

		class T_DLLCLASS PreviewControl : public ui::Widget
		{
			T_RTTI_CLASS;

		public:
			explicit PreviewControl(editor::IEditor* editor);

			bool create(
				ui::Widget* parent,
				int style,
				db::Database* database,
				resource::IResourceManager* resourceManager,
				render::IRenderSystem* renderSystem
			);

			virtual void destroy() override final;

			void setRmlDocument(RmlDocument* document);

			virtual ui::Size getPreferredSize(const ui::Size& hint) const override final;

		private:
			editor::IEditor* m_editor;
			Ref< ui::EventSubject::IEventHandler > m_idleEventHandler;
			Ref< db::Database > m_database;
			Ref< render::IRenderView > m_renderView;
			Ref< render::RenderContext > m_renderContext;
			Ref< render::RenderGraph > m_renderGraph;
			Ref< RmlDocument > m_document;
			Rml::Context* m_rmlContext = nullptr;

			ui::Point getTwips(const ui::Point& pt) const;

			void eventSize(ui::SizeEvent* event);

			void eventPaint(ui::PaintEvent* event);
		};

	}
}