/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <angelscript.h>
#include "Core/Io/Utf8Encoding.h"
#include "Core/Log/Log.h"
#include "Core/Misc/String.h"
#include "Core/Misc/TString.h"
#include "Core/Thread/Acquire.h"
#include "Script/AngelScript/ScriptBlobAngelScript.h"
#include "Script/AngelScript/ScriptClassAngelScript.h"
#include "Script/AngelScript/ScriptContextAngelScript.h"
#include "Script/AngelScript/ScriptDelegateAngelScript.h"
#include "Script/AngelScript/ScriptManagerAngelScript.h"
#include "Script/AngelScript/ScriptObjectAngelScript.h"

namespace traktor::script
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.script.ScriptContextAngelScript", ScriptContextAngelScript, IScriptContext)

ScriptContextAngelScript::ScriptContextAngelScript(ScriptManagerAngelScript* scriptManager, asIScriptModule* module, asIScriptContext* context, bool strict)
:	m_scriptManager(scriptManager)
,	m_module(module)
,	m_context(context)
,	m_strict(strict)
,	m_lastSelf(nullptr)
{
	if (m_module)
		m_moduleName = m_module->GetName();
}

ScriptContextAngelScript::~ScriptContextAngelScript()
{
	destroy();
}

void ScriptContextAngelScript::destroy()
{
	if (m_scriptManager)
	{
		Ref< ScriptManagerAngelScript > scriptManager = m_scriptManager;
		m_scriptManager = nullptr;

		scriptManager->lock(this);
		{
			if (m_context)
			{
				m_context->Release();
				m_context = nullptr;
			}

			if (m_module)
			{
				// Discard the module
				asIScriptEngine* engine = scriptManager->getEngine();
				if (engine)
					engine->DiscardModule(m_moduleName.c_str());
				m_module = nullptr;
			}

			scriptManager->collectGarbageFull();
			scriptManager->destroyContext(this);
		}
		scriptManager->unlock();
	}
}

bool ScriptContextAngelScript::load(const IScriptBlob* scriptBlob)
{
	m_scriptManager->lock(this);
	{
		const ScriptBlobAngelScript* scriptBlobAS = mandatory_non_null_type_cast< const ScriptBlobAngelScript* >(scriptBlob);

		// Add script section to module
		int result = m_module->AddScriptSection(
			scriptBlobAS->m_fileName.c_str(),
			scriptBlobAS->m_script.c_str(),
			scriptBlobAS->m_script.length()
		);

		if (result < 0)
		{
			log::error << L"Script context failed to add script section" << Endl;
			m_scriptManager->unlock();
			return false;
		}

		// Build the module
		result = m_module->Build();
		if (result < 0)
		{
			log::error << L"Script context failed to build module" << Endl;
			m_scriptManager->unlock();
			return false;
		}
	}
	m_scriptManager->unlock();
	return true;
}

void ScriptContextAngelScript::setGlobal(const std::string& globalName, const Any& globalValue)
{
	m_scriptManager->lock(this);
	{
		// Get the global variable index
		int index = m_module->GetGlobalVarIndexByName(globalName.c_str());
		if (index < 0)
		{
			log::warning << L"Global variable \"" << mbstows(globalName) << L"\" not found" << Endl;
			m_scriptManager->unlock();
			return;
		}

		// Get variable type and pointer
		int typeId;
		m_module->GetGlobalVar(index, nullptr, nullptr, &typeId);
		void* varPtr = m_module->GetAddressOfGlobalVar(index);

		if (!varPtr)
		{
			log::warning << L"Failed to get address of global variable \"" << mbstows(globalName) << L"\"" << Endl;
			m_scriptManager->unlock();
			return;
		}

		// Set value based on type
		if (typeId == asTYPEID_BOOL)
			*static_cast<bool*>(varPtr) = globalValue.getBooleanUnsafe();
		else if (typeId == asTYPEID_INT8)
			*static_cast<int8_t*>(varPtr) = (int8_t)globalValue.getInt32Unsafe();
		else if (typeId == asTYPEID_INT16)
			*static_cast<int16_t*>(varPtr) = (int16_t)globalValue.getInt32Unsafe();
		else if (typeId == asTYPEID_INT32)
			*static_cast<int32_t*>(varPtr) = globalValue.getInt32Unsafe();
		else if (typeId == asTYPEID_INT64)
			*static_cast<int64_t*>(varPtr) = globalValue.getInt64Unsafe();
		else if (typeId == asTYPEID_FLOAT)
			*static_cast<float*>(varPtr) = globalValue.getFloatUnsafe();
		else if (typeId == asTYPEID_DOUBLE)
			*static_cast<double*>(varPtr) = globalValue.getDoubleUnsafe();
		else if (typeId & asTYPEID_OBJHANDLE)
		{
			ITypedObject* obj = globalValue.getObjectUnsafe();
			*static_cast<ITypedObject**>(varPtr) = obj;
			if (obj)
				obj->addRef(nullptr);
		}
		else
		{
			log::warning << L"Unsupported type for global variable \"" << mbstows(globalName) << L"\"" << Endl;
		}
	}
	m_scriptManager->unlock();
}

