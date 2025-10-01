/*
 * TRAKTOR
 * Copyright (c) 2022-2025 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <angelscript.h>
#include "Script/AngelScript/ScriptDebuggerAngelScript.h"
#include "Script/AngelScript/ScriptManagerAngelScript.h"

namespace traktor::script
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.script.ScriptDebuggerAngelScript", ScriptDebuggerAngelScript, IScriptDebugger)

ScriptDebuggerAngelScript::ScriptDebuggerAngelScript(ScriptManagerAngelScript* scriptManager, asIScriptEngine* scriptEngine)
:	m_scriptManager(scriptManager)
,	m_scriptEngine(scriptEngine)
,	m_state(State::Running)
{
}

ScriptDebuggerAngelScript::~ScriptDebuggerAngelScript()
{
}

bool ScriptDebuggerAngelScript::setBreakpoint(const std::wstring& fileName, int32_t lineNumber)
{
	// TODO: Implement breakpoint support
	return false;
}

bool ScriptDebuggerAngelScript::removeBreakpoint(const std::wstring& fileName, int32_t lineNumber)
{
	// TODO: Implement breakpoint support
	return false;
}

bool ScriptDebuggerAngelScript::captureStackFrame(uint32_t depth, Ref< StackFrame >& outStackFrame)
{
	// TODO: Implement stack frame capture
	return false;
}

bool ScriptDebuggerAngelScript::captureLocals(uint32_t depth, RefArray< Variable >& outLocals)
{
	// TODO: Implement local variable capture
	return false;
}

bool ScriptDebuggerAngelScript::captureObject(uint32_t object, RefArray< Variable >& outMembers)
{
	// TODO: Implement object member capture
	return false;
}

bool ScriptDebuggerAngelScript::captureBreadcrumbs(AlignedVector< uint32_t >& outBreadcrumbs)
{
	// TODO: Implement breadcrumb capture
	return false;
}

bool ScriptDebuggerAngelScript::isRunning() const
{
	return m_state == State::Running;
}

bool ScriptDebuggerAngelScript::actionBreak()
{
	m_state = State::Break;
	return true;
}

bool ScriptDebuggerAngelScript::actionContinue()
{
	m_state = State::Running;
	return true;
}

bool ScriptDebuggerAngelScript::actionStepInto()
{
	m_state = State::StepInto;
	return true;
}

bool ScriptDebuggerAngelScript::actionStepOver()
{
	m_state = State::StepOver;
	return true;
}

void ScriptDebuggerAngelScript::addListener(IListener* listener)
{
	m_listeners.insert(listener);
}

void ScriptDebuggerAngelScript::removeListener(IListener* listener)
{
	m_listeners.erase(listener);
}

}
