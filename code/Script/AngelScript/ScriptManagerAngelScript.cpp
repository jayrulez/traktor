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
#include "Core/Io/StringOutputStream.h"
#include "Core/Io/Utf8Encoding.h"
#include "Core/Log/Log.h"
#include "Core/Misc/Split.h"
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

#if defined(T_NEED_RUNTIME_SIGNATURE)
std::string convertTraktorTypeToAngelScript(const std::wstring& traktorType)
{
	std::wstring type = traktorType;

	// Strip whitespace
	size_t start = type.find_first_not_of(L" \t");
	size_t end = type.find_last_not_of(L" \t");
	if (start != std::wstring::npos && end != std::wstring::npos)
		type = type.substr(start, end - start + 1);

	// Handle const qualifiers (strip for now, can add back if needed)
	if (type.find(L"const ") == 0)
		type = type.substr(6);

	// Check for pointer types (strip trailing *)
	bool isPointer = false;
	if (!type.empty() && type.back() == L'*')
	{
		isPointer = true;
		type = type.substr(0, type.length() - 1);
	}

	// Strip trailing & (reference qualifiers)
	if (!type.empty() && type.back() == L'&')
		type = type.substr(0, type.length() - 1);

	// Strip trailing whitespace again after removing qualifiers
	end = type.find_last_not_of(L" \t");
	if (end != std::wstring::npos)
		type = type.substr(0, end + 1);

	// Map primitive types
	if (type == L"void") return "void";
	if (type == L"bool") return "bool";
	if (type == L"int8" || type == L"int8_t") return "int8";
	if (type == L"int16" || type == L"int16_t") return "int16";
	if (type == L"int32" || type == L"int32_t" || type == L"int") return "int";
	if (type == L"int64" || type == L"int64_t") return "int64";
	if (type == L"uint8" || type == L"uint8_t") return "uint8";
	if (type == L"uint16" || type == L"uint16_t") return "uint16";
	if (type == L"uint32" || type == L"uint32_t" || type == L"uint") return "uint";
	if (type == L"uint64" || type == L"uint64_t") return "uint64";
	if (type == L"float") return "float";
	if (type == L"double") return "double";
	if (type == L"wchar_t") return "uint16";
	if (type == L"std::string" || type == L"std::wstring") return "string";

	// Handle Any type - used for marshalling between C++ and script
	if (type == L"Any" || type == L"traktor.Any") return "traktor::Any";

	// Handle template types like Ref< Type >, RefArray< Type >, etc.
	size_t templateStart = type.find(L'<');
	if (templateStart != std::wstring::npos)
	{
		size_t templateEnd = type.rfind(L'>');
		if (templateEnd != std::wstring::npos && templateEnd > templateStart)
		{
			std::wstring templateName = type.substr(0, templateStart);
			std::wstring innerType = type.substr(templateStart + 1, templateEnd - templateStart - 1);

			// Strip whitespace from template name and inner type
			size_t tstart = templateName.find_first_not_of(L" \t");
			size_t tend = templateName.find_last_not_of(L" \t");
			if (tstart != std::wstring::npos && tend != std::wstring::npos)
				templateName = templateName.substr(tstart, tend - tstart + 1);

			// For Ref<>, RefArray<>, etc., just extract and convert the inner type
			// These are smart pointer wrappers that should map to handles in AngelScript
			if (templateName == L"Ref" || templateName == L"RefArray" ||
			    templateName == L"AlignedVector" || templateName == L"SmallSet" ||
			    templateName == L"SmallMap")
			{
				// Recursively convert the inner type
				return convertTraktorTypeToAngelScript(innerType);
			}
		}
	}

	// Check if it's a Traktor type (contains '.')
	if (type.find(L'.') != std::wstring::npos)
	{
		// Convert "traktor.namespace.ClassName" to "traktor::namespace::ClassName"
		std::string typeStr = wstombs(type);

		// Replace '.' with '::'
		size_t pos = 0;
		while ((pos = typeStr.find('.', pos)) != std::string::npos)
		{
			typeStr.replace(pos, 1, "::");
			pos += 2;
		}

		// Return as handle (reference type in AngelScript)
		// Both pointers and Ref<> types become handles (@)
		return typeStr + "@";
	}

	// If it was a pointer to an unknown type, still make it a handle
	if (isPointer)
	{
		return wstombs(type) + "@";
	}

	// Unknown type, pass through as-is
	return wstombs(type);
}

