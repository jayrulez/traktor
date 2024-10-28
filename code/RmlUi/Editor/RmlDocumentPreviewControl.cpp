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
#include "RmlUi/RmlDocumentResource.h"
#include "RmlUi/Backend/RmlUi.h"
#include "RmlUi/Editor/RmlDocumentPreviewControl.h"
#include "Ui/Itf/IWidget.h"
#include "Ui/Application.h"
#include "Resource/IResourceManager.h"
#include "Render/IProgram.h"

namespace traktor::rmlui
{
	T_IMPLEMENT_RTTI_CLASS(L"traktor.rmlui.RmlDocumentPreviewControl", RmlDocumentPreviewControl, ui::Widget)

		RmlDocumentPreviewControl::RmlDocumentPreviewControl(editor::IEditor* editor)
		: m_editor(editor)
	{
	}

	bool RmlDocumentPreviewControl::create(
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

		if (!RmlUi::getInstance().isInitialized())
		{
			return false;
		}

		// todo: get name from rml document
		m_rmlContext = RmlUi::getInstance().createContext(L"Test", Vector2i(m_renderView->getWidth(), m_renderView->getHeight()));

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
						background-color: #fff;
					}
					div
					{
						display: block;
						height: 200px;
						width: 200px;
						background-color: #ff0;
					}
				</style>
				</head>
				<body>
					<div/>
				</body>
				</rml>)");

		document->Show();


		addEventHandler< ui::SizeEvent >(this, &RmlDocumentPreviewControl::eventSize);
		addEventHandler< ui::PaintEvent >(this, &RmlDocumentPreviewControl::eventPaint);

		m_idleEventHandler = ui::Application::getInstance()->addEventHandler< ui::IdleEvent >(this, &RmlDocumentPreviewControl::eventIdle);

		m_database = database;
		return true;
	}

	void RmlDocumentPreviewControl::destroy()
	{
		RmlUi::getInstance().destroyContext(m_rmlContext);

		m_rmlContext = nullptr;

		ui::Application::getInstance()->removeEventHandler(m_idleEventHandler);

		safeClose(m_renderView);

		Widget::destroy();
	}

	void RmlDocumentPreviewControl::setRmlDocument(RmlDocumentResource* rmlDocument)
	{
		m_rmlDocument = rmlDocument;

		const ui::Size sz = getInnerRect().getSize();
	}



	ui::Size RmlDocumentPreviewControl::getPreferredSize(const ui::Size& hint) const
	{
		if (!m_rmlDocument)
			return ui::Size(2631, 1117);

		//Aabb2 bounds = {};//m_document->getFrameBounds();

		int width = 0;// int(bounds.mx.x / 20.0f);
		int height = 0;// int(bounds.mx.y / 20.0f);

		return ui::Size(width, height);
	}

	void RmlDocumentPreviewControl::eventSize(ui::SizeEvent* event)
	{
		ui::Size sz = event->getSize();

		if (m_renderView)
		{
			m_renderView->reset(sz.cx, sz.cy);
			if (!RmlUi::getInstance().getRenderInterface()->reloadResources())
				return;
		}

		if (m_rmlContext)
		{
			m_rmlContext->SetDimensions(Rml::Vector2i(sz.cx, sz.cy));
		}
	}

	void RmlDocumentPreviewControl::eventPaint(ui::PaintEvent* event)
	{
		if (!m_renderView)
			return;

		ui::Size sz = getInnerRect().getSize();

		// Render view events; reset view if it has become lost.
		render::RenderEvent re;
		while (m_renderView->nextEvent(re))
		{
			if (re.type == render::RenderEventType::Lost)
			{
				m_renderView->reset(sz.cx, sz.cy);
				if (!RmlUi::getInstance().getRenderInterface()->reloadResources())
					return;
			}
		}

		// Render frame.
		if (m_renderView->beginFrame())
		{
			RmlUi::getInstance().getRenderInterface()->beginRendering();

			m_rmlContext->Render();

			auto& batches = RmlUi::getInstance().getRenderInterface()->getBatches();

			render::Clear cl;
			cl.mask = render::CfColor;
			cl.colors[0] = Color4f(0.8f, 0.5f, 0.8f, 1.0f);
			if (m_renderView->beginPass(&cl, render::TfAll, render::TfAll))
			{
				for (auto& batch : batches)
				{
					m_renderView->draw(
						batch.compiledGeometry->vertexBuffer->getBufferView(),
						RmlUi::getInstance().getRenderInterface()->getVertexLayout(),
						batch.compiledGeometry->indexBuffer->getBufferView(),
						render::IndexType::UInt32,
						batch.program,
						render::Primitives(render::PrimitiveType::Triangles, 0, batch.compiledGeometry->triangleCount, 0, 0),
						1);
				}

				m_renderView->endPass();
			}

			m_renderView->endFrame();
			m_renderView->present();

			RmlUi::getInstance().getRenderInterface()->endRendering();
		}

		event->consume();
	}

	void RmlDocumentPreviewControl::eventIdle(ui::IdleEvent* event)
	{
		if (!m_rmlContext)
			return;

		m_rmlContext->Update();

		event->requestMore();
	}

}