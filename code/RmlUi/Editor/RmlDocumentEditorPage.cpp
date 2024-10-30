/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Io/FileSystem.h"
#include "Core/Io/MemoryStream.h"
#include "Core/Math/Format.h"
#include "Core/Misc/ObjectStore.h"
#include "Core/Misc/SafeDestroy.h"
#include "Core/Misc/String.h"
#include "Core/Misc/TString.h"
#include "Core/Settings/PropertyGroup.h"
#include "Core/Settings/PropertyInteger.h"
#include "Core/Settings/PropertyString.h"
#include "Core/Settings/PropertyBoolean.h"
#include "Database/Database.h"
#include "Database/Instance.h"
#include "I18N/Text.h"
#include "Editor/IDocument.h"
#include "Editor/IEditor.h"
#include "RmlUi/RmlDocumentResource.h"
#include "RmlUi/Editor/RmlDocumentEditorPage.h"
#include "RmlUi/Editor/RmlDocumentAsset.h"
#include "RmlUi/Editor/RmlDocumentPreviewControl.h"
#include "Render/IRenderSystem.h"
#include "Render/Resource/ShaderFactory.h"
#include "Resource/ResourceManager.h"
#include "Ui/Application.h"
#include "Ui/Container.h"
#include "Ui/StyleBitmap.h"
#include "Ui/TableLayout.h"
#include "Ui/AspectLayout.h"
#include "Ui/CenterLayout.h"
#include "Ui/Splitter.h"
#include "Ui/ToolBar/ToolBar.h"
#include "Ui/ToolBar/ToolBarButton.h"
#include "Ui/ToolBar/ToolBarButtonClickEvent.h"
#include "Ui/ToolBar/ToolBarSeparator.h"
#include "Ui/TreeView/TreeView.h"
#include "Ui/TreeView/TreeViewItem.h"
#include "RmlUi/Backend/RmlUi.h"

namespace traktor
{
	namespace rmlui
	{

		T_IMPLEMENT_RTTI_CLASS(L"traktor.rmlui.RmlDocumentEditorPage", RmlDocumentEditorPage, editor::IEditorPage)

			RmlDocumentEditorPage::RmlDocumentEditorPage(editor::IEditor* editor, editor::IEditorPageSite* site, editor::IDocument* document)
			: m_editor(editor)
			, m_site(site)
			, m_document(document)
		{
		}

		bool RmlDocumentEditorPage::create(ui::Container* parent)
		{
			Ref< render::IRenderSystem > renderSystem = m_editor->getObjectStore()->get< render::IRenderSystem >();
			if (!renderSystem)
				return false;

			Ref< db::Database > database = m_editor->getOutputDatabase();
			if (!database)
				return false;

			RmlUi::getInstance().reloadResources();

			// Read rml document from output database.
			//m_RmlDocument = database->getObjectReadOnly< RmlDocumentResource >(m_document->getInstance(0)->getGuid());
			//if (!m_RmlDocument)
			//	return false;

			m_resourceManager = new resource::ResourceManager(database, m_editor->getSettings()->getProperty< bool >(L"Resource.Verbose", true));
			//m_resourceManager->addFactory(new rmlui::RmlDocumentResourceFactory());

			Ref< ui::Container > container = new ui::Container();
			container->create(parent, ui::WsNone, new ui::TableLayout(L"100%", L"*,100%", 0_ut, 0_ut));

			m_toolBarPlay = new ui::ToolBar();
			m_toolBarPlay->create(container);
			for (int32_t i = 0; i < 6; ++i)
				m_toolBarPlay->addImage(new ui::StyleBitmap(L"Flash.Playback", i));
			m_toolBarPlay->addItem(new ui::ToolBarButton(i18n::Text(L"RMLUI_EDITOR_RELOAD"), 0, ui::Command(L"RmlUi.Editor.Reload")));


			Ref< ui::Splitter > splitter = new ui::Splitter();
			splitter->create(container, true, 300_ut);

			m_treeMovie = new ui::TreeView();
			m_treeMovie->create(splitter, ui::TreeView::WsTreeButtons | ui::TreeView::WsTreeLines | ui::WsDoubleBuffer);

			m_previewControl = new RmlDocumentPreviewControl(m_editor);
			if (!m_previewControl->create(splitter, ui::WsNone, database, m_resourceManager, renderSystem))
				return false;

			m_previewControl->setRmlDocument(m_RmlDocument);
			m_previewControl->update();

			return true;
		}

		void RmlDocumentEditorPage::destroy()
		{
			safeDestroy(m_previewControl);
		}

		bool RmlDocumentEditorPage::dropInstance(db::Instance* instance, const ui::Point& position)
		{
			return false;
		}

		bool RmlDocumentEditorPage::handleCommand(const ui::Command& command)
		{
			bool result = true;

			if (command == L"RmlUi.Editor.Reload")
			{
				// todo: reload document
			}
			else
				result = false;

			return result;
		}

		void RmlDocumentEditorPage::handleDatabaseEvent(db::Database* database, const Guid& eventId)
		{
			bool shouldRedraw = false;

			if (m_resourceManager)
				shouldRedraw |= m_resourceManager->reload(eventId, false);

			if (eventId == m_document->getInstance(0)->getGuid())
			{
				Ref< RmlDocumentResource > rmlDocument = database->getObjectReadOnly< RmlDocumentResource >(m_document->getInstance(0)->getGuid());
				if (rmlDocument)
				{
					m_RmlDocument = rmlDocument;

					m_previewControl->setRmlDocument(m_RmlDocument);
					shouldRedraw |= true;
				}
			}

			if (shouldRedraw)
				m_previewControl->update();
		}


	}
}