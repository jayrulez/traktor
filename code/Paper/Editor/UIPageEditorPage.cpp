/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Paper/Editor/UIPageEditorPage.h"

#include "Core/Log/Log.h"
#include "Core/Misc/ObjectStore.h"
#include "Core/Settings/PropertyBoolean.h"
#include "Core/Settings/PropertyGroup.h"
#include "Database/Database.h"
#include "Database/Instance.h"
#include "Editor/IDocument.h"
#include "Editor/IEditor.h"
#include "Paper/Ui/UIPage.h"
#include "Paper/Editor/UIPageAsset.h"
#include "Paper/Editor/UIPagePreviewControl.h"
#include "Render/Resource/ShaderFactory.h"
#include "Render/Resource/TextureFactory.h"
#include "Render/IRenderSystem.h"
#include "Resource/ResourceManager.h"
#include "Ui/Container.h"
#include "Ui/TableLayout.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.paper.UIPageEditorPage", UIPageEditorPage, editor::IEditorPage)

UIPageEditorPage::UIPageEditorPage(editor::IEditor* editor, editor::IEditorPageSite* site, editor::IDocument* document)
	: m_editor(editor)
	, m_site(site)
	, m_document(document)
{
}

bool UIPageEditorPage::create(ui::Container* parent)
{
	Ref< db::Database > database = m_editor->getOutputDatabase();
	if (!database)
		return false;

	Ref< render::IRenderSystem > renderSystem = m_editor->getObjectStore()->get< render::IRenderSystem >();
	if (!renderSystem)
		return false;

	m_resourceManager = new resource::ResourceManager(database, m_editor->getSettings()->getProperty< bool >(L"Resource.Verbose", true));
	m_resourceManager->addFactory(new render::ShaderFactory(renderSystem));
	m_resourceManager->addFactory(new render::TextureFactory(renderSystem, 0));

	Ref< ui::Container > container = new ui::Container();
	if (!container->create(parent, ui::WsNone, new ui::TableLayout(L"100%", L"100%", ui::Unit(0), ui::Unit(0))))
		return false;

	// Create preview control.
	m_previewControl = new UIPagePreviewControl(m_editor, database, m_resourceManager, renderSystem);
	if (!m_previewControl->create(container))
		return false;

	// Initial update to load the UI page.
	updatePreview();

	return true;
}

void UIPageEditorPage::destroy()
{
	if (m_previewControl)
	{
		m_previewControl->destroy();
		m_previewControl = nullptr;
	}
}

bool UIPageEditorPage::dropInstance(db::Instance* instance, const ui::Point& position)
{
	return false;
}

bool UIPageEditorPage::handleCommand(const ui::Command& command)
{
	bool result = true;

	if (command == L"Paper.Editor.Reload")
	{
		// Reload the UI page
		updatePreview();
	}
	else
		result = false;

	return result;
}

void UIPageEditorPage::handleDatabaseEvent(db::Database* database, const Guid& eventId)
{
	// If the UI page resource or its dependencies were updated, reload it.

	bool shouldRedraw = false;

	if (m_resourceManager)
		shouldRedraw |= m_resourceManager->reload(eventId, false);

	if (eventId == m_uiPageGuid)
	{
		Ref< UIPage > uiPageResource = database->getObjectReadOnly< UIPage >(m_document->getInstance(0)->getGuid());
		if (uiPageResource)
			shouldRedraw |= true;
	}

	if (shouldRedraw)
		updatePreview();
}

void UIPageEditorPage::updatePreview()
{
	if (!m_previewControl)
		return;

	// Get the source instance guid.
	Ref< db::Instance > sourceInstance = m_document->getInstance(0);
	if (!sourceInstance)
		return;

	m_uiPageGuid = sourceInstance->getGuid();

	// Try to load the built UI page from the output database.
	Ref< db::Database > outputDatabase = m_editor->getOutputDatabase();
	if (!outputDatabase)
		return;

	Ref< UIPage > uiPage = outputDatabase->getObjectReadOnly< UIPage >(m_uiPageGuid);
	if (!uiPage)
	{
		log::info << L"UIPage not yet built; preview unavailable." << Endl;
		return;
	}

	m_previewControl->setUIPage(uiPage);
}

}
