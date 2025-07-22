/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Script/AngelScript/ScriptObjectAngelScript.h"

#include "angelscript.h"
#include "Script/AngelScript/ScriptClassAngelScript.h"
#include "Script/AngelScript/ScriptManagerAngelScript.h"

namespace traktor::script
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.script.ScriptObjectAngelScript", ScriptObjectAngelScript, IRuntimeObject)

ScriptObjectAngelScript::ScriptObjectAngelScript(ScriptManagerAngelScript* scriptManager, asIScriptObject* scriptObject)
	: m_scriptManager(scriptManager)
	, m_scriptObject(scriptObject)
{
	if (m_scriptObject)
		m_scriptObject->AddRef();
}

ScriptObjectAngelScript::~ScriptObjectAngelScript()
{
	if (m_scriptObject)
	{
		m_scriptObject->Release();
		m_scriptObject = nullptr;
	}
}

Ref< const IRuntimeClass > ScriptObjectAngelScript::getRuntimeClass() const
{
	if (!m_scriptObject)
		return nullptr;

	// Get the type info from the AngelScript object
	asITypeInfo* typeInfo = m_scriptObject->GetObjectType();
	if (!typeInfo)
		return nullptr;

	// Create a ScriptClassAngelScript wrapper for the type info
	return ScriptClassAngelScript::createFromTypeInfo(m_scriptManager, typeInfo);
}

asITypeInfo* ScriptObjectAngelScript::getAngelScriptTypeInfo() const
{
	if (m_scriptObject)
		return m_scriptObject->GetObjectType();
	return nullptr;
}

}