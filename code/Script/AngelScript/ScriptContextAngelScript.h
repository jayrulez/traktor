/*
 * TRAKTOR
 * Copyright (c) 2022-2023 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Containers/SmallSet.h"
#include "Script/IScriptContext.h"

class asIScriptModule;
class asIScriptContext;

namespace traktor::script
{

class ScriptDelegateAngelScript;
class ScriptManagerAngelScript;
class ScriptObjectAngelScript;

/*! AngelScript scripting context.
 * \ingroup Script
 */
class ScriptContextAngelScript : public IScriptContext
{
	T_RTTI_CLASS;

public:
	virtual ~ScriptContextAngelScript();

	virtual void destroy() override final;

	virtual bool load(const IScriptBlob* scriptBlob) override final;

	virtual void setGlobal(const std::string& globalName, const Any& globalValue) override final;

	virtual Any getGlobal(const std::string& globalName) override final;

	virtual Ref< const IRuntimeClass > findClass(const std::string& className) override final;

	virtual bool haveFunction(const std::string& functionName) const override final;

	virtual Any executeFunction(const std::string& functionName, uint32_t argc, const Any* argv) override final;

	Any executeDelegate(ScriptDelegateAngelScript* delegate, uint32_t argc, const Any* argv);

	Any executeMethod(ScriptObjectAngelScript* self, const std::string& methodName, uint32_t argc, const Any* argv);

	asIScriptModule* getModule() { return m_module; }

private:
	friend class ScriptDebuggerAngelScript;
	friend class ScriptManagerAngelScript;

	ScriptManagerAngelScript* m_scriptManager;
	asIScriptModule* m_module;
	asIScriptContext* m_context;
	bool m_strict;
	const Object* m_lastSelf;
	SmallSet< std::string > m_globals;
	std::string m_moduleName;

	explicit ScriptContextAngelScript(ScriptManagerAngelScript* scriptManager, asIScriptModule* module, asIScriptContext* context, bool strict);
};

}
