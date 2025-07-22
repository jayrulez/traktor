/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Script/AngelScript/ScriptManagerAngelScript.h"

#include "angelscript.h"
#include "Core/Class/AutoVerify.h"
#include "Core/Class/Boxes/BoxedTypeInfo.h"
#include "Core/Class/IRuntimeClass.h"
#include "Core/Class/IRuntimeDispatch.h"
#include "Core/Io/StringOutputStream.h"
#include "Core/Io/Utf8Encoding.h"
#include "Core/Log/Log.h"
#include "Core/Memory/IAllocator.h"
#include "Core/Misc/Save.h"
#include "Core/Misc/Split.h"
#include "Core/Misc/String.h"
#include "Core/Misc/TString.h"
#include "Core/Thread/Acquire.h"
#include "Script/AngelScript/ScriptContextAngelScript.h"
#include "Script/AngelScript/ScriptDebuggerAngelScript.h"
#include "Script/AngelScript/ScriptObjectAngelScript.h"
#include "Script/AngelScript/ScriptProfilerAngelScript.h"
#include "Script/AngelScript/scriptstdstring.h"
#include "Script/AngelScript/ScriptUtilitiesAngelScript.h"

#include <regex>

namespace traktor::script
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.script.ScriptManagerAngelScript", 0, ScriptManagerAngelScript, IScriptManager)

ScriptManagerAngelScript* ScriptManagerAngelScript::ms_instance = nullptr;

// Remove the TraktorStringFactory class and string-related functions since we're using std::string

ScriptManagerAngelScript::ScriptManagerAngelScript()
	: m_engine(nullptr)
	, m_lockContext(nullptr)
{
	T_FATAL_ASSERT(ms_instance == nullptr);
	ms_instance = this;

	// Create AngelScript engine
	m_engine = asCreateScriptEngine();
	if (!m_engine)
	{
		log::error << L"Failed to create AngelScript engine" << Endl;
		return;
	}

	// Set memory functions
	//asSetGlobalMemoryFunctions(scriptAlloc, scriptFree);

	// Set message callback for compilation errors
	m_engine->SetMessageCallback(asFUNCTION(messageCallback), this, asCALL_CDECL);

	// Set engine properties
	m_engine->SetEngineProperty(asEP_ALLOW_UNSAFE_REFERENCES, true);
	m_engine->SetEngineProperty(asEP_OPTIMIZE_BYTECODE, true);
	m_engine->SetEngineProperty(asEP_COPY_SCRIPT_SECTIONS, true);
	m_engine->SetEngineProperty(asEP_AUTO_GARBAGE_COLLECT, false); // Manual GC

	// Register AngelScript's standard string addon instead of Traktor String
	RegisterStdString(m_engine);

	// Register standard utility functions
	registerStandardFunctions(m_engine);
}

ScriptManagerAngelScript::~ScriptManagerAngelScript()
{
	T_FATAL_ASSERT_M(!m_engine, L"Must call destroy");
	T_FATAL_ASSERT(ms_instance == this);
	ms_instance = nullptr;
}

void ScriptManagerAngelScript::destroy()
{
	if (!m_engine)
		return;

	T_ANONYMOUS_VAR(Ref< ScriptManagerAngelScript >)(this);
	T_FATAL_ASSERT(m_contexts.empty());

	// Discard all tags from C++ rtti types
	for (auto& rc : m_classRegistry)
		for (auto& derivedType : rc.runtimeClass->getExportType().findAllOf())
			derivedType->setTag(0);

	m_debugger = nullptr;
	m_profiler = nullptr;

	// Release engine
	if (m_engine)
	{
		m_engine->ShutDownAndRelease();
		m_engine = nullptr;
	}
}

