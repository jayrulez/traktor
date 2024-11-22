/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/Editor/TurboBadgerUiEditorPage.h"
#include "TurboBadgerUi/TurboBadgerUiResourceFactory.h"
#include "TurboBadgerUi/TurboBadgerUiResource.h"
#include "TurboBadgerUi/Editor/TurboBadgerUiPreviewControl.h"

#include "Core/Misc/SafeDestroy.h"
#include "Core/Settings/PropertyGroup.h"
#include "Core/Settings/PropertyInteger.h"
#include "Core/Settings/PropertyString.h"
#include "Core/Settings/PropertyBoolean.h"
#include "Core/Misc/ObjectStore.h"
#include "Render/IRenderSystem.h"
#include "Editor/IEditor.h"
#include "Editor/IDocument.h"
#include "Database/Database.h"
#include "Database/Instance.h"
#include "Resource/ResourceManager.h"
#include "Render/Resource/ShaderFactory.h"
#include "Render/Resource/TextureFactory.h"

#include "I18N/Text.h"

#include "Ui/Application.h"
#include "Ui/Container.h"
#include "Ui/StyleBitmap.h"
#include "Ui/TableLayout.h"
#include "Ui/AspectLayout.h"
#include "Ui/CenterLayout.h"
#include "Ui/Splitter.h"
#include "Ui/StatusBar/StatusBar.h"
#include "Ui/ToolBar/ToolBar.h"
#include "Ui/ToolBar/ToolBarButton.h"
#include "Ui/ToolBar/ToolBarButtonClickEvent.h"
#include "Ui/ToolBar/ToolBarSeparator.h"
#include "Ui/GridLayout.h"
#include "Ui/FloodLayout.h"
#include "Ui/FlowLayout.h"

namespace traktor::turbobadgerui
{
	T_IMPLEMENT_RTTI_CLASS(L"traktor.turbobadgerui.TurboBadgerUiEditorPage", TurboBadgerUiEditorPage, editor::IEditorPage)

		TurboBadgerUiEditorPage::TurboBadgerUiEditorPage(editor::IEditor* editor, editor::IEditorPageSite* site, editor::IDocument* document)
		: m_editor(editor)
		, m_site(site)
		, m_document(document)
	{
	}

	bool TurboBadgerUiEditorPage::create(ui::Container* parent)
	{
		Ref< render::IRenderSystem > renderSystem = m_editor->getObjectStore()->get< render::IRenderSystem >();
		if (!renderSystem)
			return false;

		Ref< db::Database > database = m_editor->getOutputDatabase();
		if (!database)
			return false;

		// Read ui resource from output database.
		m_uiResource = database->getObjectReadOnly< TurboBadgerUiResource >(m_document->getInstance(0)->getGuid());
		if (!m_uiResource)
			return false;

		m_resourceManager = new resource::ResourceManager(database, m_editor->getSettings()->getProperty< bool >(L"Resource.Verbose", true));
		m_resourceManager->addFactory(new render::ShaderFactory(renderSystem));
		m_resourceManager->addFactory(new render::TextureFactory(renderSystem, 0));
		m_resourceManager->addFactory(new turbobadgerui::TurboBadgerUiResourceFactory());

		m_container = new ui::Container();
		m_container->create(parent, ui::WsNone, new ui::TableLayout(L"100%", L"*,100%", 0_ut, 0_ut));

		m_toolBar = new ui::ToolBar();
		m_toolBar->create(m_container);
		for (int32_t i = 0; i < 6; ++i)
			m_toolBar->addImage(new ui::StyleBitmap(L"Flash.Playback", i));
		m_toolBar->addItem(new ui::ToolBarButton(
			i18n::Text(L"TURBOBADGERUI_EDITOR_RELOAD"),
			0,
			ui::Command(L"TurboBadgerUi.Editor.Reload")
		));

		Ref< ui::Splitter > splitter = new ui::Splitter();
		splitter->create(m_container, true, 300_ut);

		// Create aspect container.
		Ref< ui::Container > previewContainer = new ui::Container();
		previewContainer->create(m_container, ui::WsNone, new ui::AspectLayout(16.0f / 9.0f));

		m_previewControl = new TurboBadgerUiPreviewControl(m_editor, database, m_resourceManager, renderSystem);
		if (!m_previewControl->create(previewContainer, m_uiResource))
			return false;

		// Create status bar.
		m_statusBar = new ui::StatusBar();
		m_statusBar->create(m_container, ui::WsDoubleBuffer);
		m_statusBar->addColumn(-1);

		m_previewControl->update();

		return true;
	}

	void TurboBadgerUiEditorPage::destroy()
	{
		safeDestroy(m_previewControl);
		safeDestroy(m_resourceManager);
	}

	bool TurboBadgerUiEditorPage::dropInstance(db::Instance* instance, const ui::Point& position)
	{
		return false;
	}

	bool TurboBadgerUiEditorPage::handleCommand(const ui::Command& command)
	{
		bool result = true;

		if (command == L"TurboBadgerUi.Editor.Reload")
		{
			// todo: reload document
		}
		else
			result = false;

		return result;
	}

	void TurboBadgerUiEditorPage::handleDatabaseEvent(db::Database* database, const Guid& eventId)
	{
		bool shouldRedraw = false;

		if (m_resourceManager)
			shouldRedraw |= m_resourceManager->reload(eventId, false);

		if (eventId == m_document->getInstance(0)->getGuid())
		{
			Ref< TurboBadgerUiResource > uiResource = database->getObjectReadOnly< TurboBadgerUiResource >(m_document->getInstance(0)->getGuid());
			if (uiResource)
			{
				m_uiResource = uiResource;

				m_previewControl->setUiResource(m_uiResource);
				shouldRedraw |= true;
			}
		}

		if (shouldRedraw)
			m_previewControl->update();
	}
}