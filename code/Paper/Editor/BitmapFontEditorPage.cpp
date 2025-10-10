/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Paper/Editor/BitmapFontEditorPage.h"

#include "Core/Io/FileSystem.h"
#include "Core/Log/Log.h"
#include "Database/Database.h"
#include "Database/Instance.h"
#include "Editor/IDocument.h"
#include "Editor/IEditor.h"
#include "Paper/BitmapFont/BitmapFont.h"
#include "Paper/Editor/BitmapFontAsset.h"
#include "Paper/Editor/BitmapFontPreviewControl.h"
#include "Ui/Container.h"
#include "Ui/Events/ContentChangeEvent.h"
#include "Ui/RichEdit/RichEdit.h"
#include "Ui/TableLayout.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.paper.BitmapFontEditorPage", BitmapFontEditorPage, editor::IEditorPage)

BitmapFontEditorPage::BitmapFontEditorPage(editor::IEditor* editor, editor::IEditorPageSite* site, editor::IDocument* document)
:	m_editor(editor)
,	m_site(site)
,	m_document(document)
{
}

bool BitmapFontEditorPage::create(ui::Container* parent)
{
	Ref< ui::Container > container = new ui::Container();
	if (!container->create(parent, ui::WsNone, new ui::TableLayout(L"100%", L"100,100%", ui::Unit(0), ui::Unit(4))))
		return false;

	// Create text input for preview text.
	m_editPreviewText = new ui::RichEdit();
	if (!m_editPreviewText->create(container, L"The quick brown fox jumps over the lazy dog.\nABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789"))
		return false;

	m_editPreviewText->addEventHandler< ui::ContentChangeEvent >(this, &BitmapFontEditorPage::eventTextChange);

	// Create preview control.
	m_previewControl = new BitmapFontPreviewControl(m_editor);
	if (!m_previewControl->create(container))
		return false;

	// Initial update to load the font.
	updatePreview();

	// Set initial preview text
	if (m_previewControl && m_editPreviewText)
		m_previewControl->setPreviewText(m_editPreviewText->getText());

	return true;
}

void BitmapFontEditorPage::destroy()
{
	if (m_previewControl)
	{
		m_previewControl->destroy();
		m_previewControl = nullptr;
	}

	if (m_editPreviewText)
	{
		m_editPreviewText->destroy();
		m_editPreviewText = nullptr;
	}
}

bool BitmapFontEditorPage::dropInstance(db::Instance* instance, const ui::Point& position)
{
	return false;
}

bool BitmapFontEditorPage::handleCommand(const ui::Command& command)
{
	return false;
}

void BitmapFontEditorPage::handleDatabaseEvent(db::Database* database, const Guid& eventId)
{
	// If the font resource was updated, reload it.
	if (eventId == m_fontGuid)
		updatePreview();
}

void BitmapFontEditorPage::updatePreview()
{
	if (!m_previewControl)
		return;

	// Get the source instance guid.
	Ref< db::Instance > sourceInstance = m_document->getInstance(0);
	if (!sourceInstance)
		return;

	m_fontGuid = sourceInstance->getGuid();

	// Try to load the built font from the output database.
	Ref< db::Database > outputDatabase = m_editor->getOutputDatabase();
	if (!outputDatabase)
		return;

	Ref< BitmapFont > font = outputDatabase->getObjectReadOnly< BitmapFont >(m_fontGuid);
	if (!font)
	{
		log::info << L"Font not yet built; preview unavailable." << Endl;
		return;
	}

	m_previewControl->setBitmapFont(font);
}

void BitmapFontEditorPage::eventTextChange(ui::ContentChangeEvent* event)
{
	if (m_previewControl && m_editPreviewText)
		m_previewControl->setPreviewText(m_editPreviewText->getText());
}

}
