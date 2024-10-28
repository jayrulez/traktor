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
#include "RmlUi/Core/Context.h"
#include <Render/Buffer.h>
#include <Render/IVertexLayout.h>
#include <Render/Shader.h>
#include "Resource/Proxy.h"

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
		class RmlDocumentResource;

		class T_DLLCLASS RmlDocumentPreviewControl : public ui::Widget
		{
			T_RTTI_CLASS;

		public:
			explicit RmlDocumentPreviewControl(editor::IEditor* editor);

			bool create(
				ui::Widget* parent,
				int style,
				db::Database* database,
				resource::IResourceManager* resourceManager,
				render::IRenderSystem* renderSystem
			);

			virtual void destroy() override final;

			void setRmlDocument(RmlDocumentResource* rmlDocument);

			virtual ui::Size getPreferredSize(const ui::Size& hint) const override final;

		private:
			editor::IEditor* m_editor;
			Ref< ui::EventSubject::IEventHandler > m_idleEventHandler;
			Ref< db::Database > m_database;
			Ref< render::IRenderView > m_renderView;
			Rml::Context* m_rmlContext = nullptr;
			Ref< RmlDocumentResource > m_rmlDocument;

			void eventSize(ui::SizeEvent* event);

			void eventPaint(ui::PaintEvent* event);

			void eventIdle(ui::IdleEvent* event);
		};

	}
}