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
#include "Core/Math/Scalar.h"
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

	// Handle common Traktor base types that need manual registration
	if (type == L"TypeInfo" || type == L"traktor.TypeInfo") return "traktor::TypeInfo@";
	// Scalar is registered as a value type, so const Scalar& or Scalar& becomes just Scalar
	if (type == L"Scalar" || type == L"traktor.Scalar") return "traktor::Scalar";
	if (type == L"Object" || type == L"traktor.Object") return "traktor::Object@";
	if (type == L"ITypedObject" || type == L"traktor.ITypedObject") return "traktor::ITypedObject@";
	if (type == L"ISerializable" || type == L"traktor.ISerializable") return "traktor::ISerializable@";
	if (type == L"IRuntimeDelegate" || type == L"traktor.IRuntimeDelegate") return "traktor::IRuntimeDelegate@";
	if (type == L"RefArray" || type == L"traktor.RefArray") return ""; // RefArray without template param, skip

	// Handle boxed types - these wrap value types for the runtime system
	// Strip "Boxed" prefix and convert to the registered AngelScript type
	if (type.find(L"Boxed") == 0)
	{
		std::wstring unboxedType = type.substr(5); // Remove "Boxed" prefix

		// Map common boxed types to their AngelScript equivalents
		if (unboxedType == L"Color4f") return "traktor::Color4f@";
		if (unboxedType == L"Matrix33") return "traktor::Matrix33@";
		if (unboxedType == L"Matrix44") return "traktor::Matrix44@";
		if (unboxedType == L"Quaternion") return "traktor::Quaternion@";
		if (unboxedType == L"Transform") return "traktor::Transform@";
		if (unboxedType == L"Vector2") return "traktor::Vector2@";
		if (unboxedType == L"Vector4") return "traktor::Vector4@";
		if (unboxedType == L"Aabb2") return "traktor::Aabb2@";
		if (unboxedType == L"Aabb3") return "traktor::Aabb3@";
		if (unboxedType == L"Frustum") return "traktor::Frustum@";
		if (unboxedType == L"Ray3") return "traktor::Ray3@";
		if (unboxedType == L"Plane") return "traktor::Plane@";

		// If we don't recognize this boxed type, try to convert it generically
		return "traktor::" + wstombs(unboxedType) + "@";
	}

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

			// Strip whitespace from inner type
			size_t istart = innerType.find_first_not_of(L" \t");
			size_t iend = innerType.find_last_not_of(L" \t");
			if (istart != std::wstring::npos && iend != std::wstring::npos)
				innerType = innerType.substr(istart, iend - istart + 1);

			// Check if inner type is a placeholder (like "InnerType", "T", etc.)
			// If so, return empty string to indicate this signature should be skipped
			if (innerType == L"InnerType" || innerType == L"T" || innerType == L"U" || innerType == L"V")
				return "";

			// Strip namespace prefixes from template name for comparison
			// Handle both "traktor.Ref" and "resource::Proxy" formats
			std::wstring templateNameShort = templateName;

			// Strip "traktor." prefix
			if (templateName.find(L"traktor.") == 0)
				templateNameShort = templateName.substr(8);

			// Strip any remaining namespace (e.g., "resource::" -> "")
			size_t lastColon = templateNameShort.rfind(L"::");
			if (lastColon != std::wstring::npos)
				templateNameShort = templateNameShort.substr(lastColon + 2);

			size_t lastDot = templateNameShort.rfind(L'.');
			if (lastDot != std::wstring::npos)
				templateNameShort = templateNameShort.substr(lastDot + 1);

			// For Ref<>, RefArray<>, Proxy<>, etc., just extract and convert the inner type
			// These are smart pointer wrappers that should map to handles in AngelScript
			if (templateNameShort == L"Ref" || templateNameShort == L"RefArray" ||
			    templateNameShort == L"AlignedVector" || templateNameShort == L"SmallSet" ||
			    templateNameShort == L"SmallMap" || templateNameShort == L"StdVector" ||
			    templateNameShort == L"Range" || templateNameShort == L"Proxy")
			{
				// Recursively convert the inner type
				std::string converted = convertTraktorTypeToAngelScript(innerType);
				if (converted.empty())
					return ""; // Inner type couldn't be converted, skip this signature
				return converted;
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

AlignedVector< std::string > convertRuntimeSignaturesToAngelScript(const IRuntimeDispatch* dispatch, const std::string& methodName, bool logSkipped = false)
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

		// Skip this signature if return type couldn't be converted (e.g., template placeholder)
		if (returnType.empty())
		{
			if (logSkipped)
				log::info << L"    Skipped signature due to unconvertible return type: \"" << parts[0] << L"\"" << Endl;
			continue;
		}

		// Build AngelScript signature: "ReturnType methodName(ArgType1, ArgType2, ...)"
		std::string signature = returnType + " " + methodName + "(";

		bool skipSignature = false;
		std::wstring failedArgType;
		for (size_t i = 1; i < parts.size(); ++i)
		{
			std::string argType = convertTraktorTypeToAngelScript(parts[i]);

			// Skip this signature if any argument type couldn't be converted
			if (argType.empty())
			{
				skipSignature = true;
				failedArgType = parts[i];
				break;
			}

			if (i > 1)
				signature += ", ";
			signature += argType;
		}

		if (skipSignature)
		{
			if (logSkipped)
				log::info << L"    Skipped signature due to unconvertible argument type: " << failedArgType << L" (raw signature: " << overload << L")" << Endl;
			continue;
		}

		signature += ")";
		signatures.push_back(signature);
	}

	// Only log the raw signature if ALL signatures were skipped
	if (logSkipped && signatures.empty() && !signatureWide.empty())
	{
		log::info << L"    Raw signature from dispatch: \"" << signatureWide << L"\"" << Endl;
	}

	return signatures;
}
#endif

