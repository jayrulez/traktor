/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Editor/IDocument.h"
#include "Editor/IEditor.h"
#include "Paper/Editor/UIPageEditorPage.h"
#include "Paper/Editor/UIPagePreviewControl.h"
#include "Ui/Container.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.paper.UIPageEditorPage", UIPageEditorPage, editor::IEditorPage)

UIPageEditorPage::UIPageEditorPage(editor::IEditor* editor, editor::IEditorPageSite* site, editor::IDocument* document)
:	m_editor(editor)
,	m_site(site)
,	m_document(document)
{
}

bool UIPageEditorPage::create(ui::Container* parent)
{
	m_previewControl = new UIPagePreviewControl();
	if (!m_previewControl->create(parent))
		return false;

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
	return false;
}

void UIPageEditorPage::handleDatabaseEvent(db::Database* database, const Guid& eventId)
{
}

}
