/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/TurboBadgerUi.h"
#include "TurboBadgerUi/TurboBadgerUiView.h"

#include "Core/Singleton/SingletonManager.h"
#include "Core/Misc/TString.h"
#include "Core/Log/Log.h"
#include "Resource//IResourceManager.h"

#include "tb_core.h"
#include "animation/tb_animation.h"
#include "animation/tb_widget_animation.h"
#include "tb_font_renderer.h"
#include "tb_language.h"
#include "tb_msg.h"
#include "tb_widgets.h"
#include "tb_widgets_reader.h"
#include "tb_node_tree.h"

namespace traktor::turbobadgerui
{
	namespace {
		const resource::Id< resource::FileBundle > c_idTurboBadgerUiDefaultResources(Guid(L"{2719C20D-0066-0347-BD57-9866C5F7650B}")); // System/TurboBadgerUi/DefaultResources
	}

	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.turbobadgerui.TurboBadgerUi", 0, TurboBadgerUi, Object)

		TurboBadgerUi::BackendData::BackendData(render::IRenderSystem* renderSystem)
		: renderer(renderSystem)
	{

	}

	TurboBadgerUi& TurboBadgerUi::getInstance()
	{
		static TurboBadgerUi* s_instance = nullptr;
		if (!s_instance)
		{
			s_instance = new TurboBadgerUi();
			s_instance->addRef(nullptr);
			SingletonManager::getInstance().add(s_instance);
		}
		return *s_instance;
	}

	void TurboBadgerUi::destroy()
	{
		T_SAFE_RELEASE(this);
	}

