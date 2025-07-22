/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Script/AngelScript/ScriptDebuggerAngelScript.h"

#include "angelscript.h"
#include "Core/Class/Boxed.h"
#include "Core/Class/Any.h"
#include "Core/Guid.h"
#include "Core/Math/Format.h"
#include "Core/Misc/String.h"
#include "Core/Misc/TString.h"
#include "Core/Thread/Acquire.h"
#include "Core/Thread/Thread.h"
#include "Core/Thread/ThreadManager.h"
#include "Script/AngelScript/ScriptManagerAngelScript.h"
#include "Script/StackFrame.h"
#include "Script/Value.h"
#include "Script/ValueObject.h"
#include "Script/Variable.h"

namespace traktor::script
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.script.ScriptDebuggerAngelScript", ScriptDebuggerAngelScript, IScriptDebugger)

ScriptDebuggerAngelScript::ScriptDebuggerAngelScript(ScriptManagerAngelScript* scriptManager, asIScriptEngine* engine)
	: m_scriptManager(scriptManager)
	, m_engine(engine)
	, m_state(State::Running)
{
	setupDebugCallbacks();
}

ScriptDebuggerAngelScript::~ScriptDebuggerAngelScript()
{
	clearDebugCallbacks();
}

bool ScriptDebuggerAngelScript::setBreakpoint(const std::wstring& fileName, int32_t lineNumber)
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);
	m_breakpoints[lineNumber].insert(fileName);
	return true;
}

bool ScriptDebuggerAngelScript::removeBreakpoint(const std::wstring& fileName, int32_t lineNumber)
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);
	m_breakpoints[lineNumber].erase(fileName);
	return true;
}

bool ScriptDebuggerAngelScript::captureStackFrame(uint32_t depth, Ref< StackFrame >& outStackFrame)
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

	// Get the active context
	asIScriptContext* ctx = asGetActiveContext();
	if (!ctx)
		return false;

	if (depth >= ctx->GetCallstackSize())
		return false;

	asIScriptFunction* func = ctx->GetFunction(depth);
	if (!func)
		return false;

	const char* sectionName;
	int line = ctx->GetLineNumber(depth, nullptr, &sectionName);

	outStackFrame = new StackFrame();
	outStackFrame->setFileName(mbstows(sectionName ? sectionName : ""));
	outStackFrame->setFunctionName(mbstows(func->GetName()));
	outStackFrame->setLine(max(line - 1, 0));

	return true;
}

bool ScriptDebuggerAngelScript::captureLocals(uint32_t depth, RefArray< Variable >& outLocals)
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

	// Get the active context
	asIScriptContext* ctx = asGetActiveContext();
	if (!ctx)
		return false;

	if (depth >= ctx->GetCallstackSize())
		return false;

	// Get variables at the specified stack level
	int varCount = ctx->GetVarCount(depth);
	for (int i = 0; i < varCount; ++i)
	{
		const char* varName;
		int typeId;
		asETypeModifiers typeModifiers;

		if (ctx->GetVar(i, depth, &varName, &typeId, &typeModifiers) < 0)
			continue;

		if (!varName)
			continue;

		Ref< Variable > variable = new Variable(mbstows(varName), L"", 0);

		// Get variable value
		void* varPtr = ctx->GetAddressOfVar(i, depth);
		if (varPtr)
		{
			// Set type name
			variable->setTypeName(getTypeName(typeId));

			// Convert the value based on type
			if (typeId == asTYPEID_BOOL)
			{
				variable->setValue(new Value(*(bool*)varPtr ? L"true" : L"false"));
			}
			else if (typeId == asTYPEID_INT32)
			{
				variable->setValue(new Value(toString(*(int32_t*)varPtr)));
			}
			else if (typeId == asTYPEID_INT64)
			{
				variable->setValue(new Value(toString(*(int64_t*)varPtr)));
			}
			else if (typeId == asTYPEID_FLOAT)
			{
				variable->setValue(new Value(toString(*(float*)varPtr)));
			}
			else if (typeId == asTYPEID_DOUBLE)
			{
				variable->setValue(new Value(toString(*(double*)varPtr)));
			}
			else if (typeId == m_engine->GetTypeIdByDecl("string"))
			{
				std::string* str = static_cast< std::string* >(varPtr);
				variable->setValue(new Value(mbstows(*str)));
			}
			else if (typeId & asTYPEID_OBJHANDLE)
			{
				// Object handle
				void* obj = *(void**)varPtr;
				if (obj != nullptr)
				{
					// Create a ValueObject that can be expanded later
					variable->setValue(new ValueObject(0, L"<object>"));
				}
				else
				{
					variable->setValue(new Value(L"<null>"));
				}
			}
			else
			{
				variable->setValue(new Value(L"<unknown>"));
			}
		}

		outLocals.push_back(variable);
	}

	return true;
}

bool ScriptDebuggerAngelScript::captureObject(uint32_t object, RefArray< Variable >& outMembers)
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

	// AngelScript object introspection would need to be implemented here
	// This would require storing object references and implementing property access
	// For now, return false as this is complex to implement without more context
	return false;
}

