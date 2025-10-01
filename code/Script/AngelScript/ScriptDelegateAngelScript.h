/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Class/IRuntimeDelegate.h"

class asIScriptFunction;

namespace traktor::script
{

class ScriptContextAngelScript;

/*! AngelScript script delegate runtime interface.
 * \ingroup Script
 */
class ScriptDelegateAngelScript : public IRuntimeDelegate
{
	T_RTTI_CLASS;

public:
	explicit ScriptDelegateAngelScript(ScriptContextAngelScript* context, asIScriptFunction* function);

	virtual ~ScriptDelegateAngelScript();

	virtual Any call(int32_t argc, const Any* argv) override final;

	asIScriptFunction* getFunction() const { return m_function; }

private:
	ScriptContextAngelScript* m_context;
	asIScriptFunction* m_function;
};

}