	bool TurboBadgerUi::initialize(render::IRenderSystem* renderSystem)
	{
		if (m_initialized)
			return true;

		m_backendData = new BackendData(renderSystem);

		m_initialized = tb::tb_core_init(
			&m_backendData->renderer,
			&m_backendData->systemInterface,
			&m_backendData->fileInterface,
			&m_backendData->clipboardInterface
		);

		// Load language file
		if (!tb::g_tb_lng->Load("data/Assets/System/TurboBadgerUi/resources/language/lng_en.tb.txt"))
		{
			log::error << L"TurboBadgerUi: Failed to load language resource." << Endl;
		}

		// Load the default skin, and override skin that contains the graphics specific to the demo.
		if (!tb::g_tb_skin->Load("data/Assets/System/TurboBadgerUi/resources/default_skin/skin.tb.txt", "data/Assets/System/TurboBadgerUi/demo/skin/skin.tb.txt"))
		{
			log::error << L"TurboBadgerUi: Failed to skin resources." << Endl;
		}

		// Register font renderers.
#ifdef TB_FONT_RENDERER_TBBF
		void register_tbbf_font_renderer();
		register_tbbf_font_renderer();
#endif
#ifdef TB_FONT_RENDERER_STB
		void register_stb_font_renderer();
		register_stb_font_renderer();
#endif
#ifdef TB_FONT_RENDERER_FREETYPE
		void register_freetype_font_renderer();
		register_freetype_font_renderer();
#endif

		// Add fonts we can use to the font manager.
#if defined(TB_FONT_RENDERER_STB) || defined(TB_FONT_RENDERER_FREETYPE)
		if (!tb::g_font_manager->AddFontInfo("data/Assets/System/TurboBadgerUi/resources/vera.ttf", "Vera"))
		{
			log::error << L"TurboBadgerUi: Failed to load Vera font." << Endl;
		}
#endif
#ifdef TB_FONT_RENDERER_TBBF
		if (!tb::g_font_manager->AddFontInfo("data/Assets/System/TurboBadgerUi/resources/default_font/segoe_white_with_shadow.tb.txt", "Segoe"))
			log::error << L"TurboBadgerUi: Failed to load Segoe font." << Endl;

		if (!tb::g_font_manager->AddFontInfo("data/Assets/System/TurboBadgerUi/demo/fonts/neon.tb.txt", "Neon"))
			log::error << L"TurboBadgerUi: Failed to load Neon font." << Endl;

		if (!tb::g_font_manager->AddFontInfo("data/Assets/System/TurboBadgerUi/demo/fonts/orangutang.tb.txt", "Orangutang"))
			log::error << L"TurboBadgerUi: Failed to load Orangutang font." << Endl;

		if (!tb::g_font_manager->AddFontInfo("data/Assets/System/TurboBadgerUi/demo/fonts/orange.tb.txt", "Orange"))
			log::error << L"TurboBadgerUi: Failed to load Orange font." << Endl;
#endif

		// Set the default font description for widgets to one of the fonts we just added
		tb::TBFontDescription fd;
#ifdef TB_FONT_RENDERER_TBBF
		fd.SetID(TBIDC("Segoe"));
#else
		fd.SetID(TBIDC("Vera"));
#endif
		fd.SetSize(tb::g_tb_skin->GetDimensionConverter()->DpToPx(14));
		tb::g_font_manager->SetDefaultFontDescription(fd);

		// Create the font now.
		tb::TBFontFace* font = tb::g_font_manager->CreateFontFace(tb::g_font_manager->GetDefaultFontDescription());

		// Render some glyphs in one go now since we know we are going to use them. It would work fine
		// without this since glyphs are rendered when needed, but with some extra updating of the glyph bitmap.
		if (font)
			font->RenderGlyphs(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~��������");

		tb::TBWidgetsAnimationManager::Init();

		if (!m_initialized)
		{
			delete m_backendData;
		}

		return m_initialized;
		}

	bool TurboBadgerUi::loadDefaultResources(resource::IResourceManager* resourceManager)
	{
		if (!resourceManager->bind(c_idTurboBadgerUiDefaultResources, m_defaultResources))
			return false;

		//tb::g_tb_lng->Load("");
		std::wstring lang = m_defaultResources->lookup(L"data/Assets/System/TurboBadgerUi/resources/language/lng_en.tb.txt");		

		//tb::g_tb_skin->Load("");
		std::wstring skin = m_defaultResources->lookup(L"data/Assets/System/TurboBadgerUi/resources/default_skin/skin.tb.txt");

		//tb::g_font_manager->AddFontInfo("", "");
		std::wstring font1 = m_defaultResources->lookup(L"data/Assets/System/TurboBadgerUi/resources/vera.ttf");
		//tb::g_font_manager->AddFontInfo("", "");
		std::wstring font2 = m_defaultResources->lookup(L"data/Assets/System/TurboBadgerUi/resources/default_font/segoe_white_with_shadow.tb.txt");

		return true;
	}

	void TurboBadgerUi::shutdown()
	{
		if (!m_initialized)
			return;

		tb::TBWidgetsAnimationManager::Shutdown();

		tb::g_tb_skin->UnloadBitmaps();

		tb::tb_core_shutdown();

		delete m_backendData;

		m_initialized = false;
	}

	bool TurboBadgerUi::isInitialized() const
	{
		return m_initialized;
	}

	TurboBadgerUiView* TurboBadgerUi::createView(const Path& resourcePath, int32_t width, int32_t height)
	{
		if (!m_initialized)
			return nullptr;

		TurboBadgerUiView* view = new TurboBadgerUiView();
		tb::g_widgets_reader->LoadFile(view, wstombs(resourcePath.getOriginal()).c_str());

		setupViewDefaults(view, width, height);

		m_views.push_back(view);

		return view;
	}

	TurboBadgerUiView* TurboBadgerUi::createView(const void* resourceData, int32_t width, int32_t height)
	{
		if (!m_initialized)
			return nullptr;

		TurboBadgerUiView* view = new TurboBadgerUiView();
		tb::g_widgets_reader->LoadData(view, reinterpret_cast<const char*>(resourceData));

		setupViewDefaults(view, width, height);

		m_views.push_back(view);

		return view;
	}

	void TurboBadgerUi::destroyView(TurboBadgerUiView* view)
	{
		if (view == nullptr)
			return;

		auto it = std::find(m_views.begin(), m_views.end(), view);
		if (it != m_views.end())
		{
			m_views.erase(it);
		}

		view->Die();
	}

	void TurboBadgerUi::update()
	{
		tb::TBMessageHandler::ProcessMessages();
		tb::TBAnimationManager::Update();
	}

	void TurboBadgerUi::updateView(TurboBadgerUiView* view)
	{
		view->InvokeProcessStates();
		view->InvokeProcess();
	}

	void TurboBadgerUi::reloadRenderResources()
	{
		m_backendData->renderer.InvokeContextLost(); // Forget all bitmaps
		m_backendData->renderer.InvokeContextRestored(); // Reload all bitmaps
	}

	void TurboBadgerUi::renderView(TurboBadgerUiView* view, uint32_t width, uint32_t height, AlignedVector<TurboBadgerUiBatch>& batches)
	{
		batches = m_backendData->renderer.renderView(view, width, height);
	}

	void TurboBadgerUi::resizeView(TurboBadgerUiView* view, int32_t width, int32_t height)
	{
		view->Invalidate();
		view->SetSize(width, height);
		view->InvalidateLayout(tb::TBWidget::INVALIDATE_LAYOUT_RECURSIVE);
	}

	void TurboBadgerUi::setupViewDefaults(TurboBadgerUiView* view, int32_t width, int32_t height)
	{
		view->Invalidate();
		view->SetSize(width, height);
		view->SetOpacity(0.97f);
		view->SetSkinBg(TBIDC("background"));

		view->InvalidateLayout(tb::TBWidget::INVALIDATE_LAYOUT_RECURSIVE);

		// Set gravity all so we resize correctly
		view->SetGravity(tb::WIDGET_GRAVITY_ALL);

#if true
		{
			view->RemoveChild(view->GetFirstChild());

			m_mainWindow = new tb::TBWindow();
			m_mainWindow->Invalidate();
			m_mainWindow->InvalidateLayout(tb::TBWidget::INVALIDATE_LAYOUT_RECURSIVE);
			view->AddChild(m_mainWindow);

			tb::TBNode node;
			if (node.ReadFile("data/Assets/System/TurboBadgerUi/demo/ui_resources/test_ui.tb.txt"))
			{
				tb::g_widgets_reader->LoadNodeTree(m_mainWindow, &node);

				// Get title from the WindowInfo section (or use "" if not specified)
				m_mainWindow->SetText(node.GetValueString("WindowInfo>title", ""));

				const tb::TBRect parent_rect(0, 0, width, height);
				const tb::TBDimensionConverter* dc = tb::g_tb_skin->GetDimensionConverter();
				tb::TBRect window_rect = m_mainWindow->GetResizeToFitContentRect();

				// Use specified size or adapt to the preferred content size.
				tb::TBNode* tmp = node.GetNode("WindowInfo>size");
				if (tmp && tmp->GetValue().GetArrayLength() == 2)
				{
					window_rect.w = dc->GetPxFromString(tmp->GetValue().GetArray()->GetValue(0)->GetString(), window_rect.w);
					window_rect.h = dc->GetPxFromString(tmp->GetValue().GetArray()->GetValue(1)->GetString(), window_rect.h);
				}

				// Use the specified position or center in parent.
				tmp = node.GetNode("WindowInfo>position");
				if (tmp && tmp->GetValue().GetArrayLength() == 2)
				{
					window_rect.x = dc->GetPxFromString(tmp->GetValue().GetArray()->GetValue(0)->GetString(), window_rect.x);
					window_rect.y = dc->GetPxFromString(tmp->GetValue().GetArray()->GetValue(1)->GetString(), window_rect.y);
				}
				else
					window_rect = window_rect.CenterIn(parent_rect);

				// Make sure the window is inside the parent, and not larger.
				window_rect = window_rect.MoveIn(parent_rect).Clip(parent_rect);

				m_mainWindow->SetRect(window_rect);

				m_mainWindow->SetOpacity(0.97f);
			}
		}
#endif

		view->SetFocus(tb::WIDGET_FOCUS_REASON_UNKNOWN);
	}
	}