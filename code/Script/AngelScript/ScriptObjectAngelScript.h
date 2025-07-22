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
#include "Core/Class/IRuntimeObject.h"

namespace traktor::script
{

class ScriptManagerAngelScript;

/*! AngelScript script object runtime interface.
 * \ingroup Script
 */
class ScriptObjectAngelScript : public IRuntimeObject
{
	T_RTTI_CLASS;

public:
	explicit ScriptObjectAngelScript(ScriptManagerAngelScript* scriptManager, asIScriptObject* scriptObject);

	virtual ~ScriptObjectAngelScript();

	virtual Ref< const IRuntimeClass > getRuntimeClass() const override final;

	/*! Get the underlying AngelScript object. */
	asIScriptObject* getScriptObject() const { return m_scriptObject; }

	/*! Get the AngelScript type info for this object. */
	asITypeInfo* getAngelScriptTypeInfo() const;

	/*! Check if the underlying AngelScript object is valid. */
	bool isValid() const { return m_scriptObject != nullptr; }

private:
	ScriptManagerAngelScript* m_scriptManager;
	asIScriptObject* m_scriptObject;
};

}