Any ScriptContextAngelScript::getGlobal(const std::string& globalName)
{
	Any result;
	m_scriptManager->lock(this);
	{
		// Get the global variable index
		int index = m_module->GetGlobalVarIndexByName(globalName.c_str());
		if (index < 0)
		{
			log::warning << L"Global variable \"" << mbstows(globalName) << L"\" not found" << Endl;
			m_scriptManager->unlock();
			return result;
		}

		// Get variable type and pointer
		int typeId;
		m_module->GetGlobalVar(index, nullptr, nullptr, &typeId);
		void* varPtr = m_module->GetAddressOfGlobalVar(index);

		if (!varPtr)
		{
			log::warning << L"Failed to get address of global variable \"" << mbstows(globalName) << L"\"" << Endl;
			m_scriptManager->unlock();
			return result;
		}

		// Get value based on type
		if (typeId == asTYPEID_BOOL)
			result = Any::fromBoolean(*static_cast<bool*>(varPtr));
		else if (typeId == asTYPEID_INT8)
			result = Any::fromInt32(*static_cast<int8_t*>(varPtr));
		else if (typeId == asTYPEID_INT16)
			result = Any::fromInt32(*static_cast<int16_t*>(varPtr));
		else if (typeId == asTYPEID_INT32)
			result = Any::fromInt32(*static_cast<int32_t*>(varPtr));
		else if (typeId == asTYPEID_INT64)
			result = Any::fromInt64(*static_cast<int64_t*>(varPtr));
		else if (typeId == asTYPEID_FLOAT)
			result = Any::fromFloat(*static_cast<float*>(varPtr));
		else if (typeId == asTYPEID_DOUBLE)
			result = Any::fromDouble(*static_cast<double*>(varPtr));
		else if (typeId & asTYPEID_OBJHANDLE)
		{
			ITypedObject* obj = *static_cast<ITypedObject**>(varPtr);
			result = Any::fromObject(obj);
		}
		else
		{
			log::warning << L"Unsupported type for global variable \"" << mbstows(globalName) << L"\"" << Endl;
		}
	}
	m_scriptManager->unlock();
	return result;
}

Ref< const IRuntimeClass > ScriptContextAngelScript::findClass(const std::string& className)
{
	Ref< const IRuntimeClass > result;
	m_scriptManager->lock(this);
	{
		asITypeInfo* typeInfo = m_module->GetTypeInfoByName(className.c_str());
		if (typeInfo)
			result = ScriptClassAngelScript::createFromTypeInfo(m_scriptManager, this, typeInfo);
	}
	m_scriptManager->unlock();
	return result;
}

bool ScriptContextAngelScript::haveFunction(const std::string& functionName) const
{
	bool result = false;
	if (m_module)
	{
		asIScriptFunction* func = m_module->GetFunctionByName(functionName.c_str());
		result = (func != nullptr);
	}
	return result;
}

