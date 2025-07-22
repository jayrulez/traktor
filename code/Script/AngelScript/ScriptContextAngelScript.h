/*
 * TRAKTOR
 * Copyright (c) 2022-2023 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "angelscript.h"
#include "Core/Containers/SmallSet.h"
#include "Script/IScriptContext.h"

namespace traktor::script
{

class ScriptManagerAngelScript;
class ScriptDelegateAngelScript;

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

	/*! Execute a script delegate function.
	 * \param delegate Script delegate to execute.
	 * \param argc Number of arguments.
	 * \param argv Array of arguments.
	 * \return Return value from script function.
	 */
	Any executeDelegate(ScriptDelegateAngelScript* delegate, uint32_t argc, const Any* argv);

	/*! Create a delegate from a script function.
	 * \param functionName Name of the script function.
	 * \return Script delegate or null if function not found.
	 */
	Ref< ScriptDelegateAngelScript > createDelegate(const std::string& functionName);

private:
	friend class ScriptManagerAngelScript;

	ScriptManagerAngelScript* m_scriptManager;
	asIScriptEngine* m_engine;
	asIScriptModule* m_module;
	bool m_strict;
	SmallSet< std::string > m_globals;

	explicit ScriptContextAngelScript(ScriptManagerAngelScript* scriptManager, asIScriptEngine* engine, bool strict);

	// Helper functions
	asIScriptFunction* findFunction(const std::string& functionName) const;
	void setGlobalVariable(const std::string& name, const Any& value);
	Any getGlobalVariable(const std::string& name) const;
};

}