void ScriptManagerAngelScript::registerClass(IRuntimeClass* runtimeClass)
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

	const TypeInfo& exportType = runtimeClass->getExportType();
	const int32_t classRegistryIndex = int32_t(m_classRegistry.size());

	RegisteredClass& rc = m_classRegistry.push_back();
	rc.runtimeClass = runtimeClass;

	// Convert class name for AngelScript
	std::string qualifiedClassName = wstombs(exportType.getName());

	// Find the last dot
	size_t lastDot = qualifiedClassName.rfind('.');

	std::string ns;
	std::string className;

	if (lastDot == std::string::npos)
	{
		// No namespace
		ns = "";
		className = qualifiedClassName;
	}
	else
	{
		// Extract namespace and class name
		ns = qualifiedClassName.substr(0, lastDot);
		className = qualifiedClassName.substr(lastDot + 1);

		// Replace dots with :: in namespace
		for (size_t i = 0; i < ns.length(); ++i)
		{
			if (ns[i] == '.')
			{
				ns[i] = ':';
				ns.insert(i + 1, ":");
				++i;
			}
		}
	}

	std::string fullName;
	if (!ns.empty())
		fullName = ns + "::" + className;
	else
		fullName = className;

	// Determine object flags
	asQWORD flags = asOBJ_REF | asOBJ_GC;

	m_engine->SetDefaultNamespace(ns.c_str());

	// Register the object type
	int result = m_engine->RegisterObjectType(className.c_str(), 0, flags);
	if (result < 0)
	{
		log::error << L"Failed to register object type: " << exportType.getName() << Endl;
		return;
	}

	rc.typeInfo = m_engine->GetTypeInfoByName(className.c_str());

	// Register object behaviors
	registerObjectBehaviors(className, runtimeClass);

	// Register methods, properties, and static methods using reflection
	registerMethodsFromReflection(className, runtimeClass);
	registerPropertiesFromReflection(className, runtimeClass);
	registerStaticMethodsFromReflection(qualifiedClassName, runtimeClass);

	// Store index in TypeInfo tag for fast lookup
	for (auto derivedType : exportType.findAllOf())
	{
		if (derivedType->getTag() != 0)
		{
			const RegisteredClass& rc2 = m_classRegistry[derivedType->getTag() - 1];
			const TypeInfo& exportType2 = rc2.runtimeClass->getExportType();
			if (is_type_of(exportType, exportType2))
				continue;
		}
		derivedType->setTag(classRegistryIndex + 1);
	}
	m_engine->SetDefaultNamespace("");
}

Ref< IScriptContext > ScriptManagerAngelScript::createContext(bool strict)
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

	// Create context
	Ref< ScriptContextAngelScript > context = new ScriptContextAngelScript(this, m_engine, strict);
	m_contexts.push_back(context);
	return context;
}

Ref< IScriptDebugger > ScriptManagerAngelScript::createDebugger()
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

	if (!m_debugger)
		m_debugger = new ScriptDebuggerAngelScript(this, m_engine);

	return m_debugger;
}

Ref< IScriptProfiler > ScriptManagerAngelScript::createProfiler()
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

	if (!m_profiler)
		m_profiler = new ScriptProfilerAngelScript(this, m_engine);

	return m_profiler;
}

void ScriptManagerAngelScript::collectGarbage(bool full)
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

	if (full)
		m_engine->GarbageCollect(asGC_FULL_CYCLE);
	else
		m_engine->GarbageCollect(asGC_ONE_STEP);
}

void ScriptManagerAngelScript::getStatistics(ScriptStatistics& outStatistics) const
{
	asUINT currentSize, totalDestroyed, totalDetected;
	m_engine->GetGCStatistics(&currentSize, &totalDestroyed, &totalDetected);
	outStatistics.memoryUsage = currentSize;
}

void ScriptManagerAngelScript::setArgumentFromAny(asIScriptContext* ctx, uint32_t argIndex, const Any& any)
{
	// todo:
	// ScriptObjectAngelScript
	// ScriptDelegateAngelScript

	switch (any.getType())
	{
	case Any::Type::Boolean:
		ctx->SetArgByte(argIndex, any.getBooleanUnsafe() ? 1 : 0);
		break;
	case Any::Type::Int32:
		ctx->SetArgDWord(argIndex, any.getInt32Unsafe());
		break;
	case Any::Type::Int64:
		ctx->SetArgQWord(argIndex, any.getInt64Unsafe());
		break;
	case Any::Type::Float:
		ctx->SetArgFloat(argIndex, any.getFloatUnsafe());
		break;
	case Any::Type::Double:
		ctx->SetArgDouble(argIndex, any.getDoubleUnsafe());
		break;
	case Any::Type::String:
		{
			std::string narrowStr = any.getString();

			// Create AngelScript string object
			asITypeInfo* stringType = m_engine->GetTypeInfoByDecl("string");
			std::string* asString = static_cast< std::string* >(m_engine->CreateScriptObject(stringType));
			*asString = narrowStr;

			ctx->SetArgObject(argIndex, asString);
		}
		break;
	case Any::Type::Object:
		ctx->SetArgObject(argIndex, any.getObjectUnsafe());
		break;
	default:
		ctx->SetArgAddress(argIndex, nullptr);
		break;
	}
}

