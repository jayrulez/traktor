/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Script/AngelScript/ScriptContextAngelScript.h"

#include "angelscript.h"
#include "Core/Log/Log.h"
#include "Core/Misc/String.h"
#include "Core/Misc/TString.h"
#include "Core/Timer/Profiler.h"
#include "Script/AngelScript/ScriptBlobAngelScript.h"
#include "Script/AngelScript/ScriptManagerAngelScript.h"
#include "Script/AngelScript/ScriptClassAngelScript.h"
#include "Script/AngelScript/ScriptDelegateAngelScript.h"

namespace traktor::script
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.script.ScriptContextAngelScript", ScriptContextAngelScript, IScriptContext)

ScriptContextAngelScript::~ScriptContextAngelScript()
{
	destroy();
}

void ScriptContextAngelScript::destroy()
{
	if (m_scriptManager)
	{
		// Store reference locally as later the garbage
		// collect might recurse this call.
		Ref< ScriptManagerAngelScript > scriptManager = m_scriptManager;
		m_scriptManager = nullptr;

		scriptManager->lock(this);
		{
			// Release the module
			if (m_module)
			{
				m_module->Discard();
				m_module = nullptr;
			}

			// Perform a full garbage collect
			m_engine->GarbageCollect(asGC_FULL_CYCLE);
			scriptManager->destroyContext(this);
		}
		scriptManager->unlock();
	}
}

bool ScriptContextAngelScript::load(const IScriptBlob* scriptBlob)
{
	m_scriptManager->lock(this);

	const ScriptBlobAngelScript* scriptBlobAS = mandatory_non_null_type_cast< const ScriptBlobAngelScript* >(scriptBlob);

	// Create a module to hold the script
	std::string moduleName = scriptBlobAS->m_fileName.empty() ? "DefaultModule" : scriptBlobAS->m_fileName;
	m_module = m_engine->GetModule(moduleName.c_str(), asGM_ALWAYS_CREATE);
	if (!m_module)
	{
		log::error << L"Failed to create AngelScript module" << Endl;
		m_scriptManager->unlock();
		return false;
	}

	// Add the script section
	int result = m_module->AddScriptSection("main", scriptBlobAS->m_script.c_str());
	if (result < 0)
	{
		log::error << L"Failed to add script section to module" << Endl;
		m_scriptManager->unlock();
		return false;
	}

	// Build the module
	result = m_module->Build();
	if (result < 0)
	{
		log::error << L"Failed to build AngelScript module" << Endl;
		m_scriptManager->unlock();
		return false;
	}

	m_scriptManager->unlock();
	return true;
}

void ScriptContextAngelScript::setGlobal(const std::string& globalName, const Any& globalValue)
{
	m_scriptManager->lock(this);

	if (m_module)
	{
		setGlobalVariable(globalName, globalValue);
		m_globals.insert(globalName);
	}

	m_scriptManager->unlock();
}

Any ScriptContextAngelScript::getGlobal(const std::string& globalName)
{
	Any value;
	m_scriptManager->lock(this);

	if (m_module)
		value = getGlobalVariable(globalName);

	m_scriptManager->unlock();
	return value;
}

Ref< const IRuntimeClass > ScriptContextAngelScript::findClass(const std::string& className)
{
	Ref< ScriptClassAngelScript > scriptClass;
	m_scriptManager->lock(this);

	if (m_module)
	{
		// Look for the class type in the module
		asITypeInfo* typeInfo = m_module->GetTypeInfoByName(className.c_str());
		if (typeInfo)
			scriptClass = ScriptClassAngelScript::createFromTypeInfo(m_scriptManager, typeInfo);
	}

	m_scriptManager->unlock();
	return scriptClass;
}

bool ScriptContextAngelScript::haveFunction(const std::string& functionName) const
{
	bool result = false;
	m_scriptManager->lock((ScriptContextAngelScript*)this);

	if (m_module)
	{
		asIScriptFunction* func = findFunction(functionName);
		result = (func != nullptr);
	}

	m_scriptManager->unlock();
	return result;
}

