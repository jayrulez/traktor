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
#include "Render/VertexElement.h"
#include "Render/Frame/RenderGraph.h"
#include "RmlUi/RmlDocument.h"
#include "RmlUi/Backend/RmlUi.h"
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
			desc.syswin = getIWidget()->getSystemWindow();

			m_renderView = renderSystem->createRenderView(desc);
			if (!m_renderView)
				return false;

			m_renderContext = new render::RenderContext(4 * 1024 * 1024);
			m_renderGraph = new render::RenderGraph(renderSystem, desc.multiSample);

			// todo: get name from rml document
			m_rmlContext = RmlUi::getInstance().CreateContext(L"Test", Vector2i(m_renderView->getWidth(), m_renderView->getHeight()));

			/*m_vertexBuffer = renderSystem->createBuffer(render::BufferUsage::BuVertex, 0, true);
			m_indexBuffer = renderSystem->createBuffer(render::BufferUsage::BuIndex, 0, true);

			AlignedVector< render::VertexElement > vertexElements = {};
			vertexElements.push_back(render::VertexElement(render::DataUsage::Position, render::DataType::DtFloat2, 0));
			vertexElements.push_back(render::VertexElement(render::DataUsage::Color, render::DataType::DtFloat3, 8));
			m_vertexLayout = renderSystem->createVertexLayout(vertexElements);*/

			// todo: remove
			// temporary testing
			Rml::ElementDocument* document = m_rmlContext->LoadDocumentFromMemory(R"(<rml>
				<head>
				<title>Example</title>
				<style>
					body
					{
						position: absolute;
						top: 50px;
						left: 50px;
						width: 500px;
						height: 500px;
						background-color: #ccc;
					}
					div
					{
						display: block;
						height: 150px;
						width: 200px;
						background-color: #f00;
					}
				</style>
				</head>
				<body>
					<div/>
				</body>
				</rml>)");

			document->Show();


			addEventHandler< ui::SizeEvent >(this, &PreviewControl::eventSize);
			addEventHandler< ui::PaintEvent >(this, &PreviewControl::eventPaint);

			// todo: input events

			m_database = database;
			return true;
		}

		void PreviewControl::destroy()
		{
			ui::Application::getInstance()->removeEventHandler(m_idleEventHandler);

			safeDestroy(m_renderGraph);
			safeClose(m_renderView);

			m_vertexBuffer.reset();
			m_indexBuffer.reset();
			m_vertexLayout.reset();

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
				return ui::Size(2631, 1117);

			//Aabb2 bounds = {};//m_document->getFrameBounds();

			int width = 0;// int(bounds.mx.x / 20.0f);
			int height = 0;// int(bounds.mx.y / 20.0f);

			return ui::Size(width, height);
		}

		void PreviewControl::eventSize(ui::SizeEvent* event)
		{
			ui::Size sz = event->getSize();

			if (m_renderView)
			{
				m_renderView->reset(sz.cx, sz.cy);
			}

			if (m_rmlContext)
			{
				m_rmlContext->SetDimensions(Rml::Vector2i(sz.cx, sz.cy));
			}
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

			m_rmlContext->Update();


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
				RmlUi::getInstance().GetRenderInterface()->beginRendering(
					m_renderView,
					m_renderGraph,
					m_renderContext,
					m_vertexBuffer,
					m_indexBuffer,
					m_vertexLayout);

				m_rmlContext->Render();

				RmlUi::getInstance().GetRenderInterface()->endRendering();

				m_renderContext->render(m_renderView);
				m_renderView->endFrame();
				m_renderView->present();
			}

			event->consume();
		}
	}
}