void ScriptManagerAngelScript::setArgumentFromObject(asIScriptContext* ctx, uint32_t argIndex, ITypedObject* object)
{
	ctx->SetArgObject(argIndex, object);
}

Any ScriptManagerAngelScript::getReturnValueAsAny(asIScriptContext* ctx, int returnTypeId)
{
	if (returnTypeId == asTYPEID_VOID)
		return Any();

	void* returnPtr = ctx->GetAddressOfReturnValue();
	if (!returnPtr)
		return Any();

	if (returnTypeId == asTYPEID_BOOL)
		return Any::fromBoolean(*(bool*)returnPtr);
	else if (returnTypeId == asTYPEID_INT32)
		return Any::fromInt32(*(int32_t*)returnPtr);
	else if (returnTypeId == asTYPEID_INT64)
		return Any::fromInt64(*(int64_t*)returnPtr);
	else if (returnTypeId == asTYPEID_FLOAT)
		return Any::fromFloat(*(float*)returnPtr);
	else if (returnTypeId == asTYPEID_DOUBLE)
		return Any::fromDouble(*(double*)returnPtr);
	else if (returnTypeId == m_engine->GetTypeIdByDecl("string"))
	{
		// Convert AngelScript string (std::string) to wide string for Traktor
		std::string* str = static_cast< std::string* >(returnPtr);
		std::wstring wstr = mbstows(Utf8Encoding(), *str);
		return Any::fromString(wstr);
	}
	else if (returnTypeId & asTYPEID_OBJHANDLE)
	{
		ITypedObject* obj = static_cast< ITypedObject* >(returnPtr);
		return Any::fromObject(obj);
	}

	return Any();
}

ITypedObject* ScriptManagerAngelScript::getReturnValueAsObject(asIScriptContext* ctx, int returnTypeId)
{
	if (returnTypeId & asTYPEID_OBJHANDLE)
	{
		void* returnPtr = ctx->GetAddressOfReturnValue();
		return static_cast< ITypedObject* >(returnPtr);
	}
	return nullptr;
}

Any ScriptManagerAngelScript::convertAngelScriptArgToAny(asIScriptGeneric* gen, uint32_t argIndex)
{
	int typeId = gen->GetArgTypeId(argIndex);

	if (typeId == asTYPEID_BOOL)
		return Any::fromBoolean(gen->GetArgByte(argIndex) != 0);
	else if (typeId == asTYPEID_INT32)
		return Any::fromInt32(gen->GetArgDWord(argIndex));
	else if (typeId == asTYPEID_INT64)
		return Any::fromInt64(gen->GetArgQWord(argIndex));
	else if (typeId == asTYPEID_FLOAT)
		return Any::fromFloat(gen->GetArgFloat(argIndex));
	else if (typeId == asTYPEID_DOUBLE)
		return Any::fromDouble(gen->GetArgDouble(argIndex));
	else if (typeId == m_engine->GetTypeIdByDecl("string"))
	{
		// Convert AngelScript string to wide string for Traktor
		std::string* str = static_cast< std::string* >(gen->GetArgObject(argIndex));
		if (str)
		{
			std::wstring wstr = mbstows(Utf8Encoding(), *str);
			return Any::fromString(wstr);
		}
		return Any::fromString(L"");
	}
	else if (typeId & asTYPEID_OBJHANDLE)
	{
		ITypedObject* obj = static_cast< ITypedObject* >(gen->GetArgObject(argIndex));
		return Any::fromObject(obj);
	}

	return Any();
}