Any ScriptContextAngelScript::executeFunction(const std::string& functionName, uint32_t argc, const Any* argv)
{
	Any result;
	m_scriptManager->lock(this);
	{
		asIScriptFunction* func = m_module->GetFunctionByName(functionName.c_str());
		if (!func)
		{
			log::error << L"Function \"" << mbstows(functionName) << L"\" not found" << Endl;
			m_scriptManager->unlock();
			return result;
		}

		// Prepare the context
		m_context->Prepare(func);

		// Set arguments from argv array
		for (uint32_t i = 0; i < argc; ++i)
		{
			const Any& arg = argv[i];

			// Get the argument type from the function signature
			int typeId;
			asDWORD flags;
			func->GetParam(i, &typeId, &flags);

			// Convert Any to AngelScript type based on type ID
			if (typeId == asTYPEID_BOOL)
				m_context->SetArgByte(i, arg.getBooleanUnsafe() ? 1 : 0);
			else if (typeId == asTYPEID_INT8)
				m_context->SetArgByte(i, (asBYTE)arg.getInt32Unsafe());
			else if (typeId == asTYPEID_INT16)
				m_context->SetArgWord(i, (asWORD)arg.getInt32Unsafe());
			else if (typeId == asTYPEID_INT32)
				m_context->SetArgDWord(i, arg.getInt32Unsafe());
			else if (typeId == asTYPEID_INT64)
				m_context->SetArgQWord(i, arg.getInt64Unsafe());
			else if (typeId == asTYPEID_UINT8)
				m_context->SetArgByte(i, (asBYTE)arg.getInt32Unsafe());
			else if (typeId == asTYPEID_UINT16)
				m_context->SetArgWord(i, (asWORD)arg.getInt32Unsafe());
			else if (typeId == asTYPEID_UINT32)
				m_context->SetArgDWord(i, (asDWORD)arg.getInt32Unsafe());
			else if (typeId == asTYPEID_UINT64)
				m_context->SetArgQWord(i, (asQWORD)arg.getInt64Unsafe());
			else if (typeId == asTYPEID_FLOAT)
				m_context->SetArgFloat(i, arg.getFloatUnsafe());
			else if (typeId == asTYPEID_DOUBLE)
				m_context->SetArgDouble(i, arg.getDoubleUnsafe());
			else if (typeId & asTYPEID_OBJHANDLE)
			{
				// Object handle (@)
				ITypedObject* obj = arg.getObjectUnsafe();
				m_context->SetArgObject(i, obj);
			}
			else
			{
				log::warning << L"Unsupported argument type for parameter " << i << L" in function " << mbstows(functionName) << Endl;
			}
		}

		// Execute the function
		int r = m_context->Execute();
		if (r != asEXECUTION_FINISHED)
		{
			if (r == asEXECUTION_EXCEPTION)
			{
				log::error << L"Script exception: " << mbstows(m_context->GetExceptionString()) << Endl;
			}
			m_scriptManager->unlock();
			return result;
		}

		// Get return value and convert to Any
		int returnTypeId = func->GetReturnTypeId();

		if (returnTypeId == asTYPEID_VOID)
		{
			// No return value
		}
		else if (returnTypeId == asTYPEID_BOOL)
			result = Any::fromBoolean(m_context->GetReturnByte() != 0);
		else if (returnTypeId == asTYPEID_INT8)
			result = Any::fromInt32((int8_t)m_context->GetReturnByte());
		else if (returnTypeId == asTYPEID_INT16)
			result = Any::fromInt32((int16_t)m_context->GetReturnWord());
		else if (returnTypeId == asTYPEID_INT32)
			result = Any::fromInt32(m_context->GetReturnDWord());
		else if (returnTypeId == asTYPEID_INT64)
			result = Any::fromInt64(m_context->GetReturnQWord());
		else if (returnTypeId == asTYPEID_UINT8)
			result = Any::fromInt32((uint8_t)m_context->GetReturnByte());
		else if (returnTypeId == asTYPEID_UINT16)
			result = Any::fromInt32((uint16_t)m_context->GetReturnWord());
		else if (returnTypeId == asTYPEID_UINT32)
			result = Any::fromInt32(m_context->GetReturnDWord());
		else if (returnTypeId == asTYPEID_UINT64)
			result = Any::fromInt64(m_context->GetReturnQWord());
		else if (returnTypeId == asTYPEID_FLOAT)
			result = Any::fromFloat(m_context->GetReturnFloat());
		else if (returnTypeId == asTYPEID_DOUBLE)
			result = Any::fromDouble(m_context->GetReturnDouble());
		else if (returnTypeId & asTYPEID_OBJHANDLE)
		{
			// Object handle (@)
			ITypedObject* obj = static_cast<ITypedObject*>(m_context->GetReturnObject());
			result = Any::fromObject(obj);
		}
		else
		{
			log::warning << L"Unsupported return type in function " << mbstows(functionName) << Endl;
		}
	}
	m_scriptManager->unlock();
	return result;
}

