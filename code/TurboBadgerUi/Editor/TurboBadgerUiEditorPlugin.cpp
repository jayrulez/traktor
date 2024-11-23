/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/Editor/TurboBadgerUiEditorPlugin.h"
#include "TurboBadgerUi/TurboBadgerUi.h"

#include "Core/Misc/ObjectStore.h"
#include "render/IRenderSystem.h"
#include "Editor/IEditor.h"

namespace traktor::turbobadgerui
{
	using namespace traktor::editor;

	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.turbobadgerui.TurboBadgerUiEditorPlugin", 0, TurboBadgerUiEditorPlugin, IEditorPlugin)

		bool TurboBadgerUiEditorPlugin::create(editor::IEditor* editor, ui::Widget* parent, IEditorPageSite* site)
	{
		m_editor = editor;

		render::IRenderSystem* renderSystem = m_editor->getObjectStore()->get< render::IRenderSystem >();

		return TurboBadgerUi::getInstance().initialize(renderSystem);
	}

	void TurboBadgerUiEditorPlugin::destroy()
	{
		TurboBadgerUi::getInstance().shutdown();
	}

	int32_t TurboBadgerUiEditorPlugin::getOrdinal() const
	{
		return 0;
	}

	void TurboBadgerUiEditorPlugin::getCommands(std::list< ui::Command >& outCommands) const
	{
	}

	bool TurboBadgerUiEditorPlugin::handleCommand(const ui::Command& command, bool result)
	{
		return false;
	}

	void TurboBadgerUiEditorPlugin::handleDatabaseEvent(db::Database* database, const Guid& eventId)
	{
	}

	void TurboBadgerUiEditorPlugin::handleWorkspaceOpened()
	{
	}

	void TurboBadgerUiEditorPlugin::handleWorkspaceClosed()
	{
	}

	void TurboBadgerUiEditorPlugin::handleEditorClosed()
	{
	}
}