void setAngelScriptReturnValue(asIScriptGeneric* gen, const Any& result)
{
	// Set return value based on Any type
	if (result.isVoid())
	{
		// No return value (void)
		return;
	}
	else if (result.isBoolean())
	{
		gen->SetReturnByte(result.getBooleanUnsafe() ? 1 : 0);
	}
	else if (result.isInt32())
	{
		gen->SetReturnDWord(result.getInt32Unsafe());
	}
	else if (result.isInt64())
	{
		gen->SetReturnQWord(result.getInt64Unsafe());
	}
	else if (result.isFloat())
	{
		gen->SetReturnFloat(result.getFloatUnsafe());
	}
	else if (result.isObject())
	{
		// Return object handle
		ITypedObject* obj = result.getObjectUnsafe();
		gen->SetReturnObject(obj);
	}
	else
	{
		// Unknown type, return null/void
		return;
	}
}

void asGenericStaticMethodWrapper(asIScriptGeneric* gen)
{
	// Get the dispatch pointer from auxiliary data
	const IRuntimeDispatch* dispatch = static_cast<const IRuntimeDispatch*>(gen->GetAuxiliary());
	if (!dispatch)
		return;

	// Convert AngelScript arguments to Any[]
	const int32_t argCount = gen->GetArgCount();
	AlignedVector< Any > args;
	args.resize(argCount);

	for (int32_t i = 0; i < argCount; ++i)
	{
		// Get argument type ID
		int typeId = gen->GetArgTypeId(i);

		// Convert based on type
		if (typeId == asTYPEID_BOOL)
			args[i] = Any::fromBoolean(gen->GetArgByte(i) != 0);
		else if (typeId == asTYPEID_INT8)
			args[i] = Any::fromInt32(gen->GetArgByte(i));
		else if (typeId == asTYPEID_INT16)
			args[i] = Any::fromInt32(gen->GetArgWord(i));
		else if (typeId == asTYPEID_INT32)
			args[i] = Any::fromInt32(gen->GetArgDWord(i));
		else if (typeId == asTYPEID_INT64)
			args[i] = Any::fromInt64(gen->GetArgQWord(i));
		else if (typeId == asTYPEID_UINT8)
			args[i] = Any::fromInt32(gen->GetArgByte(i));
		else if (typeId == asTYPEID_UINT16)
			args[i] = Any::fromInt32(gen->GetArgWord(i));
		else if (typeId == asTYPEID_UINT32)
			args[i] = Any::fromInt32((int32_t)gen->GetArgDWord(i));
		else if (typeId == asTYPEID_UINT64)
			args[i] = Any::fromInt64((int64_t)gen->GetArgQWord(i));
		else if (typeId == asTYPEID_FLOAT)
			args[i] = Any::fromFloat(gen->GetArgFloat(i));
		else if (typeId == asTYPEID_DOUBLE)
			args[i] = Any::fromFloat((float)gen->GetArgDouble(i));
		else if (typeId & asTYPEID_OBJHANDLE)
		{
			// Object handle (reference type)
			void* obj = *(void**)gen->GetAddressOfArg(i);
			args[i] = Any::fromObject((ITypedObject*)obj);
		}
		else
		{
			// Unknown type, use null
			args[i] = Any();
		}
	}

	// Call dispatch->invoke() with no 'this' object (static method)
	Any result = dispatch->invoke(nullptr, argCount, argCount > 0 ? args.ptr() : nullptr);

	// Set return value
	setAngelScriptReturnValue(gen, result);
}

