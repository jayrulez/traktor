/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <angelscript.h>
#include "Script/AngelScript/ScriptObjectAngelScript.h"
#include "Script/AngelScript/ScriptManagerAngelScript.h"
#include "Script/AngelScript/ScriptContextAngelScript.h"
#include "Script/AngelScript/ScriptClassAngelScript.h"

namespace traktor::script
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.script.ScriptObjectAngelScript", ScriptObjectAngelScript, IRuntimeObject)

ScriptObjectAngelScript::ScriptObjectAngelScript(ScriptManagerAngelScript* scriptManager, ScriptContextAngelScript* scriptContext, asIScriptObject* scriptObject)
:	m_scriptManager(scriptManager)
,	m_scriptContext(scriptContext)
,	m_scriptObject(scriptObject)
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

	asITypeInfo* typeInfo = m_scriptObject->GetObjectType();
	if (!typeInfo)
		return nullptr;

	return ScriptClassAngelScript::createFromTypeInfo(m_scriptManager, m_scriptContext, typeInfo);
}

}