void ScriptManagerAngelScript::setAngelScriptReturnValue(asIScriptGeneric* gen, const Any& value)
{
	switch (value.getType())
	{
	case Any::Type::Void:
		// Nothing to set for void return
		break;
	case Any::Type::Boolean:
		gen->SetReturnByte(value.getBooleanUnsafe() ? 1 : 0);
		break;
	case Any::Type::Int32:
		gen->SetReturnDWord(value.getInt32Unsafe());
		break;
	case Any::Type::Int64:
		gen->SetReturnQWord(value.getInt64Unsafe());
		break;
	case Any::Type::Float:
		gen->SetReturnFloat(value.getFloatUnsafe());
		break;
	case Any::Type::Double:
		gen->SetReturnDouble(value.getDoubleUnsafe());
		break;
	case Any::Type::String:
		{
			// Convert wide string to narrow string for AngelScript
			std::string narrowStr = value.getString();

			// Create AngelScript string object
			asIScriptEngine* engine = gen->GetEngine();
			asITypeInfo* stringType = engine->GetTypeInfoByDecl("string");
			std::string* asString = static_cast< std::string* >(engine->CreateScriptObject(stringType));
			*asString = narrowStr;

			gen->SetReturnObject(asString);
		}
		break;
	case Any::Type::Object:
		gen->SetReturnObject(value.getObjectUnsafe());
		break;
	default:
		gen->SetReturnAddress(nullptr);
		break;
	}
}

ITypedObject* ScriptManagerAngelScript::extractObjectFromGeneric(asIScriptGeneric* gen)
{
	void* obj = gen->GetObject();
	return static_cast< ITypedObject* >(obj);
}

void ScriptManagerAngelScript::destroyContext(ScriptContextAngelScript* context)
{
	m_contexts.remove(context);
}

void ScriptManagerAngelScript::registerObjectBehaviors(const std::string& className, IRuntimeClass* runtimeClass)
{
	// Register garbage collection behaviors
	m_engine->RegisterObjectBehaviour(className.c_str(), asBEHAVE_ADDREF, "void f()", asFUNCTION(objectAddRef), asCALL_CDECL_OBJLAST);
	m_engine->RegisterObjectBehaviour(className.c_str(), asBEHAVE_RELEASE, "void f()", asFUNCTION(objectRelease), asCALL_CDECL_OBJLAST);

	// Register constructor if available
	if (runtimeClass->getConstructorDispatch())
	{
		std::string factoryDecl = className + "@ f()";
		m_engine->RegisterObjectBehaviour(className.c_str(), asBEHAVE_FACTORY, factoryDecl.c_str(), asFUNCTION(genericConstructorWrapper), asCALL_GENERIC, (void*)runtimeClass->getConstructorDispatch());
	}
}

void ScriptManagerAngelScript::registerMethodsFromReflection(const std::string& className, IRuntimeClass* runtimeClass)
{
	const uint32_t methodCount = runtimeClass->getMethodCount();
	for (uint32_t i = 0; i < methodCount; ++i)
	{
		const IRuntimeDispatch* dispatch = runtimeClass->getMethodDispatch(i);
		if (!dispatch)
			continue;

#if defined(T_NEED_RUNTIME_SIGNATURE)
		// Get signature from Traktor's reflection system
		StringOutputStream ss;
		dispatch->signature(ss);
		std::string signature = wstombs(ss.str());
		size_t firstComma = signature.find(',');
		std::string qualifiedReturnType;
		std::string parameters;

		if (firstComma == std::string::npos)
		{
			// No comma, entire string is the return type
			qualifiedReturnType = signature;
			parameters = "";
		}
		else
		{
			// Split at first comma
			qualifiedReturnType = signature.substr(0, firstComma);
			parameters = signature.substr(firstComma + 1);
		}
		parameters = convertSignatureToAngelScript(parameters);
		parameters = convertReferenceParameters(parameters);

		// Replace all dots with ::
		size_t pos = 0;
		while ((pos = qualifiedReturnType.find('.', pos)) != std::string::npos)
		{
			qualifiedReturnType.replace(pos, 1, "::");
			pos += 2; // Move past the inserted ::
		}

		pos = 0;
		while ((pos = parameters.find('.', pos)) != std::string::npos)
		{
			parameters.replace(pos, 1, "::");
			pos += 2; // Move past the inserted ::
		}

		// convertSignatureToAngelScript(qualifiedReturnType);
		const std::string methodName = runtimeClass->getMethodName(i);

		std::string asSignature;
		if (qualifiedReturnType.substr(0, 7) == "traktor")
			asSignature = "const " + qualifiedReturnType + "&" + " " + methodName + "(" + parameters + ")";
		else
			asSignature = qualifiedReturnType + " " + methodName + "(" + parameters + ")";
#else
		// Fallback for when signatures aren't available
		const std::string methodName = runtimeClass->getMethodName(i);
		std::string asSignature = methodName + "()";
#endif

		// Register with AngelScript
		int result = m_engine->RegisterObjectMethod(
			className.c_str(),
			asSignature.c_str(),
			asFUNCTION(genericMethodWrapper),
			asCALL_GENERIC,
			(void*)dispatch);

		if (result < 0)
			log::warning << L"Failed to register method: " << mbstows(className) << L"::"
						 << mbstows(runtimeClass->getMethodName(i))
						 << L" with signature: " << mbstows(asSignature) << Endl;
	}
}