void asGenericInstanceMethodWrapper(asIScriptGeneric* gen)
{
	// Get the dispatch pointer from auxiliary data
	const IRuntimeDispatch* dispatch = static_cast<const IRuntimeDispatch*>(gen->GetAuxiliary());
	if (!dispatch)
		return;

	// Get the 'this' object from AngelScript
	void* thisPtr = gen->GetObject();
	if (!thisPtr)
		return;

	// Convert AngelScript arguments to Any[]
	const int32_t argCount = gen->GetArgCount();
	AlignedVector< Any > args;
	args.resize(argCount);

	for (int32_t i = 0; i < argCount; ++i)
	{
		// Get argument type ID
		int typeId = gen->GetArgTypeId(i);

		// Convert based on type
		if (typeId == asTYPEID_BOOL)
			args[i] = Any::fromBoolean(gen->GetArgByte(i) != 0);
		else if (typeId == asTYPEID_INT8)
			args[i] = Any::fromInt32(gen->GetArgByte(i));
		else if (typeId == asTYPEID_INT16)
			args[i] = Any::fromInt32(gen->GetArgWord(i));
		else if (typeId == asTYPEID_INT32)
			args[i] = Any::fromInt32(gen->GetArgDWord(i));
		else if (typeId == asTYPEID_INT64)
			args[i] = Any::fromInt64(gen->GetArgQWord(i));
		else if (typeId == asTYPEID_UINT8)
			args[i] = Any::fromInt32(gen->GetArgByte(i));
		else if (typeId == asTYPEID_UINT16)
			args[i] = Any::fromInt32(gen->GetArgWord(i));
		else if (typeId == asTYPEID_UINT32)
			args[i] = Any::fromInt32((int32_t)gen->GetArgDWord(i));
		else if (typeId == asTYPEID_UINT64)
			args[i] = Any::fromInt64((int64_t)gen->GetArgQWord(i));
		else if (typeId == asTYPEID_FLOAT)
			args[i] = Any::fromFloat(gen->GetArgFloat(i));
		else if (typeId == asTYPEID_DOUBLE)
			args[i] = Any::fromFloat((float)gen->GetArgDouble(i));
		else if (typeId & asTYPEID_OBJHANDLE)
		{
			// Object handle (reference type)
			void* obj = *(void**)gen->GetAddressOfArg(i);
			args[i] = Any::fromObject((ITypedObject*)obj);
		}
		else
		{
			// Unknown type, use null
			args[i] = Any();
		}
	}

	// Call dispatch->invoke() with 'this' object
	Any result = dispatch->invoke((ITypedObject*)thisPtr, argCount, argCount > 0 ? args.ptr() : nullptr);

	// Set return value
	setAngelScriptReturnValue(gen, result);
}

void asGenericPropertyGetWrapper(asIScriptGeneric* gen)
{
	// Get the dispatch pointer from auxiliary data
	const IRuntimeDispatch* dispatch = static_cast<const IRuntimeDispatch*>(gen->GetAuxiliary());
	if (!dispatch)
		return;

	// Get the 'this' object from AngelScript
	void* thisPtr = gen->GetObject();
	if (!thisPtr)
		return;

	// Call dispatch->invoke() with 'this' object (no arguments for getter)
	Any result = dispatch->invoke((ITypedObject*)thisPtr, 0, nullptr);

	// Set return value
	setAngelScriptReturnValue(gen, result);
}

