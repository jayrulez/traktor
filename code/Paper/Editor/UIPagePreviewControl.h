/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Ref.h"
#include "Core/Timer/Timer.h"
#include "Ui/Widget.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_PAPER_EDITOR_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::db
{

class Database;

}

namespace traktor::editor
{

class IEditor;

}

namespace traktor::render
{

class IRenderSystem;
class IRenderView;
class RenderContext;
class RenderGraph;

}

namespace traktor::resource
{

class IResourceManager;

}

namespace traktor::paper
{

class UIPage;
class Draw2D;
class FontManager;
class BitmapFontRenderer;

class T_DLLCLASS UIPagePreviewControl : public ui::Widget
{
	T_RTTI_CLASS;

public:
	explicit UIPagePreviewControl(editor::IEditor* editor,
		db::Database* database,
		resource::IResourceManager* resourceManager,
		render::IRenderSystem* renderSystem);

	bool create(ui::Widget* parent);

	virtual void destroy() override final;

	void setUIPage(UIPage* uiPage);

private:
	editor::IEditor* m_editor;
	Ref< ui::EventSubject::IEventHandler > m_idleEventHandler;
	Ref< db::Database > m_database = nullptr;
	Ref< resource::IResourceManager > m_resourceManager;
	Ref< render::IRenderSystem > m_renderSystem;
	Ref< render::IRenderView > m_renderView;
	Ref< render::RenderContext > m_renderContext;
	Ref< render::RenderGraph > m_renderGraph;
	Ref< Draw2D > m_renderer;
	Ref< FontManager > m_fontManager;
	Ref< BitmapFontRenderer > m_fontRenderer;
	Ref< UIPage > m_uiPage;
	Timer m_timer;

	void eventSize(ui::SizeEvent* event);

	void eventPaint(ui::PaintEvent* event);

	void eventIdle(ui::IdleEvent* event);

	void eventMouseMove(ui::MouseMoveEvent* event);

	void eventButtonDown(ui::MouseButtonDownEvent* event);

	void eventButtonUp(ui::MouseButtonUpEvent* event);
};

}
