/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "angelscript.h"
#include "Core/Class/IRuntimeDelegate.h"
#include "Core/Class/Any.h"

namespace traktor::script
{

class ScriptContextAngelScript;

/*! AngelScript script delegate wrapper.
 * \ingroup Script
 */
class ScriptDelegateAngelScript : public IRuntimeDelegate
{
	T_RTTI_CLASS;

public:
	explicit ScriptDelegateAngelScript(ScriptContextAngelScript* scriptContext, asIScriptFunction* function);

	virtual ~ScriptDelegateAngelScript();

	virtual Any call(int32_t argc, const Any* argv) override final;

	/*! Get wrapped AngelScript function. */
	asIScriptFunction* getFunction() const { return m_function; }

	/*! Get script context. */
	ScriptContextAngelScript* getScriptContext() const { return m_scriptContext; }

	/*! Check if delegate is valid. */
	bool isValid() const { return m_function != nullptr; }

private:
	ScriptContextAngelScript* m_scriptContext;
	asIScriptFunction* m_function;
};

}