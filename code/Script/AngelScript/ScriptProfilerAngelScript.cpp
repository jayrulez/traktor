/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Script/AngelScript/ScriptProfilerAngelScript.h"

#include "angelscript.h"
#include "Core/Misc/String.h"
#include "Core/Misc/TString.h"
#include "Core/Thread/Acquire.h"
#include "Script/AngelScript/ScriptManagerAngelScript.h"

namespace traktor::script
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.script.ScriptProfilerAngelScript", ScriptProfilerAngelScript, IScriptProfiler)

ScriptProfilerAngelScript::ScriptProfilerAngelScript(ScriptManagerAngelScript* scriptManager, asIScriptEngine* engine)
	: m_scriptManager(scriptManager)
	, m_engine(engine)
	, m_enabled(false)
	, m_capturing(false)
{
	m_timer.reset();
}

ScriptProfilerAngelScript::~ScriptProfilerAngelScript()
{
	clearProfilingCallbacks();
}

void ScriptProfilerAngelScript::addListener(IListener* listener)
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);
	m_listeners.insert(listener);

	// Enable profiling when first listener is added
	if (m_listeners.size() == 1)
		enable(true);
}

void ScriptProfilerAngelScript::removeListener(IListener* listener)
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);
	m_listeners.erase(listener);

	// Disable profiling when last listener is removed
	if (m_listeners.empty())
		enable(false);
}

void ScriptProfilerAngelScript::enable(bool enable)
{
	if (enable && !m_enabled)
	{
		setupProfilingCallbacks();
		m_enabled = true;
		m_capturing = true;
		m_timer.reset();
	}
	else if (!enable && m_enabled)
	{
		clearProfilingCallbacks();
		m_enabled = false;
		m_capturing = false;

		// Send final measurements to listeners
		sendMeasurements();

		// Clear profiling data
		m_profiles.clear();
		m_callStack.clear();
	}
}

void ScriptProfilerAngelScript::setupProfilingCallbacks()
{
	// Note: AngelScript doesn't have global function enter/leave callbacks
	// These would need to be set on individual contexts when they are created
	// in ScriptManagerAngelScript using context-specific callbacks
}

void ScriptProfilerAngelScript::clearProfilingCallbacks()
{
	// Clear any profiling callbacks if they were set
}

void ScriptProfilerAngelScript::enterFunctionCallback(asIScriptContext* ctx, void* obj)
{
	ScriptProfilerAngelScript* profiler = static_cast< ScriptProfilerAngelScript* >(obj);

	if (!profiler->m_capturing)
		return;

	T_ANONYMOUS_VAR(Acquire< Semaphore >)(profiler->m_lock);

	// Get the current function
	asIScriptFunction* func = ctx->GetFunction(0);
	if (!func)
		return;

	const double currentTime = profiler->getCurrentTime();
	std::wstring functionName = profiler->getFunctionName(func);

	// Notify listeners of function entry
	for (auto listener : profiler->m_listeners)
		listener->callEnter(Guid(), functionName); // Using empty GUID for now

	// Update call stack
	CallStackEntry& entry = profiler->m_callStack.push_back();
	entry.function = func;
	entry.enterTime = currentTime;
	entry.childTime = 0.0;

	// Update function profile
	FunctionProfile& profile = profiler->m_profiles[func];
	if (profile.functionName.empty())
	{
		// First time seeing this function - initialize profile
		profile.functionName = functionName;
		profile.fileName = profiler->getFileName(func);
		profile.line = profiler->getLineNumber(func);
	}

	profile.callCount++;
	profile.lastEnterTime = currentTime;
	profile.recursionDepth++;
}

void ScriptProfilerAngelScript::leaveFunctionCallback(asIScriptContext* ctx, void* obj)
{
	ScriptProfilerAngelScript* profiler = static_cast< ScriptProfilerAngelScript* >(obj);

	if (!profiler->m_capturing)
		return;

	T_ANONYMOUS_VAR(Acquire< Semaphore >)(profiler->m_lock);

	if (profiler->m_callStack.empty())
		return;

	// Get the current function
	asIScriptFunction* func = ctx->GetFunction(0);
	if (!func)
		return;

	const double currentTime = profiler->getCurrentTime();
	std::wstring functionName = profiler->getFunctionName(func);

	// Notify listeners of function exit
	for (auto listener : profiler->m_listeners)
		listener->callLeave(Guid(), functionName); // Using empty GUID for now

	// Pop from call stack
	CallStackEntry entry = profiler->m_callStack.back();
	profiler->m_callStack.pop_back();

	if (entry.function != func)
	{
		// This shouldn't happen but handle gracefully
		return;
	}

	// Calculate timing
	const double functionTime = currentTime - entry.enterTime;
	const double exclusiveTime = functionTime - entry.childTime;

	// Update function profile
	auto it = profiler->m_profiles.find(func);
	if (it != profiler->m_profiles.end())
	{
		FunctionProfile& profile = it->second;
		profile.inclusiveTime += functionTime;
		profile.exclusiveTime += exclusiveTime;
		profile.recursionDepth--;
	}

	// Add this function's time to parent's child time
	if (!profiler->m_callStack.empty())
		profiler->m_callStack.back().childTime += functionTime;
}

void ScriptProfilerAngelScript::sendMeasurements()
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

	for (const auto& pair : m_profiles)
	{
		const FunctionProfile& profile = pair.second;

		if (profile.callCount > 0)
			for (auto listener : m_listeners)
				listener->callMeasured(
					Guid(), // Using empty GUID for now
					profile.functionName,
					profile.callCount,
					profile.inclusiveTime,
					profile.exclusiveTime);
	}
}

double ScriptProfilerAngelScript::getCurrentTime() const
{
	return m_timer.getElapsedTime();
}

std::wstring ScriptProfilerAngelScript::getFunctionName(asIScriptFunction* func) const
{
	if (!func)
		return L"<unknown>";

	const char* name = func->GetName();
	return mbstows(name ? name : "<anonymous>");
}

std::wstring ScriptProfilerAngelScript::getFileName(asIScriptFunction* func) const
{
	if (!func)
		return L"<unknown>";

	const char* scriptSection = func->GetScriptSectionName();
	return mbstows(scriptSection ? scriptSection : "<unknown>");
}

int32_t ScriptProfilerAngelScript::getLineNumber(asIScriptFunction* func) const
{
	if (!func)
		return 0;

	// AngelScript doesn't provide direct access to function line numbers
	// This would need to be tracked during compilation or retrieved differently
	return 0;
}

}