void ScriptManagerAngelScript::registerPropertiesFromReflection(const std::string& className, IRuntimeClass* runtimeClass)
{
	const uint32_t propertyCount = runtimeClass->getPropertiesCount();
	for (uint32_t i = 0; i < propertyCount; ++i)
	{
		const std::string propertyName = runtimeClass->getPropertyName(i);

		// Register getter
		if (const IRuntimeDispatch* getter = runtimeClass->getPropertyGetDispatch(i))
		{
#if defined(T_NEED_RUNTIME_SIGNATURE)
			StringOutputStream ss;
			getter->signature(ss);
			std::string getterSig = convertPropertySignatureToAngelScript(wstombs(ss.str()), propertyName, true);
#else
			std::string getterSig = "void get_" + propertyName + "() const";
#endif

			m_engine->RegisterObjectMethod(
				className.c_str(),
				getterSig.c_str(),
				asFUNCTION(genericPropertyGetWrapper),
				asCALL_GENERIC,
				(void*)getter);
		}

		// Register setter
		if (const IRuntimeDispatch* setter = runtimeClass->getPropertySetDispatch(i))
		{
#if defined(T_NEED_RUNTIME_SIGNATURE)
			StringOutputStream ss;
			setter->signature(ss);
			std::string setterSig = convertPropertySignatureToAngelScript(wstombs(ss.str()), propertyName, false);
#else
			std::string setterSig = "void set_" + propertyName + "(?)";
#endif

			m_engine->RegisterObjectMethod(
				className.c_str(),
				setterSig.c_str(),
				asFUNCTION(genericPropertySetWrapper),
				asCALL_GENERIC,
				(void*)setter);
		}
	}
}

