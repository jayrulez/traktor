/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Thread/Semaphore.h"
#include "Script/IScriptCompiler.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SCRIPT_ANGELSCRIPT_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

class asIScriptEngine;

namespace traktor::script
{

/*! AngelScript script compiler.
 * \ingroup Script
 */
class T_DLLCLASS ScriptCompilerAngelScript : public IScriptCompiler
{
	T_RTTI_CLASS;

public:
	ScriptCompilerAngelScript();

	virtual ~ScriptCompilerAngelScript();

	virtual Ref< IScriptBlob > compile(const std::wstring& fileName, const std::wstring& script, IErrorCallback* errorCallback) const override final;

private:
	mutable Semaphore m_lock;
	asIScriptEngine* m_scriptEngine;
};

}