AlignedVector< std::string > convertRuntimeSignaturesToAngelScript(const IRuntimeDispatch* dispatch, const std::string& methodName)
{
	AlignedVector< std::string > signatures;

	// Get runtime signature - may contain multiple overloads separated by ';'
	StringOutputStream ss;
	dispatch->signature(ss);
	std::wstring signatureWide = ss.str();

	// Split by semicolon to handle overloads
	AlignedVector< std::wstring > overloads;
	Split< std::wstring >::any(signatureWide, L";", overloads, true);

	for (const auto& overload : overloads)
	{
		// Split each overload by comma: "ReturnType,ArgType1,ArgType2,..."
		AlignedVector< std::wstring > parts;
		Split< std::wstring >::any(overload, L",", parts, true);

		if (parts.empty())
		{
			signatures.push_back("void " + methodName + "()");
			continue;
		}

		// First part is return type
		std::string returnType = convertTraktorTypeToAngelScript(parts[0]);

		// Build AngelScript signature: "ReturnType methodName(ArgType1, ArgType2, ...)"
		std::string signature = returnType + " " + methodName + "(";

		for (size_t i = 1; i < parts.size(); ++i)
		{
			if (i > 1)
				signature += ", ";
			signature += convertTraktorTypeToAngelScript(parts[i]);
		}

		signature += ")";
		signatures.push_back(signature);
	}

	return signatures;
}
#endif

void asGenericStaticMethodWrapper(asIScriptGeneric* gen)
{
	// Get the dispatch pointer from auxiliary data
	const IRuntimeDispatch* dispatch = static_cast<const IRuntimeDispatch*>(gen->GetAuxiliary());
	if (!dispatch)
		return;

	// TODO: Convert AngelScript arguments to Any[]
	// TODO: Call dispatch->invoke()
	// TODO: Set return value
}

void asMessageCallback(const asSMessageInfo* msg, void* param)
{
	const wchar_t* type = L"INFO";
	if (msg->type == asMSGTYPE_WARNING)
		type = L"WARN";
	else if (msg->type == asMSGTYPE_ERROR)
		type = L"ERROR";

	log::info << L"AngelScript [" << type << L"] " << mbstows(msg->section) << L" (" << msg->row << L", " << msg->col << L"): " << mbstows(msg->message) << Endl;
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
	m_scriptEngine->SetMessageCallback(asFUNCTION(asMessageCallback), this, asCALL_CDECL);

	// Register a dummy string type (std::string)
	// This is a placeholder until we implement proper string support
	int r = m_scriptEngine->RegisterObjectType("string", sizeof(std::string), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK);
	if (r < 0)
		log::warning << L"Failed to register string type: " << getErrorString(r) << L" (" << r << L")" << Endl;

	// Register Any type - used for marshalling values between C++ and script
	r = m_scriptEngine->SetDefaultNamespace("traktor");
	if (r >= 0)
	{
		r = m_scriptEngine->RegisterObjectType("Any", sizeof(Any), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK);
		if (r < 0)
			log::warning << L"Failed to register Any type: " << getErrorString(r) << L" (" << r << L")" << Endl;
		m_scriptEngine->SetDefaultNamespace("");
	}
	else
		log::warning << L"Failed to set namespace for Any type: " << getErrorString(r) << L" (" << r << L")" << Endl;

	// Register standard functions
	// Note: Temporarily commenting these out until string type is properly registered
	// m_scriptEngine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(asPrint), asCALL_GENERIC);
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

	// PHASE 1: Register only the type itself
	// This allows other types to reference this type in their methods/properties

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

	// Get the type ID (must be done while still in the namespace)
	rc.typeId = m_scriptEngine->GetTypeIdByDecl(className.c_str());

	// Reset to global namespace
	m_scriptEngine->SetDefaultNamespace("");

	// Members (methods, properties, etc.) will be registered in Phase 2 via completeRegistration()
}