void ScriptManagerAngelScript::registerStaticMethodsFromReflection(const std::string& className, IRuntimeClass* runtimeClass)
{
	const std::string& defaultNS = m_engine->GetDefaultNamespace();

	std::string ns = className;

	// Replace dots with :: in namespace
	for (size_t i = 0; i < ns.length(); ++i)
	{
		if (ns[i] == '.')
		{
			ns[i] = ':';
			ns.insert(i + 1, ":");
			++i;
		}
	}

	m_engine->SetDefaultNamespace(ns.c_str());

	const uint32_t staticMethodCount = runtimeClass->getStaticMethodCount();
	for (uint32_t i = 0; i < staticMethodCount; ++i)
	{
		const IRuntimeDispatch* dispatch = runtimeClass->getStaticMethodDispatch(i);
		if (!dispatch)
			continue;

#if defined(T_NEED_RUNTIME_SIGNATURE)
		StringOutputStream ss;
		dispatch->signature(ss);
		std::string signature = wstombs(ss.str());
		size_t firstComma = signature.find(',');

		std::string qualifiedReturnType;
		std::string parameters;

		if (firstComma == std::string::npos)
		{
			// No comma, entire string is the return type
			qualifiedReturnType = signature;
			parameters = "";
		}
		else
		{
			// Split at first comma
			qualifiedReturnType = signature.substr(0, firstComma);
			parameters = signature.substr(firstComma + 1);
		}
		// Replace all dots with ::
		size_t pos = 0;
		while ((pos = qualifiedReturnType.find('.', pos)) != std::string::npos)
		{
			qualifiedReturnType.replace(pos, 1, "::");
			pos += 2; // Move past the inserted ::
		}

		pos = 0;
		while ((pos = parameters.find('.', pos)) != std::string::npos)
		{
			parameters.replace(pos, 1, "::");
			pos += 2; // Move past the inserted ::
		}

		//convertSignatureToAngelScript(qualifiedReturnType);
		const std::string methodName = runtimeClass->getStaticMethodName(i);

		std::string asSignature;
		if (qualifiedReturnType.substr(0, 7) == "traktor")
			asSignature = "const " + qualifiedReturnType + "&" + " " + methodName + "(" + parameters + ")";
		else
			asSignature = qualifiedReturnType + " " + methodName + "(" + parameters + ")";
#else
		const std::string methodName = runtimeClass->getStaticMethodName(i);
		std::string asSignature = methodName + "()";
#endif

		int result = m_engine->RegisterGlobalFunction(asSignature.c_str(),
			asFUNCTION(genericStaticMethodWrapper),
			asCALL_GENERIC,
			(void*)dispatch);

		if (result < 0)
			log::warning << L"Failed to register static method: " << mbstows(className) << L"::"
						 << mbstows(runtimeClass->getStaticMethodName(i))
						 << L" with signature: " << mbstows(asSignature) << Endl;
	}
	m_engine->SetDefaultNamespace(defaultNS.c_str());
}

// Signature conversion utilities
std::string ScriptManagerAngelScript::convertSignatureToAngelScript(const std::string& traktorSignature)
{
	std::string result = traktorSignature;

	// Convert common Traktor types to AngelScript types
	result = std::regex_replace(result, std::regex("\\bstd::wstring\\b"), "string");
	result = std::regex_replace(result, std::regex("\\bwstring\\b"), "string");
	result = std::regex_replace(result, std::regex("\\bint32_t\\b"), "int");
	result = std::regex_replace(result, std::regex("\\buint32_t\\b"), "uint");
	result = std::regex_replace(result, std::regex("\\bint64_t\\b"), "int64");
	result = std::regex_replace(result, std::regex("\\buint64_t\\b"), "uint64");

	// Convert reference syntax
	result = convertReferenceParameters(result);

	return result;
}

std::string ScriptManagerAngelScript::convertPropertySignatureToAngelScript(
	const std::string& traktorSignature,
	const std::string& propertyName,
	bool isGetter)
{
	if (isGetter)
	{
		std::string returnType = extractReturnTypeFromSignature(traktorSignature);
		return convertTypeToAngelScript(returnType) + " get_" + propertyName + "() const";
	}
	else
	{
		std::string paramType = extractParameterTypeFromSignature(traktorSignature);
		return "void set_" + propertyName + "(" + convertTypeToAngelScript(paramType) + " &in)";
	}
}

std::string ScriptManagerAngelScript::convertTypeToAngelScript(const std::string& traktorType)
{
	if (traktorType == "std::wstring" || traktorType == "wstring")
		return "string";
	if (traktorType == "int32_t")
		return "int";
	if (traktorType == "uint32_t")
		return "uint";
	if (traktorType == "int64_t")
		return "int64";
	if (traktorType == "uint64_t")
		return "uint64";
	return traktorType;
}

std::string ScriptManagerAngelScript::convertReferenceParameters(const std::string& signature)
{
	std::string result = signature;
	// First pass: mark const references
	result = std::regex_replace(result, std::regex("const\\s+(\\w+)\\s*&"), "const $1 &in");
	// Second pass: convert remaining references (that aren't already &in)
	result = std::regex_replace(result, std::regex("(\\w+)\\s*&(?!in)"), "$1 &inout");
	return result;
}

