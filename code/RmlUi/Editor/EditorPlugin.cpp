/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Misc/ObjectStore.h"
#include "Core/Misc/SafeDestroy.h"
#include "Core/Settings/PropertyBoolean.h"
#include "Core/Settings/PropertyGroup.h"
#include "Editor/IEditor.h"
#include "RmlUi/Editor/EditorPlugin.h"
#include "RmlUi/Backend/RmlUi.h"

namespace traktor::rmlui
{
	using namespace traktor::editor;

	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.rmlui.EditorPlugin", 0, EditorPlugin, IEditorPlugin)

		bool EditorPlugin::create(editor::IEditor* editor, ui::Widget* parent, IEditorPageSite* site)
	{
		m_editor = editor;

		RmlUi::getInstance().Initialize(m_editor->getObjectStore()->get< render::IRenderSystem >());
		return true;
	}

	void EditorPlugin::destroy()
	{
		RmlUi::getInstance().Shutdown();
	}

	int32_t EditorPlugin::getOrdinal() const
	{
		return 0;
	}

	void EditorPlugin::getCommands(std::list< ui::Command >& outCommands) const
	{
	}

	bool EditorPlugin::handleCommand(const ui::Command& command, bool result)
	{
		return false;
	}

	void EditorPlugin::handleDatabaseEvent(db::Database* database, const Guid& eventId)
	{
	}

	void EditorPlugin::handleWorkspaceOpened()
	{
	}

	void EditorPlugin::handleWorkspaceClosed()
	{
	}

	void EditorPlugin::handleEditorClosed()
	{
	}

}
