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
#include "Resource/IResourceManager.h"
#include "Render/IProgram.h"

namespace traktor::rmlui
{
	namespace
	{
		const resource::Id< render::Shader > c_idShaderRmlUiShader(Guid(L"{20046EBD-5DC4-494B-AF59-8F89AFFCC107}"));
		//const resource::Id< render::Shader > c_idShaderRmlUiShaderTexture(Guid(L"{20046EBD-5DC4-494B-AF59-8F89AFFCC107}"));
	}

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

		// todo: get name from rml document
		m_rmlContext = RmlUi::getInstance().CreateContext(L"Test", Vector2i(m_renderView->getWidth(), m_renderView->getHeight()));

		if (!resourceManager->bind(c_idShaderRmlUiShader, m_rmlUiShader))
			return false;

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


		addEventHandler< ui::SizeEvent >(this, &PreviewControl::eventSize);
		addEventHandler< ui::PaintEvent >(this, &PreviewControl::eventPaint);

		m_idleEventHandler = ui::Application::getInstance()->addEventHandler< ui::IdleEvent >(this, &PreviewControl::eventIdle);

		m_database = database;
		return true;
	}

	void PreviewControl::destroy()
	{
		RmlUi::getInstance().DestroyContext(m_rmlContext);

		m_rmlContext = nullptr;

		ui::Application::getInstance()->removeEventHandler(m_idleEventHandler);

		safeClose(m_renderView);

		Widget::destroy();

		m_rmlUiShader.clear();
	}

	void PreviewControl::setRmlDocument(RmlDocument* rmlDocument)
	{
		m_rmlDocument = rmlDocument;

		const ui::Size sz = getInnerRect().getSize();
	}



	ui::Size PreviewControl::getPreferredSize(const ui::Size& hint) const
	{
		if (!m_rmlDocument)
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

		// Render frame.
		if (m_renderView->beginFrame())
		{
			RmlUi::getInstance().GetRenderInterface()->beginRendering();

			m_rmlContext->Render();

			auto& batches = RmlUi::getInstance().GetRenderInterface()->getBatches();

			const render::Shader::Permutation perm(render::handle_t(render::Handle(L"Default")));

			render::IProgram* program = m_rmlUiShader->getProgram(perm).program;

			render::Clear cl;
			cl.mask = render::CfColor;
			cl.colors[0] = Color4f(0.8f, 0.5f, 0.8f, 1.0f);
			if (m_renderView->beginPass(&cl, render::TfAll, render::TfAll))
			{
				for (auto& batch : batches)
				{
					program->setVectorParameter(render::getParameterHandle(L"RmlUi_Translation"), Vector4(batch.translation.x, batch.translation.y, 0, 0));

					m_renderView->draw(
						batch.compiledGeometry->vertexBuffer->getBufferView(),
						RmlUi::getInstance().GetRenderInterface()->getVertexLayout(),
						batch.compiledGeometry->indexBuffer->getBufferView(),
						render::IndexType::UInt32,
						program,
						render::Primitives(render::PrimitiveType::Triangles, 0, batch.compiledGeometry->triangleCount, 0, 0),
						1);
				}

				m_renderView->endPass();
			}

			m_renderView->endFrame();
			m_renderView->present();

			RmlUi::getInstance().GetRenderInterface()->endRendering();
		}

		event->consume();
	}

	void PreviewControl::eventIdle(ui::IdleEvent* event)
	{
		if (!m_rmlContext)
			return;

		m_rmlContext->Update();

		event->requestMore();
	}

}