std::string ScriptManagerAngelScript::extractReturnTypeFromSignature(const std::string& signature)
{
	// Simple extraction - find first word before space or (
	std::regex returnTypeRegex("^(\\w+)");
	std::smatch match;
	if (std::regex_search(signature, match, returnTypeRegex))
		return match[1].str();
	return "void";
}

std::string ScriptManagerAngelScript::extractParameterTypeFromSignature(const std::string& signature)
{
	// Extract first parameter type from signature
	std::regex paramRegex("\\((.*?)\\)");
	std::smatch match;
	if (std::regex_search(signature, match, paramRegex))
	{
		std::string params = match[1].str();
		// Get first parameter
		size_t commaPos = params.find(',');
		if (commaPos != std::string::npos)
			params = params.substr(0, commaPos);
		return params;
	}
	return "void";
}

std::string ScriptManagerAngelScript::convertTypeName(const std::wstring& traktorName) const
{
	std::string name = wstombs(traktorName);
	std::replace(name.begin(), name.end(), '.', '_');
	return name;
}

// Static callback implementations
void ScriptManagerAngelScript::messageCallback(const asSMessageInfo* msg, void* param)
{
	ScriptManagerAngelScript* manager = static_cast< ScriptManagerAngelScript* >(param);

	std::wstring section = mbstows(msg->section);
	std::wstring message = mbstows(msg->message);

	switch (msg->type)
	{
	case asMSGTYPE_ERROR:
		log::error << section << L"(" << msg->row << L"," << msg->col << L"): " << message << Endl;
		break;
	case asMSGTYPE_WARNING:
		log::warning << section << L"(" << msg->row << L"," << msg->col << L"): " << message << Endl;
		break;
	case asMSGTYPE_INFORMATION:
		log::info << section << L"(" << msg->row << L"," << msg->col << L"): " << message << Endl;
		break;
	}
}

void* ScriptManagerAngelScript::scriptAlloc(size_t size)
{
	return getAllocator()->alloc(size, 16, T_FILE_LINE);
}

void ScriptManagerAngelScript::scriptFree(void* ptr)
{
	getAllocator()->free(ptr);
}

void ScriptManagerAngelScript::genericMethodWrapper(asIScriptGeneric* gen)
{
	const IRuntimeDispatch* dispatch = static_cast< const IRuntimeDispatch* >(gen->GetAuxiliary());

	// Get the object instance
	ITypedObject* object = ms_instance->extractObjectFromGeneric(gen);
	if (!object)
	{
		asIScriptContext* ctx = asGetActiveContext();
		if (ctx)
			ctx->SetException("Null object reference");
		return;
	}

	// Convert arguments from AngelScript to Any array
	const uint32_t argc = gen->GetArgCount();
	std::vector< Any > argv(argc);

	for (uint32_t i = 0; i < argc; ++i)
		argv[i] = ms_instance->convertAngelScriptArgToAny(gen, i);

	// Invoke the method
#if T_VERIFY_USING_EXCEPTIONS
	try
	{
		Any result = dispatch->invoke(object, argc, argv.data());
		ms_instance->setAngelScriptReturnValue(gen, result);
	}
	catch (const RuntimeException& e)
	{
		asIScriptContext* ctx = asGetActiveContext();
		if (ctx)
			ctx->SetException(wstombs(e.what()).c_str());
	}
#else
	Any result = dispatch->invoke(object, argc, argv.data());
	ms_instance->setAngelScriptReturnValue(gen, result);
#endif
}

void ScriptManagerAngelScript::genericConstructorWrapper(asIScriptGeneric* gen)
{
	const IRuntimeDispatch* dispatch = static_cast< const IRuntimeDispatch* >(gen->GetAuxiliary());

	const uint32_t argc = gen->GetArgCount();
	std::vector< Any > argv(argc);

	for (uint32_t i = 0; i < argc; ++i)
		argv[i] = ms_instance->convertAngelScriptArgToAny(gen, i);

#if T_VERIFY_USING_EXCEPTIONS
	try
	{
		Any result = dispatch->invoke(nullptr, argc, argv.data());
		if (result.isObject())
		{
			ITypedObject* obj = result.getObjectUnsafe();
			if (obj)
				obj->addRef(nullptr); // For AngelScript ownership
			gen->SetReturnObject(obj);
		}
		else
		{
			gen->SetReturnObject(nullptr);
		}
	}
	catch (const RuntimeException& e)
	{
		asIScriptContext* ctx = asGetActiveContext();
		if (ctx)
			ctx->SetException(wstombs(e.what()).c_str());
	}
#else
	Any result = dispatch->invoke(nullptr, argc, argv.data());
	if (result.isObject())
	{
		ITypedObject* obj = result.getObjectUnsafe();
		if (obj)
			obj->addRef(); // For AngelScript ownership
		gen->SetReturnObject(obj);
	}
	else
	{
		gen->SetReturnObject(nullptr);
	}
#endif
}