void ScriptManagerAngelScript::completeRegistration()
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

	// PHASE 2: Register all members (methods, properties, operators, etc.)
	// At this point, all types are registered and can be referenced

	for (auto& rc : m_classRegistry)
	{
		const IRuntimeClass* runtimeClass = rc.runtimeClass;
		const TypeInfo& exportType = runtimeClass->getExportType();

		// Parse namespace and class name
		std::wstring fullName = exportType.getName();
		std::string fullNameStr = wstombs(Utf8Encoding(), fullName);

		// Replace '.' with '::'
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
			namespaceName = fullNameStr.substr(0, lastSep - 1);
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
			log::warning << L"Failed to set namespace for completing registration of class \"" << mbstows(fullNameStr) << L"\"" << Endl;
			continue;
		}

		// Register static methods as global functions within the class namespace
		// AngelScript doesn't support static methods on types, so we register them as global functions
		// within the fully qualified namespace (e.g., "traktor::ClassName::staticMethod")
		const uint32_t staticMethodCount = runtimeClass->getStaticMethodCount();
		for (uint32_t i = 0; i < staticMethodCount; ++i)
		{
			const std::string methodName = runtimeClass->getStaticMethodName(i);
			const IRuntimeDispatch* dispatch = runtimeClass->getStaticMethodDispatch(i);

#if defined(T_NEED_RUNTIME_SIGNATURE)
			// Build method signatures from dispatch runtime signature (handles overloads)
			AlignedVector< std::string > signatures = convertRuntimeSignaturesToAngelScript(dispatch, methodName);

			// Register each overload as a global function within the class namespace
			// Set namespace to "namespace::ClassName" to make it accessible as "ClassName::staticMethod"
			std::string staticNamespace = namespaceName.empty() ? className : namespaceName + "::" + className;
			r = m_scriptEngine->SetDefaultNamespace(staticNamespace.c_str());
			if (r < 0)
			{
				log::warning << L"Failed to set namespace \"" << mbstows(staticNamespace) << L"\" for static method \"" << mbstows(methodName) << L"\": " << getErrorString(r) << L" (" << r << L")" << Endl;
				m_scriptEngine->SetDefaultNamespace(namespaceName.c_str()); // Restore class namespace
				continue;
			}

			// Register each overload
			for (const auto& signature : signatures)
			{
				r = m_scriptEngine->RegisterGlobalFunction(
					signature.c_str(),
					asFUNCTION(asGenericStaticMethodWrapper),
					asCALL_GENERIC,
					const_cast<void*>(static_cast<const void*>(dispatch))
				);
				if (r < 0)
				{
					log::warning << L"Failed to register static method \"" << mbstows(methodName) << L"\" with signature \"" << mbstows(signature) << L"\" for class \"" << mbstows(fullNameStr) << L"\": " << getErrorString(r) << L" (" << r << L")" << Endl;
				}
			}

			// Restore namespace to class level
			m_scriptEngine->SetDefaultNamespace(namespaceName.c_str());
#else
			log::error << L"AngelScript backend requires T_NEED_RUNTIME_SIGNATURE to be defined for class registration" << Endl;
			m_scriptEngine->SetDefaultNamespace("");
			return;
#endif
		}

		// TODO: Register instance methods
		// TODO: Register properties
		// TODO: Register operators

		// Reset to global namespace
		m_scriptEngine->SetDefaultNamespace("");
	}
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
