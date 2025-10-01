/*
 * TRAKTOR
 * Copyright (c) 2022-2025 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Containers/CircularVector.h"
#include "Core/Containers/SmallMap.h"
#include "Core/Containers/SmallSet.h"
#include "Core/Thread/Semaphore.h"
#include "Script/IScriptDebugger.h"

class asIScriptEngine;
class asIScriptContext;

namespace traktor::script
{

class ScriptManagerAngelScript;

/*! AngelScript script debugger
 * \ingroup Script
 */
class ScriptDebuggerAngelScript : public IScriptDebugger
{
	T_RTTI_CLASS;

public:
	explicit ScriptDebuggerAngelScript(ScriptManagerAngelScript* scriptManager, asIScriptEngine* scriptEngine);

	virtual ~ScriptDebuggerAngelScript();

	virtual bool setBreakpoint(const std::wstring& fileName, int32_t lineNumber) override final;

	virtual bool removeBreakpoint(const std::wstring& fileName, int32_t lineNumber) override final;

	virtual bool captureStackFrame(uint32_t depth, Ref< StackFrame >& outStackFrame) override final;

	virtual bool captureLocals(uint32_t depth, RefArray< Variable >& outLocals) override final;

	virtual bool captureObject(uint32_t object, RefArray< Variable >& outMembers) override final;

	virtual bool captureBreadcrumbs(AlignedVector< uint32_t >& outBreadcrumbs) override final;

	virtual bool isRunning() const override final;

	virtual bool actionBreak() override final;

	virtual bool actionContinue() override final;

	virtual bool actionStepInto() override final;

	virtual bool actionStepOver() override final;

	virtual void addListener(IListener* listener) override final;

	virtual void removeListener(IListener* listener) override final;

private:
	friend class ScriptManagerAngelScript;

	enum class State
	{
		Running,
		Halted,
		Break,
		StepInto,
		StepOver
	};

	ScriptManagerAngelScript* m_scriptManager;
	asIScriptEngine* m_scriptEngine;
	Semaphore m_lock;
	SmallMap< int32_t, SmallSet< std::wstring > > m_breakpoints;
	SmallSet< IListener* > m_listeners;
	CircularVector< int32_t, 32 > m_breadcrumb;
	std::wstring m_lastFile;
	State m_state;
};

}
