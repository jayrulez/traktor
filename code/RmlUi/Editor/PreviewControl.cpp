/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <cstring>
#include "Core/Log/Log.h"
#include "Core/Math/Const.h"
#include "Core/Misc/SafeDestroy.h"
#include "Core/Settings/PropertyFloat.h"
#include "Core/Settings/PropertyGroup.h"
#include "Core/Settings/PropertyInteger.h"
#include "Editor/IEditor.h"
#include "Render/IRenderSystem.h"
#include "Render/IRenderTargetSet.h"
#include "Render/IRenderView.h"
#include "Render/ScreenRenderer.h"
#include "Render/Shader.h"
#include "Render/Context/RenderContext.h"
#include "Render/Frame/RenderGraph.h"
#include "RmlUi/RmlDocument.h"
#include "RmlUi/Editor/PreviewControl.h"
#include "Ui/Itf/IWidget.h"
#include "Ui/Application.h"

namespace traktor
{
	namespace rmlui
	{
		T_IMPLEMENT_RTTI_CLASS(L"traktor.rmlui.PreviewControl", PreviewControl, ui::Widget)

			PreviewControl::PreviewControl(editor::IEditor* editor)
			: m_editor(editor)
		{
		}

		bool PreviewControl::create(
			ui::Widget* parent,
			int style,
			db::Database* database,
			resource::IResourceManager* resourceManager,
			render::IRenderSystem* renderSystem
		)
		{
			if (!Widget::create(parent, style | ui::WsFocus | ui::WsNoCanvas))
				return false;

			render::RenderViewEmbeddedDesc desc;
			desc.depthBits = 16;
			desc.stencilBits = 8;
			desc.multiSample = m_editor->getSettings()->getProperty< int32_t >(L"Editor.MultiSample", 4);
			desc.multiSampleShading = m_editor->getSettings()->getProperty< float >(L"Editor.MultiSampleShading", 0.0f);
			desc.waitVBlanks = 1;
			//desc.syswin = getIWidget()->getSystemWindow();

			m_renderView = renderSystem->createRenderView(desc);
			if (!m_renderView)
				return false;

			m_renderContext = new render::RenderContext(4 * 1024 * 1024);
			m_renderGraph = new render::RenderGraph(renderSystem, desc.multiSample);

			
			

			addEventHandler< ui::SizeEvent >(this, &PreviewControl::eventSize);
			addEventHandler< ui::PaintEvent >(this, &PreviewControl::eventPaint);

			m_database = database;
			return true;
		}

		void PreviewControl::destroy()
		{
			ui::Application::getInstance()->removeEventHandler(m_idleEventHandler);

			safeDestroy(m_renderGraph);
			safeClose(m_renderView);

			Widget::destroy();
		}

		void PreviewControl::setRmlDocument(RmlDocument* document)
		{
			m_document = document;

			

			const ui::Size sz = getInnerRect().getSize();

			
		}

		

		ui::Size PreviewControl::getPreferredSize(const ui::Size& hint) const
		{
			if (!m_document)
				return ui::Size(400, 300);

			//Aabb2 bounds = {};//m_document->getFrameBounds();

			int width = 0;// int(bounds.mx.x / 20.0f);
			int height = 0;// int(bounds.mx.y / 20.0f);

			return ui::Size(width, height);
		}

		ui::Point PreviewControl::getTwips(const ui::Point& pt) const
		{
			ui::Size innerSize = getInnerRect().getSize();

			float x = 1;// (pt.x * m_movie->getFrameBounds().mx.x) / float(innerSize.cx);
			float y = 1;// (pt.y * m_movie->getFrameBounds().mx.y) / float(innerSize.cy);

			return ui::Point(int(x), int(y));
		}

		void PreviewControl::eventSize(ui::SizeEvent* event)
		{
			ui::Size sz = event->getSize();

			if (m_renderView)
				m_renderView->reset(sz.cx, sz.cy);
		}

		void PreviewControl::eventPaint(ui::PaintEvent* event)
		{
			if (!m_renderView)
				return;

			ui::Size sz = getInnerRect().getSize();

			// Render view events; reset view if it has become lost.
			render::RenderEvent re;
			while (m_renderView->nextEvent(re))
			{
				if (re.type == render::RenderEventType::Lost)
					m_renderView->reset(sz.cx, sz.cy);
			}

			// Add passes to render graph.

			// Validate render graph.
			if (!m_renderGraph->validate())
				return;

			// Build render context.
			m_renderContext->flush();
			m_renderGraph->build(m_renderContext, sz.cx, sz.cy);

			// Render frame.
			if (m_renderView->beginFrame())
			{
				m_renderContext->render(m_renderView);
				m_renderView->endFrame();
				m_renderView->present();
			}

			event->consume();
		}
	}
}