Any ScriptContextAngelScript::executeDelegate(ScriptDelegateAngelScript* delegate, uint32_t argc, const Any* argv)
{
	Any result;

	if (!delegate || !delegate->m_function)
		return result;

	m_scriptManager->lock(this);
	{
		asIScriptFunction* func = delegate->m_function;

		// Prepare the context
		m_context->Prepare(func);

		// Set arguments from argv array (same logic as executeFunction)
		for (uint32_t i = 0; i < argc; ++i)
		{
			const Any& arg = argv[i];

			// Get the argument type from the function signature
			int typeId;
			asDWORD flags;
			func->GetParam(i, &typeId, &flags);

			// Convert Any to AngelScript type based on type ID
			if (typeId == asTYPEID_BOOL)
				m_context->SetArgByte(i, arg.getBooleanUnsafe() ? 1 : 0);
			else if (typeId == asTYPEID_INT8)
				m_context->SetArgByte(i, (asBYTE)arg.getInt32Unsafe());
			else if (typeId == asTYPEID_INT16)
				m_context->SetArgWord(i, (asWORD)arg.getInt32Unsafe());
			else if (typeId == asTYPEID_INT32)
				m_context->SetArgDWord(i, arg.getInt32Unsafe());
			else if (typeId == asTYPEID_INT64)
				m_context->SetArgQWord(i, arg.getInt64Unsafe());
			else if (typeId == asTYPEID_UINT8)
				m_context->SetArgByte(i, (asBYTE)arg.getInt32Unsafe());
			else if (typeId == asTYPEID_UINT16)
				m_context->SetArgWord(i, (asWORD)arg.getInt32Unsafe());
			else if (typeId == asTYPEID_UINT32)
				m_context->SetArgDWord(i, (asDWORD)arg.getInt32Unsafe());
			else if (typeId == asTYPEID_UINT64)
				m_context->SetArgQWord(i, (asQWORD)arg.getInt64Unsafe());
			else if (typeId == asTYPEID_FLOAT)
				m_context->SetArgFloat(i, arg.getFloatUnsafe());
			else if (typeId == asTYPEID_DOUBLE)
				m_context->SetArgDouble(i, arg.getDoubleUnsafe());
			else if (typeId & asTYPEID_OBJHANDLE)
			{
				// Object handle (@)
				ITypedObject* obj = arg.getObjectUnsafe();
				m_context->SetArgObject(i, obj);
			}
		}

		// Execute the function
		int r = m_context->Execute();
		if (r != asEXECUTION_FINISHED)
		{
			if (r == asEXECUTION_EXCEPTION)
			{
				log::error << L"Script exception in delegate: " << mbstows(m_context->GetExceptionString()) << Endl;
			}
			m_scriptManager->unlock();
			return result;
		}

		// Get return value and convert to Any (same logic as executeFunction)
		int returnTypeId = func->GetReturnTypeId();

		if (returnTypeId == asTYPEID_VOID)
		{
			// No return value
		}
		else if (returnTypeId == asTYPEID_BOOL)
			result = Any::fromBoolean(m_context->GetReturnByte() != 0);
		else if (returnTypeId == asTYPEID_INT8)
			result = Any::fromInt32((int8_t)m_context->GetReturnByte());
		else if (returnTypeId == asTYPEID_INT16)
			result = Any::fromInt32((int16_t)m_context->GetReturnWord());
		else if (returnTypeId == asTYPEID_INT32)
			result = Any::fromInt32(m_context->GetReturnDWord());
		else if (returnTypeId == asTYPEID_INT64)
			result = Any::fromInt64(m_context->GetReturnQWord());
		else if (returnTypeId == asTYPEID_UINT8)
			result = Any::fromInt32((uint8_t)m_context->GetReturnByte());
		else if (returnTypeId == asTYPEID_UINT16)
			result = Any::fromInt32((uint16_t)m_context->GetReturnWord());
		else if (returnTypeId == asTYPEID_UINT32)
			result = Any::fromInt32(m_context->GetReturnDWord());
		else if (returnTypeId == asTYPEID_UINT64)
			result = Any::fromInt64(m_context->GetReturnQWord());
		else if (returnTypeId == asTYPEID_FLOAT)
			result = Any::fromFloat(m_context->GetReturnFloat());
		else if (returnTypeId == asTYPEID_DOUBLE)
			result = Any::fromDouble(m_context->GetReturnDouble());
		else if (returnTypeId & asTYPEID_OBJHANDLE)
		{
			// Object handle (@)
			ITypedObject* obj = static_cast<ITypedObject*>(m_context->GetReturnObject());
			result = Any::fromObject(obj);
		}
	}
	m_scriptManager->unlock();
	return result;
}