Any ScriptContextAngelScript::executeFunction(const std::string& functionName, uint32_t argc, const Any* argv)
{
	T_PROFILER_SCOPE(L"Script executeFunction");

	Any returnValue;
	m_scriptManager->lock(this);

	if (!m_module)
	{
		m_scriptManager->unlock();
		return returnValue;
	}

	// Find the function
	asIScriptFunction* func = findFunction(functionName);
	if (!func)
	{
		log::error << L"Unable to find function: " << mbstows(functionName) << Endl;
		m_scriptManager->unlock();
		return returnValue;
	}

	// Create a context for execution
	asIScriptContext* ctx = m_engine->CreateContext();
	if (!ctx)
	{
		log::error << L"Failed to create execution context" << Endl;
		m_scriptManager->unlock();
		return returnValue;
	}

	// Prepare the function call
	int result = ctx->Prepare(func);
	if (result < 0)
	{
		log::error << L"Failed to prepare function call" << Endl;
		ctx->Release();
		m_scriptManager->unlock();
		return returnValue;
	}

	// Set arguments - updated to use new method names
	for (uint32_t i = 0; i < argc; ++i)
		m_scriptManager->setArgumentFromAny(ctx, i, argv[i]);

	// Execute the function
	result = ctx->Execute();
	if (result == asEXECUTION_FINISHED)
	{
		// Get return value - updated to use new method names
		int returnTypeId = func->GetReturnTypeId();
		if (returnTypeId != asTYPEID_VOID)
			returnValue = m_scriptManager->getReturnValueAsAny(ctx, returnTypeId);
	}
	else if (result == asEXECUTION_EXCEPTION)
	{
		log::error << L"Script exception: " << mbstows(ctx->GetExceptionString()) << Endl;
	}
	else
	{
		log::error << L"Script execution failed with result: " << result << Endl;
	}

	ctx->Release();
	m_scriptManager->unlock();
	return returnValue;
}

Any ScriptContextAngelScript::executeDelegate(ScriptDelegateAngelScript* delegate, uint32_t argc, const Any* argv)
{
	T_PROFILER_SCOPE(L"Script executeDelegate");

	Any returnValue;
	m_scriptManager->lock(this);

	if (!delegate || !delegate->isValid())
	{
		m_scriptManager->unlock();
		return returnValue;
	}

	asIScriptFunction* func = delegate->getFunction();
	if (!func)
	{
		m_scriptManager->unlock();
		return returnValue;
	}

	// Create a context for execution
	asIScriptContext* ctx = m_engine->CreateContext();
	if (!ctx)
	{
		log::error << L"Failed to create execution context for delegate" << Endl;
		m_scriptManager->unlock();
		return returnValue;
	}

	// Prepare the function call
	int result = ctx->Prepare(func);
	if (result < 0)
	{
		log::error << L"Failed to prepare delegate function call" << Endl;
		ctx->Release();
		m_scriptManager->unlock();
		return returnValue;
	}

	// Set arguments
	for (uint32_t i = 0; i < argc; ++i)
		m_scriptManager->setArgumentFromAny(ctx, i, argv[i]);

	// Execute the function
	result = ctx->Execute();
	if (result == asEXECUTION_FINISHED)
	{
		// Get return value
		int returnTypeId = func->GetReturnTypeId();
		if (returnTypeId != asTYPEID_VOID)
			returnValue = m_scriptManager->getReturnValueAsAny(ctx, returnTypeId);
	}
	else if (result == asEXECUTION_EXCEPTION)
	{
		log::error << L"Script delegate exception: " << mbstows(ctx->GetExceptionString()) << Endl;
	}
	else
	{
		log::error << L"Script delegate execution failed with result: " << result << Endl;
	}

	ctx->Release();
	m_scriptManager->unlock();
	return returnValue;
}

