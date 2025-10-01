/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Class/IRuntimeObject.h"

class asIScriptObject;

namespace traktor::script
{

class ScriptManagerAngelScript;
class ScriptContextAngelScript;

/*! AngelScript script object runtime interface.
 * \ingroup Script
 */
class ScriptObjectAngelScript : public IRuntimeObject
{
	T_RTTI_CLASS;

public:
	explicit ScriptObjectAngelScript(ScriptManagerAngelScript* scriptManager, ScriptContextAngelScript* scriptContext, asIScriptObject* scriptObject);

	virtual ~ScriptObjectAngelScript();

	virtual Ref< const IRuntimeClass > getRuntimeClass() const override final;

	asIScriptObject* getScriptObject() const { return m_scriptObject; }

private:
	ScriptManagerAngelScript* m_scriptManager;
	ScriptContextAngelScript* m_scriptContext;
	asIScriptObject* m_scriptObject;
};

}
