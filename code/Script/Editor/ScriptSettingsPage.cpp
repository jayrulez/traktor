/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Misc/String.h"
#include "Core/Settings/PropertyBoolean.h"
#include "Core/Settings/PropertyGroup.h"
#include "I18N/Text.h"
#include "Script/Editor/ScriptSettingsPage.h"
#include "Ui/CheckBox.h"
#include "Ui/Container.h"
#include "Ui/TableLayout.h"

namespace traktor::script
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.script.ScriptSettingsPage", 0, ScriptSettingsPage, editor::ISettingsPage)

bool ScriptSettingsPage::create(ui::Container* parent, const PropertyGroup* originalSettings, PropertyGroup* settings, const std::list< ui::Command >& shortcutCommands)
{
	Ref< ui::Container > container = new ui::Container();
	if (!container->create(parent, ui::WsNone, new ui::TableLayout(L"100%", L"*", 0_ut, 4_ut)))
		return false;

	m_checkAutoOpenDebugger = new ui::CheckBox();
	m_checkAutoOpenDebugger->create(container, i18n::Text(L"EDITOR_SETTINGS_SCRIPT_AUTO_OPEN_DEBUGGER"), false);
	m_checkAutoOpenDebugger->setChecked(settings->getProperty< bool >(L"Editor.AutoOpenDebugger", false));

	m_checkAutoOpenScript = new ui::CheckBox();
	m_checkAutoOpenScript->create(container, i18n::Text(L"EDITOR_SETTINGS_SCRIPT_AUTO_OPEN_SCRIPT"), false);
	m_checkAutoOpenScript->setChecked(settings->getProperty< bool >(L"Editor.AutoOpenDebuggedScript", true));

	m_checkAutocompleteEnabled = new ui::CheckBox();
	m_checkAutocompleteEnabled->create(container, i18n::Text(L"EDITOR_SETTINGS_SCRIPT_AUTOCOMPLETE_ENABLED"), false);
	m_checkAutocompleteEnabled->setChecked(settings->getProperty< bool >(L"Editor.AutocompleteEnabled", true));

	parent->setText(i18n::Text(L"EDITOR_SETTINGS_SCRIPT"));
	return true;
}

void ScriptSettingsPage::destroy()
{
}

bool ScriptSettingsPage::apply(PropertyGroup* settings)
{
	settings->setProperty< PropertyBoolean >(L"Editor.AutoOpenDebugger", m_checkAutoOpenDebugger->isChecked());
	settings->setProperty< PropertyBoolean >(L"Editor.AutoOpenDebuggedScript", m_checkAutoOpenScript->isChecked());
	settings->setProperty< PropertyBoolean >(L"Editor.AutocompleteEnabled", m_checkAutocompleteEnabled->isChecked());
	return true;
}

}