void ScriptManagerAngelScript::genericPropertyGetWrapper(asIScriptGeneric* gen)
{
	const IRuntimeDispatch* dispatch = static_cast< const IRuntimeDispatch* >(gen->GetAuxiliary());

	ITypedObject* object = ms_instance->extractObjectFromGeneric(gen);
	if (!object)
	{
		asIScriptContext* ctx = asGetActiveContext();
		if (ctx)
			ctx->SetException("Null object reference");
		return;
	}

#if T_VERIFY_USING_EXCEPTIONS
	try
	{
		Any result = dispatch->invoke(object, 0, nullptr);
		ms_instance->setAngelScriptReturnValue(gen, result);
	}
	catch (const RuntimeException& e)
	{
		asIScriptContext* ctx = asGetActiveContext();
		if (ctx)
			ctx->SetException(wstombs(e.what()).c_str());
	}
#else
	Any result = dispatch->invoke(object, 0, nullptr);
	ms_instance->setAngelScriptReturnValue(gen, result);
#endif
}

void ScriptManagerAngelScript::genericPropertySetWrapper(asIScriptGeneric* gen)
{
	const IRuntimeDispatch* dispatch = static_cast< const IRuntimeDispatch* >(gen->GetAuxiliary());

	ITypedObject* object = ms_instance->extractObjectFromGeneric(gen);
	if (!object)
	{
		asIScriptContext* ctx = asGetActiveContext();
		if (ctx)
			ctx->SetException("Null object reference");
		return;
	}

	// Get the value to set
	Any value = ms_instance->convertAngelScriptArgToAny(gen, 0);

#if T_VERIFY_USING_EXCEPTIONS
	try
	{
		dispatch->invoke(object, 1, &value);
	}
	catch (const RuntimeException& e)
	{
		asIScriptContext* ctx = asGetActiveContext();
		if (ctx)
			ctx->SetException(wstombs(e.what()).c_str());
	}
#else
	dispatch->invoke(object, 1, &value);
#endif
}

void ScriptManagerAngelScript::genericStaticMethodWrapper(asIScriptGeneric* gen)
{
	const IRuntimeDispatch* dispatch = static_cast< const IRuntimeDispatch* >(gen->GetAuxiliary());

	const uint32_t argc = gen->GetArgCount();
	std::vector< Any > argv(argc);

	for (uint32_t i = 0; i < argc; ++i)
		argv[i] = ms_instance->convertAngelScriptArgToAny(gen, i);

#if T_VERIFY_USING_EXCEPTIONS
	try
	{
		Any result = dispatch->invoke(nullptr, argc, argv.data());
		ms_instance->setAngelScriptReturnValue(gen, result);
	}
	catch (const RuntimeException& e)
	{
		asIScriptContext* ctx = asGetActiveContext();
		if (ctx)
			ctx->SetException(wstombs(e.what()).c_str());
	}
#else
	Any result = dispatch->invoke(nullptr, argc, argv.data());
	ms_instance->setAngelScriptReturnValue(gen, result);
#endif
}

void ScriptManagerAngelScript::objectAddRef(void* obj)
{
	if (obj)
	{
		ITypedObject* typedObj = static_cast< ITypedObject* >(obj);
		typedObj->addRef(nullptr);
	}
}

void ScriptManagerAngelScript::objectRelease(void* obj)
{
	if (obj)
	{
		ITypedObject* typedObj = static_cast< ITypedObject* >(obj);
		typedObj->release(nullptr);
	}
}

}