Ref< ScriptDelegateAngelScript > ScriptContextAngelScript::createDelegate(const std::string& functionName)
{
	m_scriptManager->lock(this);

	if (!m_module)
	{
		m_scriptManager->unlock();
		return nullptr;
	}

	// Find the function
	asIScriptFunction* func = findFunction(functionName);
	if (!func)
	{
		m_scriptManager->unlock();
		return nullptr;
	}

	// Create the delegate
	Ref< ScriptDelegateAngelScript > delegate = new ScriptDelegateAngelScript(this, func);

	m_scriptManager->unlock();
	return delegate;
}

ScriptContextAngelScript::ScriptContextAngelScript(ScriptManagerAngelScript* scriptManager, asIScriptEngine* engine, bool strict)
	: m_scriptManager(scriptManager)
	, m_engine(engine)
	, m_module(nullptr)
	, m_strict(strict)
{
}

asIScriptFunction* ScriptContextAngelScript::findFunction(const std::string& functionName) const
{
	if (!m_module)
		return nullptr;

	return m_module->GetFunctionByName(functionName.c_str());
}

void ScriptContextAngelScript::setGlobalVariable(const std::string& name, const Any& value)
{
	if (!m_module)
		return;

	int index = m_module->GetGlobalVarIndexByName(name.c_str());
	if (index >= 0)
	{
		void* ptr = m_module->GetAddressOfGlobalVar(index);
		if (ptr)
		{
			// Convert Any to the appropriate type and set the global variable
			switch (value.getType())
			{
			case Any::Type::Boolean:
				*(bool*)ptr = value.getBooleanUnsafe();
				break;
			case Any::Type::Int32:
				*(int32_t*)ptr = value.getInt32Unsafe();
				break;
			case Any::Type::Int64:
				*(int64_t*)ptr = value.getInt64Unsafe();
				break;
			case Any::Type::Float:
				*(float*)ptr = value.getFloatUnsafe();
				break;
			case Any::Type::Double:
				*(double*)ptr = value.getDoubleUnsafe();
				break;
			case Any::Type::String:
				{
					// Convert to std::string for AngelScript
					std::string narrowStr = value.getStringUnsafe();
					*(std::string*)ptr = narrowStr;
				}
				break;
			case Any::Type::Object:
				*(void**)ptr = value.getObjectUnsafe();
				break;
			}
		}
	}
}

Any ScriptContextAngelScript::getGlobalVariable(const std::string& name) const
{
	if (!m_module)
		return Any();

	int index = m_module->GetGlobalVarIndexByName(name.c_str());
	if (index >= 0)
	{
		void* ptr = m_module->GetAddressOfGlobalVar(index);
		int typeId;
		m_module->GetGlobalVar(index, nullptr, nullptr, &typeId);

		if (ptr)
		{
			// Convert AngelScript type to Any
			if (typeId == asTYPEID_BOOL)
				return Any::fromBoolean(*(bool*)ptr);
			else if (typeId == asTYPEID_INT32)
				return Any::fromInt32(*(int32_t*)ptr);
			else if (typeId == asTYPEID_INT64)
				return Any::fromInt64(*(int64_t*)ptr);
			else if (typeId == asTYPEID_FLOAT)
				return Any::fromFloat(*(float*)ptr);
			else if (typeId == asTYPEID_DOUBLE)
				return Any::fromDouble(*(double*)ptr);
			else if (typeId == m_engine->GetTypeIdByDecl("string"))
			{
				// Convert std::string to wide string for Traktor
				std::string* str = static_cast< std::string* >(ptr);
				std::wstring wstr = mbstows(*str);
				return Any::fromString(wstr);
			}
			else if (typeId & asTYPEID_OBJHANDLE)
			{
				ITypedObject* obj = static_cast< ITypedObject* >(*(void**)ptr);
				return Any::fromObject(obj);
			}
		}
	}

	return Any();
}

}