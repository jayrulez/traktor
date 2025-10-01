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
#include "Script/IErrorCallback.h"
#include "Script/AngelScript/ScriptBlobAngelScript.h"
#include "Script/AngelScript/ScriptCompilerAngelScript.h"

namespace traktor::script
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.script.ScriptCompilerAngelScript", 0, ScriptCompilerAngelScript, IScriptCompiler)

ScriptCompilerAngelScript::ScriptCompilerAngelScript()
:	m_scriptEngine(nullptr)
{
	m_scriptEngine = asCreateScriptEngine();
}

ScriptCompilerAngelScript::~ScriptCompilerAngelScript()
{
	if (m_scriptEngine)
	{
		m_scriptEngine->ShutDownAndRelease();
		m_scriptEngine = nullptr;
	}
}

Ref< IScriptBlob > ScriptCompilerAngelScript::compile(const std::wstring& fileName, const std::wstring& script, IErrorCallback* errorCallback) const
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

	const std::string moduleFileName = wstombs(Utf8Encoding(), fileName);
	const std::string text = wstombs(Utf8Encoding(), script);

	// Create a temporary module for compilation validation
	asIScriptModule* module = m_scriptEngine->GetModule("__compile_test__", asGM_ALWAYS_CREATE);
	if (!module)
	{
		if (errorCallback)
			errorCallback->syntaxError(fileName, 0, L"Failed to create compilation module");
		else
			log::error << fileName << L": Failed to create compilation module" << Endl;
		return nullptr;
	}

	// Try to add and build the script section
	int result = module->AddScriptSection(moduleFileName.c_str(), text.c_str(), text.length());
	if (result < 0)
	{
		if (errorCallback)
			errorCallback->syntaxError(fileName, 0, L"Failed to add script section");
		else
			log::error << fileName << L": Failed to add script section" << Endl;
		m_scriptEngine->DiscardModule("__compile_test__");
		return nullptr;
	}

	result = module->Build();
	if (result < 0)
	{
		// AngelScript will have already reported errors through its message callback
		// For now, we'll provide a generic error
		if (errorCallback)
			errorCallback->syntaxError(fileName, 0, L"Script compilation failed");
		else
			log::error << fileName << L": Script compilation failed" << Endl;
		m_scriptEngine->DiscardModule("__compile_test__");
		return nullptr;
	}

	// Discard the test module
	m_scriptEngine->DiscardModule("__compile_test__");

	// Create blob with source text (similar to Lua approach)
	Ref< ScriptBlobAngelScript > blob = new ScriptBlobAngelScript();
	blob->m_fileName = moduleFileName;
	blob->m_script = text;

	return blob;
}

}
