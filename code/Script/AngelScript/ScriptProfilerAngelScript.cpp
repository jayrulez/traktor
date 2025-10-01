/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <angelscript.h>
#include "Script/AngelScript/ScriptProfilerAngelScript.h"
#include "Script/AngelScript/ScriptManagerAngelScript.h"

namespace traktor::script
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.script.ScriptProfilerAngelScript", ScriptProfilerAngelScript, IScriptProfiler)

ScriptProfilerAngelScript::ScriptProfilerAngelScript(ScriptManagerAngelScript* scriptManager, asIScriptEngine* scriptEngine)
:	m_scriptManager(scriptManager)
,	m_scriptEngine(scriptEngine)
{
	m_timer.reset();
}

void ScriptProfilerAngelScript::addListener(IListener* listener)
{
	m_listeners.insert(listener);
}

void ScriptProfilerAngelScript::removeListener(IListener* listener)
{
	m_listeners.erase(listener);
}

void ScriptProfilerAngelScript::notifyCallEnter()
{
	// TODO: Implement profiler call tracking
}

void ScriptProfilerAngelScript::notifyCallLeave()
{
	// TODO: Implement profiler call tracking
}

}
