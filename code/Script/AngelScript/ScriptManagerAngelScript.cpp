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
	namespace
	{

const wchar_t* getErrorString(int errorCode)
{
	switch (errorCode)
	{
	case asSUCCESS: return L"Success";
	case asERROR: return L"Error";
	case asINVALID_ARG: return L"Invalid argument";
	case asNO_FUNCTION: return L"No function";
	case asNOT_SUPPORTED: return L"Not supported";
	case asINVALID_NAME: return L"Invalid name";
	case asNAME_TAKEN: return L"Name already taken";
	case asINVALID_DECLARATION: return L"Invalid declaration";
	case asINVALID_OBJECT: return L"Invalid object";
	case asINVALID_TYPE: return L"Invalid type";
	case asALREADY_REGISTERED: return L"Already registered";
	case asMULTIPLE_FUNCTIONS: return L"Multiple functions";
	case asNO_MODULE: return L"No module";
	case asNO_GLOBAL_VAR: return L"No global variable";
	case asINVALID_CONFIGURATION: return L"Invalid configuration";
	case asINVALID_INTERFACE: return L"Invalid interface";
	case asCANT_BIND_ALL_FUNCTIONS: return L"Can't bind all functions";
	case asLOWER_ARRAY_DIMENSION_NOT_REGISTERED: return L"Lower array dimension not registered";
	case asWRONG_CONFIG_GROUP: return L"Wrong config group";
	case asCONFIG_GROUP_IS_IN_USE: return L"Config group is in use";
	case asILLEGAL_BEHAVIOUR_FOR_TYPE: return L"Illegal behaviour for type";
	case asWRONG_CALLING_CONV: return L"Wrong calling convention";
	case asBUILD_IN_PROGRESS: return L"Build in progress";
	case asINIT_GLOBAL_VARS_FAILED: return L"Init global vars failed";
	case asOUT_OF_MEMORY: return L"Out of memory";
	case asMODULE_IS_IN_USE: return L"Module is in use";
	default: return L"Unknown error";
	}
}

	}

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

	// Parse RTTI path (e.g., "traktor.subnamespace.ClassName")
	// Convert to AngelScript namespace format ("::" separator)
	std::wstring fullName = exportType.getName();
	std::string fullNameStr = wstombs(Utf8Encoding(), fullName);

	// Replace '.' with '::' for AngelScript namespace syntax
	size_t pos = 0;
	while ((pos = fullNameStr.find('.', pos)) != std::string::npos)
	{
		fullNameStr.replace(pos, 1, "::");
		pos += 2;
	}

	// Extract namespace and class name
	size_t lastSep = fullNameStr.find_last_of(':');
	std::string namespaceName;
	std::string className;

	if (lastSep != std::string::npos && lastSep > 0)
	{
		namespaceName = fullNameStr.substr(0, lastSep - 1); // -1 to skip the second ':'
		className = fullNameStr.substr(lastSep + 1);
	}
	else
	{
		className = fullNameStr;
	}

	// Set the namespace for registration
	int r = m_scriptEngine->SetDefaultNamespace(namespaceName.c_str());
	if (r < 0)
	{
		log::error << L"Failed to set namespace \"" << mbstows(namespaceName) << L"\" for class \"" << mbstows(className) << L"\": " << getErrorString(r) << L" (" << r << L")" << Endl;
		m_scriptEngine->SetDefaultNamespace(""); // Reset to global namespace
		return;
	}

	// Register the type as a reference type
	r = m_scriptEngine->RegisterObjectType(className.c_str(), 0, asOBJ_REF);
	if (r < 0)
	{
		log::error << L"Failed to register class \"" << mbstows(fullNameStr) << L"\": " << getErrorString(r) << L" (" << r << L")" << Endl;
		m_scriptEngine->SetDefaultNamespace(""); // Reset to global namespace
		return;
	}

	// TODO: Register behaviors, methods, properties, etc.

	// Get the type ID (must be done while still in the namespace)
	rc.typeId = m_scriptEngine->GetTypeIdByDecl(className.c_str());

	// Reset to global namespace
	m_scriptEngine->SetDefaultNamespace("");
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
