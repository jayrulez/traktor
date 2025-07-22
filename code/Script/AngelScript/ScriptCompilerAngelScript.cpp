/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Script/AngelScript/ScriptCompilerAngelScript.h"

#include "angelscript.h"
#include "Core/Io/Utf8Encoding.h"
#include "Core/Log/Log.h"
#include "Core/Memory/IAllocator.h"
#include "Core/Misc/String.h"
#include "Core/Thread/Acquire.h"
#include "Script/AngelScript/ScriptBlobAngelScript.h"
#include "Script/AngelScript/ScriptUtilitiesAngelScript.h"
#include "Script/IErrorCallback.h"
#include "Script/AngelScript/scriptstdstring.h"
#include "Script/AngelScript/scriptarray.h"

namespace traktor::script
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.script.ScriptCompilerAngelScript", 0, ScriptCompilerAngelScript, IScriptCompiler)

ScriptCompilerAngelScript::ScriptCompilerAngelScript()
	: m_engine(nullptr)
{
	m_engine = asCreateScriptEngine();
	if (m_engine)
		setupCompilationEngine();
}

ScriptCompilerAngelScript::~ScriptCompilerAngelScript()
{
	if (m_engine)
	{
		cleanupCompilationEngine();
		m_engine->ShutDownAndRelease();
		m_engine = nullptr;
	}
}

Ref< IScriptBlob > ScriptCompilerAngelScript::compile(const std::wstring& fileName, const std::wstring& script, IErrorCallback* errorCallback) const
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_lock);

	if (!m_engine)
	{
		if (errorCallback)
			errorCallback->syntaxError(fileName, 0, L"AngelScript engine not initialized");
		return nullptr;
	}

	const std::string fileNameUtf8 = wstombs(Utf8Encoding(), fileName);
	const std::string scriptUtf8 = wstombs(Utf8Encoding(), script);

	// Set up compilation context for error handling
	CompilationContext compCtx;
	compCtx.errorCallback = errorCallback;
	compCtx.hasErrors = false;
	compCtx.fileName = fileName;

	// Set message callback for compilation
	m_engine->SetMessageCallback(asFUNCTION(compilationMessageCallback), &compCtx, asCALL_CDECL);

	// Create a temporary module for compilation verification
	std::string moduleName = "CompileTest_" + fileNameUtf8;
	asIScriptModule* module = m_engine->GetModule(moduleName.c_str(), asGM_ALWAYS_CREATE);
	if (!module)
	{
		if (errorCallback)
			errorCallback->syntaxError(fileName, 0, L"Failed to create compilation module");
		m_engine->ClearMessageCallback();
		return nullptr;
	}

	// Add script section
	int result = module->AddScriptSection(fileNameUtf8.c_str(), scriptUtf8.c_str());
	if (result < 0)
	{
		if (errorCallback)
			errorCallback->syntaxError(fileName, 0, L"Failed to add script section");
		module->Discard();
		m_engine->ClearMessageCallback();
		return nullptr;
	}

	// Build the module to check for compilation errors
	result = module->Build();

	// Clean up the temporary module
	module->Discard();

	// Clear message callback
	m_engine->ClearMessageCallback();

	if (result < 0 || compCtx.hasErrors)
	{
		// Compilation failed
		return nullptr;
	}

	// Create the script blob
	Ref< ScriptBlobAngelScript > blob = new ScriptBlobAngelScript();
	blob->m_fileName = fileNameUtf8;
	blob->m_script = scriptUtf8;

	return blob;
}

void ScriptCompilerAngelScript::setupCompilationEngine()
{
	if (!m_engine)
		return;

	// Set memory functions
	asSetGlobalMemoryFunctions(scriptAlloc, scriptFree);

	// Set basic engine properties for compilation
	m_engine->SetEngineProperty(asEP_ALLOW_UNSAFE_REFERENCES, true);
	m_engine->SetEngineProperty(asEP_OPTIMIZE_BYTECODE, true);
	m_engine->SetEngineProperty(asEP_COPY_SCRIPT_SECTIONS, true);
	m_engine->SetEngineProperty(asEP_AUTO_GARBAGE_COLLECT, false);

	// Register standard string addon
	RegisterStdString(m_engine);
	RegisterScriptArray(m_engine, true);

	// todo: register Any

	// Register standard functions for compilation validation
	registerStandardFunctions(m_engine);
}

void ScriptCompilerAngelScript::cleanupCompilationEngine()
{
	// Any cleanup needed before destroying the engine
	if (m_engine)
		m_engine->GarbageCollect(asGC_FULL_CYCLE);
}

void ScriptCompilerAngelScript::compilationMessageCallback(const asSMessageInfo* msg, void* param)
{
	CompilationContext* ctx = static_cast< CompilationContext* >(param);

	std::wstring section = mbstows(msg->section);
	std::wstring message = mbstows(msg->message);

	if (msg->type == asMSGTYPE_ERROR)
	{
		ctx->hasErrors = true;
		if (ctx->errorCallback)
			ctx->errorCallback->syntaxError(ctx->fileName, msg->row, message);
		else
			log::error << section << L" (" << msg->row << L"): " << message << Endl;
	}
	else if (msg->type == asMSGTYPE_WARNING)
	{
		log::warning << section << L" (" << msg->row << L"): " << message << Endl;
	}
	else if (msg->type == asMSGTYPE_INFORMATION)
	{
		log::info << section << L" (" << msg->row << L"): " << message << Endl;
	}
}

// Static memory allocation functions
void* ScriptCompilerAngelScript::scriptAlloc(size_t size)
{
	return getAllocator()->alloc(size, 16, T_FILE_LINE);
}

void ScriptCompilerAngelScript::scriptFree(void* ptr)
{
	getAllocator()->free(ptr);
}

}