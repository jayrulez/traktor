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
#include "RmlUi/Editor/RmlUiEditorPlugin.h"
#include "RmlUi/RmlUi.h"
#include "Resource/ResourceManager.h"
#include "Render/Resource/ShaderFactory.h"

namespace traktor::rmlui
{
	using namespace traktor::editor;

	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.rmlui.RmlUiEditorPlugin", 0, RmlUiEditorPlugin, IEditorPlugin)

		bool RmlUiEditorPlugin::create(editor::IEditor* editor, ui::Widget* parent, IEditorPageSite* site)
	{
		m_editor = editor;

		render::IRenderSystem* renderSystem = m_editor->getObjectStore()->get< render::IRenderSystem >();

		return RmlUi::getInstance().initialize(renderSystem);
	}

	void RmlUiEditorPlugin::destroy()
	{
		if (RmlUi::getInstance().isInitialized())
			RmlUi::getInstance().shutdown();
	}

	int32_t RmlUiEditorPlugin::getOrdinal() const
	{
		return 0;
	}

	void RmlUiEditorPlugin::getCommands(std::list< ui::Command >& outCommands) const
	{
	}

	bool RmlUiEditorPlugin::handleCommand(const ui::Command& command, bool result)
	{
		return false;
	}

	void RmlUiEditorPlugin::handleDatabaseEvent(db::Database* database, const Guid& eventId)
	{
	}

	void RmlUiEditorPlugin::handleWorkspaceOpened()
	{
	}

	void RmlUiEditorPlugin::handleWorkspaceClosed()
	{
	}

	void RmlUiEditorPlugin::handleEditorClosed()
	{
	}

}