void asGenericPropertySetWrapper(asIScriptGeneric* gen)
{
	// Get the dispatch pointer from auxiliary data
	const IRuntimeDispatch* dispatch = static_cast<const IRuntimeDispatch*>(gen->GetAuxiliary());
	if (!dispatch)
		return;

	// Get the 'this' object from AngelScript
	void* thisPtr = gen->GetObject();
	if (!thisPtr)
		return;

	// Get the property value from AngelScript (argument 0)
	int typeId = gen->GetArgTypeId(0);
	Any value;

	if (typeId == asTYPEID_BOOL)
		value = Any::fromBoolean(gen->GetArgByte(0) != 0);
	else if (typeId == asTYPEID_INT8)
		value = Any::fromInt32(gen->GetArgByte(0));
	else if (typeId == asTYPEID_INT16)
		value = Any::fromInt32(gen->GetArgWord(0));
	else if (typeId == asTYPEID_INT32)
		value = Any::fromInt32(gen->GetArgDWord(0));
	else if (typeId == asTYPEID_INT64)
		value = Any::fromInt64(gen->GetArgQWord(0));
	else if (typeId == asTYPEID_UINT8)
		value = Any::fromInt32(gen->GetArgByte(0));
	else if (typeId == asTYPEID_UINT16)
		value = Any::fromInt32(gen->GetArgWord(0));
	else if (typeId == asTYPEID_UINT32)
		value = Any::fromInt32((int32_t)gen->GetArgDWord(0));
	else if (typeId == asTYPEID_UINT64)
		value = Any::fromInt64((int64_t)gen->GetArgQWord(0));
	else if (typeId == asTYPEID_FLOAT)
		value = Any::fromFloat(gen->GetArgFloat(0));
	else if (typeId == asTYPEID_DOUBLE)
		value = Any::fromFloat((float)gen->GetArgDouble(0));
	else if (typeId & asTYPEID_OBJHANDLE)
	{
		// Object handle (reference type)
		void* obj = *(void**)gen->GetAddressOfArg(0);
		value = Any::fromObject((ITypedObject*)obj);
	}

	// Call dispatch->invoke() with 'this' object and value
	dispatch->invoke((ITypedObject*)thisPtr, 1, &value);
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

		// Register common base types as opaque reference types
		// These will be fully defined when their RTTI classes are registered
		// Note: TypeInfo is registered through RTTI, don't register manually

		r = m_scriptEngine->RegisterObjectType("Object", 0, asOBJ_REF | asOBJ_NOCOUNT);
		if (r < 0)
			log::warning << L"Failed to register Object type: " << getErrorString(r) << L" (" << r << L")" << Endl;

		r = m_scriptEngine->RegisterObjectType("ITypedObject", 0, asOBJ_REF | asOBJ_NOCOUNT);
		if (r < 0)
			log::warning << L"Failed to register ITypedObject type: " << getErrorString(r) << L" (" << r << L")" << Endl;

		r = m_scriptEngine->RegisterObjectType("ISerializable", 0, asOBJ_REF | asOBJ_NOCOUNT);
		if (r < 0)
			log::warning << L"Failed to register ISerializable type: " << getErrorString(r) << L" (" << r << L")" << Endl;

		r = m_scriptEngine->RegisterObjectType("IRuntimeDelegate", 0, asOBJ_REF | asOBJ_NOCOUNT);
		if (r < 0)
			log::warning << L"Failed to register IRuntimeDelegate type: " << getErrorString(r) << L" (" << r << L")" << Endl;

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

// Reference counting behaviors for Traktor objects
void asObjectAddRef(ITypedObject* obj)
{
	if (obj)
		obj->addRef(nullptr);
}

void asObjectRelease(ITypedObject* obj)
{
	if (obj)
		obj->release(nullptr);
}

// Scalar registration helper functions
void asScalarDefaultConstructor(void* memory)
{
	new(memory) Scalar();
}

void asScalarFloatConstructor(void* memory, float v)
{
	new(memory) Scalar(v);
}

void asScalarCopyConstructor(void* memory, const Scalar& v)
{
	new(memory) Scalar(v);
}

void asScalarDestructor(void* memory)
{
	((Scalar*)memory)->~Scalar();
}

Scalar& asScalarAssign(Scalar* self, const Scalar& other)
{
	return *self = other;
}

float asScalarToFloat(const Scalar* self)
{
	return float(*self);
}

Scalar asScalarAdd(const Scalar* self, const Scalar& other)
{
	return *self + other;
}

Scalar asScalarSub(const Scalar* self, const Scalar& other)
{
	return *self - other;
}

Scalar asScalarMul(const Scalar* self, const Scalar& other)
{
	return *self * other;
}

Scalar asScalarDiv(const Scalar* self, const Scalar& other)
{
	return *self / other;
}

bool asScalarEquals(const Scalar* self, const Scalar& other)
{
	return *self == other;
}

int asScalarCmp(const Scalar* self, const Scalar& other)
{
	if (*self < other) return -1;
	if (*self > other) return 1;
	return 0;
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

	// Register AddRef and Release behaviors for reference counting
	// These are required for all asOBJ_REF types
	r = m_scriptEngine->RegisterObjectBehaviour(
		className.c_str(),
		asBEHAVE_ADDREF,
		"void f()",
		asFUNCTION(asObjectAddRef),
		asCALL_CDECL_OBJFIRST
	);
	if (r < 0)
	{
		log::error << L"Failed to register AddRef for class \"" << mbstows(fullNameStr) << L"\": " << getErrorString(r) << L" (" << r << L")" << Endl;
	}

	r = m_scriptEngine->RegisterObjectBehaviour(
		className.c_str(),
		asBEHAVE_RELEASE,
		"void f()",
		asFUNCTION(asObjectRelease),
		asCALL_CDECL_OBJFIRST
	);
	if (r < 0)
	{
		log::error << L"Failed to register Release for class \"" << mbstows(fullNameStr) << L"\": " << getErrorString(r) << L" (" << r << L")" << Endl;
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

	// First, register Scalar as a value type since many classes use it
	// This must be done before registering members of other classes
	m_scriptEngine->SetDefaultNamespace("traktor");

	int r = m_scriptEngine->RegisterObjectType("Scalar", sizeof(Scalar), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Scalar>());
	if (r < 0)
	{
		log::warning << L"Failed to register Scalar type: " << getErrorString(r) << Endl;
	}
	else
	{
		// Default constructor
		r = m_scriptEngine->RegisterObjectBehaviour("Scalar", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(asScalarDefaultConstructor), asCALL_CDECL_OBJFIRST);
		if (r < 0) log::warning << L"Failed to register Scalar default constructor: " << getErrorString(r) << Endl;

		// Constructor from float
		r = m_scriptEngine->RegisterObjectBehaviour("Scalar", asBEHAVE_CONSTRUCT, "void f(float)", asFUNCTION(asScalarFloatConstructor), asCALL_CDECL_OBJFIRST);
		if (r < 0) log::warning << L"Failed to register Scalar(float) constructor: " << getErrorString(r) << Endl;

		// Copy constructor
		r = m_scriptEngine->RegisterObjectBehaviour("Scalar", asBEHAVE_CONSTRUCT, "void f(const Scalar &in)", asFUNCTION(asScalarCopyConstructor), asCALL_CDECL_OBJFIRST);
		if (r < 0) log::warning << L"Failed to register Scalar copy constructor: " << getErrorString(r) << Endl;

		// Destructor (trivial, but required)
		r = m_scriptEngine->RegisterObjectBehaviour("Scalar", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(asScalarDestructor), asCALL_CDECL_OBJFIRST);
		if (r < 0) log::warning << L"Failed to register Scalar destructor: " << getErrorString(r) << Endl;

		// Assignment operator
		r = m_scriptEngine->RegisterObjectMethod("Scalar", "Scalar& opAssign(const Scalar &in)", asFUNCTION(asScalarAssign), asCALL_CDECL_OBJFIRST);
		if (r < 0) log::warning << L"Failed to register Scalar opAssign: " << getErrorString(r) << Endl;

		// Conversion to float
		r = m_scriptEngine->RegisterObjectMethod("Scalar", "float opImplConv() const", asFUNCTION(asScalarToFloat), asCALL_CDECL_OBJFIRST);
		if (r < 0) log::warning << L"Failed to register Scalar float conversion: " << getErrorString(r) << Endl;

		// Arithmetic operators
		r = m_scriptEngine->RegisterObjectMethod("Scalar", "Scalar opAdd(const Scalar &in) const", asFUNCTION(asScalarAdd), asCALL_CDECL_OBJFIRST);
		if (r < 0) log::warning << L"Failed to register Scalar opAdd: " << getErrorString(r) << Endl;

		r = m_scriptEngine->RegisterObjectMethod("Scalar", "Scalar opSub(const Scalar &in) const", asFUNCTION(asScalarSub), asCALL_CDECL_OBJFIRST);
		if (r < 0) log::warning << L"Failed to register Scalar opSub: " << getErrorString(r) << Endl;

		r = m_scriptEngine->RegisterObjectMethod("Scalar", "Scalar opMul(const Scalar &in) const", asFUNCTION(asScalarMul), asCALL_CDECL_OBJFIRST);
		if (r < 0) log::warning << L"Failed to register Scalar opMul: " << getErrorString(r) << Endl;

		r = m_scriptEngine->RegisterObjectMethod("Scalar", "Scalar opDiv(const Scalar &in) const", asFUNCTION(asScalarDiv), asCALL_CDECL_OBJFIRST);
		if (r < 0) log::warning << L"Failed to register Scalar opDiv: " << getErrorString(r) << Endl;

		// Comparison operators
		r = m_scriptEngine->RegisterObjectMethod("Scalar", "bool opEquals(const Scalar &in) const", asFUNCTION(asScalarEquals), asCALL_CDECL_OBJFIRST);
		if (r < 0) log::warning << L"Failed to register Scalar opEquals: " << getErrorString(r) << Endl;

		r = m_scriptEngine->RegisterObjectMethod("Scalar", "int opCmp(const Scalar &in) const", asFUNCTION(asScalarCmp), asCALL_CDECL_OBJFIRST);
		if (r < 0) log::warning << L"Failed to register Scalar opCmp: " << getErrorString(r) << Endl;
	}

	m_scriptEngine->SetDefaultNamespace("");

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

		// Register constants as global properties within the class namespace
		// Similar to static methods, we use the class namespace trick (e.g., "traktor::ClassName::CONSTANT")
		// Note: We need to keep constant values alive for the lifetime of the script engine,
		// so we store them in the RegisteredClass structure
		const uint32_t constantCount = runtimeClass->getConstantCount();
		for (uint32_t i = 0; i < constantCount; ++i)
		{
			const std::string constantName = runtimeClass->getConstantName(i);
			const Any constantValue = runtimeClass->getConstantValue(i);

			// Set namespace to "namespace::ClassName" to make it accessible as "ClassName::CONSTANT"
			std::string constantNamespace = namespaceName.empty() ? className : namespaceName + "::" + className;
			r = m_scriptEngine->SetDefaultNamespace(constantNamespace.c_str());
			if (r < 0)
			{
				log::warning << L"Failed to set namespace \"" << mbstows(constantNamespace) << L"\" for constant \"" << mbstows(constantName) << L"\": " << getErrorString(r) << L" (" << r << L")" << Endl;
				m_scriptEngine->SetDefaultNamespace(namespaceName.c_str()); // Restore class namespace
				continue;
			}

			// Store constant value in the RegisteredClass to keep it alive
			// Then register it based on its type
			void* constantPtr = nullptr;
			std::string constantDecl;

			if (constantValue.isInt32())
			{
				ConstantValue cv;
				cv.i32 = constantValue.getInt32Unsafe();
				rc.constantValues.push_back(cv);
				constantPtr = &rc.constantValues.back().i32;
				constantDecl = "const int " + constantName;
			}
			else if (constantValue.isInt64())
			{
				ConstantValue cv;
				cv.i64 = constantValue.getInt64Unsafe();
				rc.constantValues.push_back(cv);
				constantPtr = &rc.constantValues.back().i64;
				constantDecl = "const int64 " + constantName;
			}
			else if (constantValue.isFloat())
			{
				ConstantValue cv;
				cv.f32 = constantValue.getFloatUnsafe();
				rc.constantValues.push_back(cv);
				constantPtr = &rc.constantValues.back().f32;
				constantDecl = "const float " + constantName;
			}
			else if (constantValue.isBoolean())
			{
				ConstantValue cv;
				cv.b = constantValue.getBooleanUnsafe();
				rc.constantValues.push_back(cv);
				constantPtr = &rc.constantValues.back().b;
				constantDecl = "const bool " + constantName;
			}
			else if (constantValue.isObject())
			{
				// For object constants, we need to register them as object handles
				// Store the object in the constant objects array
				ITypedObject* obj = constantValue.getObjectUnsafe();
				if (obj)
				{
					// Get the object's type name and convert to AngelScript format
					const TypeInfo& typeInfo = type_of(obj);
					std::wstring typeName = typeInfo.getName();
					std::string typeNameStr = wstombs(typeName);

					// Replace '.' with '::'
					size_t pos = 0;
					while ((pos = typeNameStr.find('.', pos)) != std::string::npos)
					{
						typeNameStr.replace(pos, 1, "::");
						pos += 2;
					}

					// Store raw pointer in constantObjectPointers
					rc.constantObjectPointers.push_back(obj);
					constantPtr = &rc.constantObjectPointers.back();
					constantDecl = "const " + typeNameStr + "@ " + constantName;

					// Also keep a strong reference to prevent deletion
					rc.constantObjects.push_back(obj);
				}
				else
				{
					log::info << L"Skipped constant \"" << mbstows(constantName) << L"\" for class \"" << mbstows(fullNameStr) << L"\" (null object constant)" << Endl;
					m_scriptEngine->SetDefaultNamespace(namespaceName.c_str());
					continue;
				}
			}
			else
			{
				log::info << L"Skipped constant \"" << mbstows(constantName) << L"\" for class \"" << mbstows(fullNameStr) << L"\" (unsupported constant type)" << Endl;
				m_scriptEngine->SetDefaultNamespace(namespaceName.c_str());
				continue;
			}

			r = m_scriptEngine->RegisterGlobalProperty(constantDecl.c_str(), constantPtr);
			if (r < 0)
			{
				log::warning << L"Failed to register constant \"" << mbstows(constantName) << L"\" for class \"" << mbstows(fullNameStr) << L"\": " << getErrorString(r) << L" (" << r << L")" << Endl;
			}

			// Restore namespace to class level
			m_scriptEngine->SetDefaultNamespace(namespaceName.c_str());
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

			// Log if all signatures were skipped due to unconvertible types
			if (signatures.empty())
			{
				log::info << L"Skipped static method \"" << mbstows(methodName) << L"\" for class \"" << mbstows(fullNameStr) << L"\" (all signatures contain unconvertible types)" << Endl;
				continue;
			}

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

		// Register instance methods
		const uint32_t methodCount = runtimeClass->getMethodCount();
		for (uint32_t i = 0; i < methodCount; ++i)
		{
			const std::string methodName = runtimeClass->getMethodName(i);
			const IRuntimeDispatch* dispatch = runtimeClass->getMethodDispatch(i);

#if defined(T_NEED_RUNTIME_SIGNATURE)
			// Build method signatures from dispatch runtime signature (handles overloads)
			AlignedVector< std::string > signatures = convertRuntimeSignaturesToAngelScript(dispatch, methodName, true);

			// Log if all signatures were skipped due to unconvertible types
			if (signatures.empty())
			{
				log::info << L"Skipped instance method \"" << mbstows(methodName) << L"\" for class \"" << mbstows(fullNameStr) << L"\" (all signatures contain unconvertible types)" << Endl;
				continue;
			}

			// Register each overload as an instance method
			for (const auto& signature : signatures)
			{
				r = m_scriptEngine->RegisterObjectMethod(
					className.c_str(),
					signature.c_str(),
					asFUNCTION(asGenericInstanceMethodWrapper),
					asCALL_GENERIC,
					const_cast<void*>(static_cast<const void*>(dispatch))
				);
				if (r < 0)
				{
					log::warning << L"Failed to register instance method \"" << mbstows(methodName) << L"\" with signature \"" << mbstows(signature) << L"\" for class \"" << mbstows(fullNameStr) << L"\": " << getErrorString(r) << L" (" << r << L")" << Endl;
				}
			}
#else
			log::error << L"AngelScript backend requires T_NEED_RUNTIME_SIGNATURE to be defined for class registration" << Endl;
			m_scriptEngine->SetDefaultNamespace("");
			return;
#endif
		}

		// Register properties
		const uint32_t propertyCount = runtimeClass->getPropertiesCount();
		for (uint32_t i = 0; i < propertyCount; ++i)
		{
			const std::string propertyName = runtimeClass->getPropertyName(i);
			const IRuntimeDispatch* getDispatch = runtimeClass->getPropertyGetDispatch(i);
			const IRuntimeDispatch* setDispatch = runtimeClass->getPropertySetDispatch(i);

#if defined(T_NEED_RUNTIME_SIGNATURE)
			// Register getter if available
			if (getDispatch)
			{
				// Get the property type from the getter's return type
				AlignedVector< std::string > getSignatures = convertRuntimeSignaturesToAngelScript(getDispatch, "get_" + propertyName, true);
				if (getSignatures.empty())
				{
					// Log if property was skipped due to unconvertible type
					log::info << L"Skipped property \"" << mbstows(propertyName) << L"\" for class \"" << mbstows(fullNameStr) << L"\" (property type contains unconvertible types)" << Endl;
					continue;
				}

				if (!getSignatures.empty())
				{
					// Extract return type from first signature (e.g., "ReturnType get_propertyName()")
					const std::string& getSig = getSignatures[0];
					size_t spacePos = getSig.find(' ');
					if (spacePos != std::string::npos)
					{
						std::string propertyType = getSig.substr(0, spacePos);

						// Skip if property type couldn't be determined
						if (!propertyType.empty())
						{
							// Register getter method
							std::string getterSig = propertyType + " get_" + propertyName + "()";
							r = m_scriptEngine->RegisterObjectMethod(
								className.c_str(),
								getterSig.c_str(),
								asFUNCTION(asGenericPropertyGetWrapper),
								asCALL_GENERIC,
								const_cast<void*>(static_cast<const void*>(getDispatch))
							);
							if (r < 0)
							{
								log::warning << L"Failed to register property getter \"" << mbstows(propertyName) << L"\" with signature \"" << mbstows(getterSig) << L"\" for class \"" << mbstows(fullNameStr) << L"\": " << getErrorString(r) << L" (" << r << L")" << Endl;
							}

							// Register setter if available
							if (setDispatch)
							{
								std::string setterSig = "void set_" + propertyName + "(" + propertyType + ")";
								r = m_scriptEngine->RegisterObjectMethod(
									className.c_str(),
									setterSig.c_str(),
									asFUNCTION(asGenericPropertySetWrapper),
									asCALL_GENERIC,
									const_cast<void*>(static_cast<const void*>(setDispatch))
								);
								if (r < 0)
								{
									log::warning << L"Failed to register property setter \"" << mbstows(propertyName) << L"\" with signature \"" << mbstows(setterSig) << L"\" for class \"" << mbstows(fullNameStr) << L"\": " << getErrorString(r) << L" (" << r << L")" << Endl;
								}
							}
						}
					}
				}
			}
#else
			log::error << L"AngelScript backend requires T_NEED_RUNTIME_SIGNATURE to be defined for class registration" << Endl;
			m_scriptEngine->SetDefaultNamespace("");
			return;
#endif
		}

		// Register operators
		// Note: Operator dispatches don't have runtime signature information (signature() returns empty),
		// but like Lua, we can register them as generic wrappers that handle runtime dispatch.
		// The dispatch->invoke() will try each overload and find the one that accepts the argument types.
		struct OperatorMapping
		{
			IRuntimeClass::Operator traktorOp;
			const char* asMethodName;
		};

		const OperatorMapping operators[] = {
			{ IRuntimeClass::Operator::Add, "opAdd" },
			{ IRuntimeClass::Operator::Subtract, "opSub" },
			{ IRuntimeClass::Operator::Multiply, "opMul" },
			{ IRuntimeClass::Operator::Divide, "opDiv" }
		};

		for (const auto& op : operators)
		{
			const IRuntimeDispatch* dispatch = runtimeClass->getOperatorDispatch(op.traktorOp);
			if (!dispatch)
				continue;

			// Register operator with return type matching the class type and accepting the class type as parameter
			// This is a common pattern for operators - the dispatch will handle overload resolution at runtime
			// Note: Since we register types as asOBJ_REF (reference/handle types), we need to use @ for handles
			// AngelScript requires 'const' at the end for const methods
			std::string signature = className + "@ " + op.asMethodName + "(const " + className + "@) const";

			r = m_scriptEngine->RegisterObjectMethod(
				className.c_str(),
				signature.c_str(),
				asFUNCTION(asGenericInstanceMethodWrapper),
				asCALL_GENERIC,
				const_cast<void*>(static_cast<const void*>(dispatch))
			);
			if (r < 0)
			{
				log::info << L"Skipped operator \"" << mbstows(op.asMethodName) << L"\" for class \"" << mbstows(fullNameStr) << L"\": " << getErrorString(r) << L" (" << r << L")" << Endl;
			}
		}

		// Reset to global namespace
		m_scriptEngine->SetDefaultNamespace("");
	}

	// Dump registration summary
	//dumpRegistration();
}

void ScriptManagerAngelScript::dumpRegistration()
{
	log::info << L"========== AngelScript Registration Summary ==========" << Endl;

	// Get total counts
	asUINT typeCount = m_scriptEngine->GetObjectTypeCount();
	asUINT globalFuncCount = m_scriptEngine->GetGlobalFunctionCount();
	asUINT globalPropCount = m_scriptEngine->GetGlobalPropertyCount();

	log::info << L"Total Types: " << typeCount << Endl;
	log::info << L"Total Global Functions: " << globalFuncCount << Endl;
	log::info << L"Total Global Properties: " << globalPropCount << Endl;
	log::info << L"" << Endl;

	// Dump all registered types
	for (asUINT i = 0; i < typeCount; i++)
	{
		asITypeInfo* typeInfo = m_scriptEngine->GetObjectTypeByIndex(i);
		if (!typeInfo)
			continue;

		const char* typeName = typeInfo->GetName();
		const char* namespaceName = typeInfo->GetNamespace();

		std::string fullName;
		if (namespaceName && namespaceName[0] != '\0')
			fullName = std::string(namespaceName) + "::" + typeName;
		else
			fullName = typeName;

		log::info << L"Type: " << mbstows(fullName) << Endl;

		// Dump methods (including operators)
		asUINT methodCount = typeInfo->GetMethodCount();
		if (methodCount > 0)
		{
			log::info << L"  Methods (" << methodCount << L"):" << Endl;
			for (asUINT m = 0; m < methodCount; m++)
			{
				asIScriptFunction* func = typeInfo->GetMethodByIndex(m);
				if (func)
				{
					const char* decl = func->GetDeclaration(false, false, false);
					std::string declStr(decl);

					// Check if this is an operator method
					bool isOperator = (declStr.find("opAdd") != std::string::npos ||
					                   declStr.find("opSub") != std::string::npos ||
					                   declStr.find("opMul") != std::string::npos ||
					                   declStr.find("opDiv") != std::string::npos ||
					                   declStr.find("opEquals") != std::string::npos ||
					                   declStr.find("opCmp") != std::string::npos ||
					                   declStr.find("opNeg") != std::string::npos);

					if (isOperator)
						log::info << L"    " << mbstows(decl) << L" [operator]" << Endl;
					else
						log::info << L"    " << mbstows(decl) << Endl;
				}
			}
		}

		// Dump properties
		asUINT propCount = typeInfo->GetPropertyCount();
		if (propCount > 0)
		{
			log::info << L"  Properties (" << propCount << L"):" << Endl;
			for (asUINT p = 0; p < propCount; p++)
			{
				const char* propName;
				int typeId;
				bool isPrivate;
				bool isProtected;
				int offset;
				bool isReference;
				asDWORD accessMask;
				int compositeOffset;
				bool isCompositeIndirect;

				typeInfo->GetProperty(p, &propName, &typeId, &isPrivate, &isProtected, &offset, &isReference, &accessMask, &compositeOffset, &isCompositeIndirect);
				log::info << L"    " << mbstows(propName) << Endl;
			}
		}

		log::info << L"" << Endl;
	}

	// Dump global functions (organized by namespace)
	if (globalFuncCount > 0)
	{
		log::info << L"Global Functions (" << globalFuncCount << L"):" << Endl;
		for (asUINT i = 0; i < globalFuncCount; i++)
		{
			asIScriptFunction* func = m_scriptEngine->GetGlobalFunctionByIndex(i);
			if (func)
			{
				const char* decl = func->GetDeclaration(true, true, false);
				log::info << L"  " << mbstows(decl) << Endl;
			}
		}
		log::info << L"" << Endl;
	}

	// Dump global properties (constants)
	if (globalPropCount > 0)
	{
		log::info << L"Global Properties (" << globalPropCount << L"):" << Endl;
		for (asUINT i = 0; i < globalPropCount; i++)
		{
			const char* propName;
			const char* namespaceName;
			int typeId;
			bool isConst;
			const char* configGroup;
			void* pointer;
			asDWORD accessMask;

			m_scriptEngine->GetGlobalPropertyByIndex(i, &propName, &namespaceName, &typeId, &isConst, &configGroup, &pointer, &accessMask);

			std::string fullPropName;
			if (namespaceName && namespaceName[0] != '\0')
				fullPropName = std::string(namespaceName) + "::" + propName;
			else
				fullPropName = propName;

			log::info << L"  " << mbstows(fullPropName) << (isConst ? L" [const]" : L"") << Endl;
		}
		log::info << L"" << Endl;
	}

	log::info << L"=======================================================" << Endl;
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
