/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <angelscript.h>
#include "Core/Class/IRuntimeClass.h"
#include "Core/Class/IRuntimeDispatch.h"
#include "Core/Io/Utf8Encoding.h"
#include "Core/Log/Log.h"
#include "Core/Misc/String.h"
#include "Core/Misc/TString.h"
#include "Core/Thread/Acquire.h"
#include "Script/AngelScript/ScriptContextAngelScript.h"
#include "Script/AngelScript/ScriptDebuggerAngelScript.h"
#include "Script/AngelScript/ScriptManagerAngelScript.h"
#include "Script/AngelScript/ScriptProfilerAngelScript.h"
#include "Script/AngelScript/ScriptUtilitiesAngelScript.h"

namespace traktor::script
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.script.ScriptManagerAngelScript", 0, ScriptManagerAngelScript, IScriptManager)

ScriptManagerAngelScript* ScriptManagerAngelScript::ms_instance = nullptr;

ScriptManagerAngelScript::ScriptManagerAngelScript()
:	m_scriptEngine(nullptr)
,	m_lockContext(nullptr)
,	m_totalMemoryUse(0)
{
	T_FATAL_ASSERT(ms_instance == nullptr);
	ms_instance = this;

	m_scriptEngine = asCreateScriptEngine();
	T_FATAL_ASSERT(m_scriptEngine != nullptr);

	// Set message callback to receive compilation and execution errors
	// TODO: Implement message callback

	// Register standard functions
	m_scriptEngine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(asPrint), asCALL_GENERIC);
	m_scriptEngine->RegisterGlobalFunction("void sleep(int)", asFUNCTION(asSleep), asCALL_GENERIC);
}

ScriptManagerAngelScript::~ScriptManagerAngelScript()
{
	T_FATAL_ASSERT_M(!m_scriptEngine, L"Must call destroy");
	T_FATAL_ASSERT(ms_instance == this);
	ms_instance = nullptr;
}

void ScriptManagerAngelScript::destroy()
{
	if (!m_scriptEngine)
		return;

	T_ANONYMOUS_VAR(Ref< ScriptManagerAngelScript >)(this);
	T_FATAL_ASSERT(m_contexts.empty());

	asIScriptEngine* scriptEngine = m_scriptEngine;
	m_scriptEngine = nullptr;

	// Discard all tags from C++ rtti types
	for (auto& rc : m_classRegistry)
	{
		for (auto& derivedType : rc.runtimeClass->getExportType().findAllOf())
			derivedType->setTag(0);
	}

	m_debugger = nullptr;
	m_profiler = nullptr;

	scriptEngine->ShutDownAndRelease();
}

void ScriptManagerAngelScript::registerClass(IRuntimeClass* runtimeClass)
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

	const TypeInfo& exportType = runtimeClass->getExportType();
	const int32_t classRegistryIndex = int32_t(m_classRegistry.size());

	RegisteredClass& rc = m_classRegistry.push_back();
	rc.runtimeClass = runtimeClass;

	// TODO: Implement full class registration
	// AngelScript requires explicit registration of:
	// - Type itself (RegisterObjectType)
	// - Constructor (RegisterObjectBehaviour)
	// - Methods (RegisterObjectMethod)
	// - Properties (RegisterObjectProperty)
	// - Operators (RegisterObjectMethod with operator overloads)

	const std::string className = wstombs(Utf8Encoding(), exportType.getName());

	// Register the type as a reference type
	m_scriptEngine->RegisterObjectType(className.c_str(), 0, asOBJ_REF);

	// TODO: Register behaviors, methods, properties, etc.

	rc.typeId = m_scriptEngine->GetTypeIdByDecl(className.c_str());
}

Ref< IScriptContext > ScriptManagerAngelScript::createContext(bool strict)
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

	// Create a unique module for this context
	static int32_t s_moduleCounter = 0;
	std::string moduleName = "Module_" + wstombs(toString(s_moduleCounter++));

	asIScriptModule* module = m_scriptEngine->GetModule(moduleName.c_str(), asGM_ALWAYS_CREATE);
	if (!module)
	{
		log::error << L"Failed to create AngelScript module" << Endl;
		return nullptr;
	}

	asIScriptContext* context = m_scriptEngine->CreateContext();
	if (!context)
	{
		log::error << L"Failed to create AngelScript context" << Endl;
		return nullptr;
	}

	Ref< ScriptContextAngelScript > scriptContext = new ScriptContextAngelScript(this, module, context, strict);
	m_contexts.push_back(scriptContext);
	return scriptContext;
}

Ref< IScriptDebugger > ScriptManagerAngelScript::createDebugger()
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

	if (!m_debugger)
		m_debugger = new ScriptDebuggerAngelScript(this, m_scriptEngine);

	return m_debugger;
}

Ref< IScriptProfiler > ScriptManagerAngelScript::createProfiler()
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

	if (!m_profiler)
		m_profiler = new ScriptProfilerAngelScript(this, m_scriptEngine);

	return m_profiler;
}

void ScriptManagerAngelScript::collectGarbage(bool full)
{
	if (!full)
		collectGarbagePartial();
	else
		collectGarbageFull();
}

void ScriptManagerAngelScript::getStatistics(ScriptStatistics& outStatistics) const
{
	outStatistics.memoryUsage = uint32_t(m_totalMemoryUse);
}

void ScriptManagerAngelScript::pushObject(ITypedObject* object)
{
	// TODO: Implement object pushing to AngelScript stack
}

void ScriptManagerAngelScript::pushAny(const Any& any)
{
	// TODO: Implement Any to AngelScript type conversion
}

void ScriptManagerAngelScript::pushAny(const Any* anys, int32_t count)
{
	for (int32_t i = 0; i < count; ++i)
		pushAny(anys[i]);
}

Any ScriptManagerAngelScript::toAny(int32_t index)
{
	// TODO: Implement AngelScript type to Any conversion
	return Any();
}

void ScriptManagerAngelScript::toAny(int32_t base, int32_t count, Any* outAnys)
{
	for (int32_t i = 0; i < count; ++i)
		outAnys[i] = toAny(base + i);
}

void ScriptManagerAngelScript::destroyContext(ScriptContextAngelScript* context)
{
	m_contexts.remove(context);
}

void ScriptManagerAngelScript::collectGarbageFull()
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);
	m_scriptEngine->GarbageCollect(asGC_FULL_CYCLE);
}

void ScriptManagerAngelScript::collectGarbagePartial()
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);
	m_scriptEngine->GarbageCollect(asGC_ONE_STEP);
}

}
