/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Editor/IEditor.h"
#include "Editor/IEditorPageSite.h"
#include "Paper/Editor/PaperEditorPlugin.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.PaperEditorPlugin", 0, PaperEditorPlugin, editor::IEditorPlugin)

bool PaperEditorPlugin::create(editor::IEditor* editor, ui::Widget* parent, editor::IEditorPageSite* site)
{
	m_editor = editor;
	m_site = site;
	return true;
}

void PaperEditorPlugin::destroy()
{
}

int32_t PaperEditorPlugin::getOrdinal() const
{
	return 0;
}

void PaperEditorPlugin::getCommands(std::list< ui::Command >& outCommands) const
{
}

bool PaperEditorPlugin::handleCommand(const ui::Command& command, bool result)
{
	return false;
}

void PaperEditorPlugin::handleDatabaseEvent(db::Database* database, const Guid& eventId)
{
}

void PaperEditorPlugin::handleWorkspaceOpened()
{
}

void PaperEditorPlugin::handleWorkspaceClosed()
{
}

void PaperEditorPlugin::handleEditorClosed()
{
}

}
