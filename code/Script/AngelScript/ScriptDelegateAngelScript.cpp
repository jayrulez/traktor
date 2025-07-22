/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "Core/Class/IRuntimeObject.h"
#include "Script/AngelScript/ScriptDelegateAngelScript.h"

#include "angelscript.h"
#include "Script/AngelScript/ScriptClassAngelScript.h"
#include "Script/AngelScript/ScriptContextAngelScript.h"

namespace traktor::script
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.script.ScriptDelegateAngelScript", ScriptDelegateAngelScript, IRuntimeObject)

ScriptDelegateAngelScript::ScriptDelegateAngelScript(ScriptContextAngelScript* scriptContext, asIScriptFunction* function)
	: m_scriptContext(scriptContext)
	, m_function(function)
{
	if (m_function)
		m_function->AddRef();
}

ScriptDelegateAngelScript::~ScriptDelegateAngelScript()
{
	if (m_function)
	{
		m_function->Release();
		m_function = nullptr;
	}
}

Any ScriptDelegateAngelScript::call(int32_t argc, const Any* argv)
{
	if (!m_scriptContext || !m_function)
		return Any();

	// Delegate the call to the script context
	return m_scriptContext->executeDelegate(this, argc, argv);
}

}