Any ScriptContextAngelScript::executeMethod(ScriptObjectAngelScript* self, const std::string& methodName, uint32_t argc, const Any* argv)
{
	Any result;

	if (!self || !self->m_scriptObject)
		return result;

	m_scriptManager->lock(this);
	{
		asIScriptObject* scriptObject = self->m_scriptObject;
		asITypeInfo* typeInfo = scriptObject->GetObjectType();

		if (!typeInfo)
		{
			m_scriptManager->unlock();
			return result;
		}

		// Find the method by name
		asIScriptFunction* func = typeInfo->GetMethodByName(methodName.c_str());
		if (!func)
		{
			log::error << L"Method \"" << mbstows(methodName) << L"\" not found on script object" << Endl;
			m_scriptManager->unlock();
			return result;
		}

		// Prepare the context
		m_context->Prepare(func);

		// Set the object
		m_context->SetObject(scriptObject);

		// Set arguments from argv array (same logic as executeFunction)
		for (uint32_t i = 0; i < argc; ++i)
		{
			const Any& arg = argv[i];

			// Get the argument type from the function signature
			int typeId;
			asDWORD flags;
			func->GetParam(i, &typeId, &flags);

			// Convert Any to AngelScript type based on type ID
			if (typeId == asTYPEID_BOOL)
				m_context->SetArgByte(i, arg.getBooleanUnsafe() ? 1 : 0);
			else if (typeId == asTYPEID_INT8)
				m_context->SetArgByte(i, (asBYTE)arg.getInt32Unsafe());
			else if (typeId == asTYPEID_INT16)
				m_context->SetArgWord(i, (asWORD)arg.getInt32Unsafe());
			else if (typeId == asTYPEID_INT32)
				m_context->SetArgDWord(i, arg.getInt32Unsafe());
			else if (typeId == asTYPEID_INT64)
				m_context->SetArgQWord(i, arg.getInt64Unsafe());
			else if (typeId == asTYPEID_UINT8)
				m_context->SetArgByte(i, (asBYTE)arg.getInt32Unsafe());
			else if (typeId == asTYPEID_UINT16)
				m_context->SetArgWord(i, (asWORD)arg.getInt32Unsafe());
			else if (typeId == asTYPEID_UINT32)
				m_context->SetArgDWord(i, (asDWORD)arg.getInt32Unsafe());
			else if (typeId == asTYPEID_UINT64)
				m_context->SetArgQWord(i, (asQWORD)arg.getInt64Unsafe());
			else if (typeId == asTYPEID_FLOAT)
				m_context->SetArgFloat(i, arg.getFloatUnsafe());
			else if (typeId == asTYPEID_DOUBLE)
				m_context->SetArgDouble(i, arg.getDoubleUnsafe());
			else if (typeId & asTYPEID_OBJHANDLE)
			{
				// Object handle (@)
				ITypedObject* obj = arg.getObjectUnsafe();
				m_context->SetArgObject(i, obj);
			}
		}

		// Execute the method
		int r = m_context->Execute();
		if (r != asEXECUTION_FINISHED)
		{
			if (r == asEXECUTION_EXCEPTION)
			{
				log::error << L"Script exception in method: " << mbstows(m_context->GetExceptionString()) << Endl;
			}
			m_scriptManager->unlock();
			return result;
		}

		// Get return value and convert to Any (same logic as executeFunction)
		int returnTypeId = func->GetReturnTypeId();

		if (returnTypeId == asTYPEID_VOID)
		{
			// No return value
		}
		else if (returnTypeId == asTYPEID_BOOL)
			result = Any::fromBoolean(m_context->GetReturnByte() != 0);
		else if (returnTypeId == asTYPEID_INT8)
			result = Any::fromInt32((int8_t)m_context->GetReturnByte());
		else if (returnTypeId == asTYPEID_INT16)
			result = Any::fromInt32((int16_t)m_context->GetReturnWord());
		else if (returnTypeId == asTYPEID_INT32)
			result = Any::fromInt32(m_context->GetReturnDWord());
		else if (returnTypeId == asTYPEID_INT64)
			result = Any::fromInt64(m_context->GetReturnQWord());
		else if (returnTypeId == asTYPEID_UINT8)
			result = Any::fromInt32((uint8_t)m_context->GetReturnByte());
		else if (returnTypeId == asTYPEID_UINT16)
			result = Any::fromInt32((uint16_t)m_context->GetReturnWord());
		else if (returnTypeId == asTYPEID_UINT32)
			result = Any::fromInt32(m_context->GetReturnDWord());
		else if (returnTypeId == asTYPEID_UINT64)
			result = Any::fromInt64(m_context->GetReturnQWord());
		else if (returnTypeId == asTYPEID_FLOAT)
			result = Any::fromFloat(m_context->GetReturnFloat());
		else if (returnTypeId == asTYPEID_DOUBLE)
			result = Any::fromDouble(m_context->GetReturnDouble());
		else if (returnTypeId & asTYPEID_OBJHANDLE)
		{
			// Object handle (@)
			ITypedObject* obj = static_cast<ITypedObject*>(m_context->GetReturnObject());
			result = Any::fromObject(obj);
		}
	}
	m_scriptManager->unlock();
	return result;
}

}
