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
		// TODO: Implement global variable setting in AngelScript
		// AngelScript doesn't have a direct equivalent to Lua's global environment
		// This might require registering global properties or using a different approach
	}
	m_scriptManager->unlock();
}

Any ScriptContextAngelScript::getGlobal(const std::string& globalName)
{
	Any result;
	m_scriptManager->lock(this);
	{
		// TODO: Implement global variable getting in AngelScript
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

		// TODO: Set arguments from argv array
		// This requires converting Any types to AngelScript types

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

		// TODO: Get return value and convert to Any
	}
	m_scriptManager->unlock();
	return result;
}

Any ScriptContextAngelScript::executeDelegate(ScriptDelegateAngelScript* delegate, uint32_t argc, const Any* argv)
{
	// TODO: Implement delegate execution
	return Any();
}

Any ScriptContextAngelScript::executeMethod(ScriptObjectAngelScript* self, const std::string& methodName, uint32_t argc, const Any* argv)
{
	// TODO: Implement method execution
	return Any();
}

}