bool ScriptDebuggerAngelScript::captureBreadcrumbs(AlignedVector< uint32_t >& outBreadcrumbs)
{
	outBreadcrumbs.resize(m_breadcrumb.size());
	for (uint32_t i = 0; i < m_breadcrumb.size(); ++i)
		outBreadcrumbs[i] = m_breadcrumb[i];
	return true;
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
	return false;
}

bool ScriptDebuggerAngelScript::actionStepOver()
{
	m_state = State::StepOver;
	return false;
}

void ScriptDebuggerAngelScript::addListener(IListener* listener)
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);
	m_listeners.insert(listener);
}

void ScriptDebuggerAngelScript::removeListener(IListener* listener)
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);
	m_listeners.erase(listener);

	// If all listeners have been removed then automatically continue running
	if (m_listeners.empty())
		m_state = State::Running;
}

void ScriptDebuggerAngelScript::setupDebugCallbacks()
{
	// Note: AngelScript doesn't have a global line callback like Lua
	// Instead, you need to set line callbacks on individual contexts
	// This would typically be done when creating contexts in the script manager
}

void ScriptDebuggerAngelScript::clearDebugCallbacks()
{
	// Clear any debug callbacks if they were set
}

void ScriptDebuggerAngelScript::lineCallback(asIScriptContext* ctx, void* obj)
{
	ScriptDebuggerAngelScript* debugger = static_cast< ScriptDebuggerAngelScript* >(obj);
	debugger->analyzeState(ctx);
}

void ScriptDebuggerAngelScript::analyzeState(asIScriptContext* ctx)
{
	const char* sectionName;
	int line = ctx->GetLineNumber(0, nullptr, &sectionName);
	if (line < 0)
		return;

	const int32_t currentLine = line - 1;
	m_breadcrumb.push_back(currentLine);

	if (m_state == State::Running)
	{
		T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

		// Check for breakpoint
		auto it = m_breakpoints.find(currentLine);
		if (it != m_breakpoints.end())
		{
			std::wstring currentFile = mbstows(sectionName ? sectionName : "");

			if (it->second.find(currentFile) != it->second.end())
			{
				m_state = State::Halted;
				m_lastFile = currentFile;

				for (auto listener : m_listeners)
					listener->debugeeStateChange(this);
			}
		}
	}
	else if (m_state == State::Break)
	{
		T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

		std::wstring currentFile = mbstows(sectionName ? sectionName : "");
		m_state = State::Halted;
		m_lastFile = currentFile;

		for (auto listener : m_listeners)
			listener->debugeeStateChange(this);
	}
	else if (m_state == State::StepInto)
	{
		T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

		std::wstring currentFile = mbstows(sectionName ? sectionName : "");
		m_state = State::Halted;
		m_lastFile = currentFile;

		for (auto listener : m_listeners)
			listener->debugeeStateChange(this);
	}
	else if (m_state == State::StepOver)
	{
		T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

		std::wstring currentFile = mbstows(sectionName ? sectionName : "");

		if (currentFile == m_lastFile)
		{
			m_state = State::Halted;
			m_lastFile = currentFile;

			for (auto listener : m_listeners)
				listener->debugeeStateChange(this);
		}
	}

	if (m_state == State::Halted)
	{
		// Wait until state is no longer halted
		Thread* currentThread = ThreadManager::getInstance().getCurrentThread();
		while (m_state == State::Halted && !m_listeners.empty() && !currentThread->stopped())
			currentThread->sleep(100);

		if (m_state == State::Halted)
			m_state = State::Running;

		m_breadcrumb.clear();

		for (auto listener : m_listeners)
			listener->debugeeStateChange(this);
	}
}

Any ScriptDebuggerAngelScript::convertAngelScriptValueToAny(void* value, int typeId) const
{
	if (typeId == asTYPEID_BOOL)
		return Any::fromBoolean(*(bool*)value);
	else if (typeId == asTYPEID_INT32)
		return Any::fromInt32(*(int32_t*)value);
	else if (typeId == asTYPEID_INT64)
		return Any::fromInt64(*(int64_t*)value);
	else if (typeId == asTYPEID_FLOAT)
		return Any::fromFloat(*(float*)value);
	else if (typeId == asTYPEID_DOUBLE)
		return Any::fromDouble(*(double*)value);
	else if (typeId == m_engine->GetTypeIdByDecl("string"))
	{
		std::string* str = static_cast< std::string* >(value);
		std::wstring wstr = mbstows(*str);
		return Any::fromString(wstr);
	}
	else if (typeId & asTYPEID_OBJHANDLE)
	{
		ITypedObject* obj = static_cast< ITypedObject* >(*(void**)value);
		return Any::fromObject(obj);
	}

	return Any();
}

std::wstring ScriptDebuggerAngelScript::getTypeName(int typeId) const
{
	if (typeId == asTYPEID_BOOL)
		return L"bool";
	else if (typeId == asTYPEID_INT32)
		return L"int";
	else if (typeId == asTYPEID_INT64)
		return L"int64";
	else if (typeId == asTYPEID_FLOAT)
		return L"float";
	else if (typeId == asTYPEID_DOUBLE)
		return L"double";
	else if (typeId == m_engine->GetTypeIdByDecl("string"))
		return L"string";
	else if (typeId & asTYPEID_OBJHANDLE)
	{
		const char* typeName = m_engine->GetTypeDeclaration(typeId);
		return mbstows(typeName ? typeName : "object");
	}

	return L"unknown";
}

}