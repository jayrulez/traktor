/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Containers/SmallSet.h"
#include "Core/Timer/Timer.h"
#include "Script/IScriptProfiler.h"

class asIScriptEngine;
class asIScriptContext;

namespace traktor::script
{

class ScriptManagerAngelScript;

/*! AngelScript script profiler
 * \ingroup Script
 */
class ScriptProfilerAngelScript : public IScriptProfiler
{
	T_RTTI_CLASS;

public:
	explicit ScriptProfilerAngelScript(ScriptManagerAngelScript* scriptManager, asIScriptEngine* scriptEngine);

	virtual void addListener(IListener* listener) override final;

	virtual void removeListener(IListener* listener) override final;

	void notifyCallEnter();

	void notifyCallLeave();

private:
	friend class ScriptManagerAngelScript;

	struct ProfileStack
	{
		double timeStamp;
		double childDuration;
	};

	ScriptManagerAngelScript* m_scriptManager;
	asIScriptEngine* m_scriptEngine;
	AlignedVector< ProfileStack > m_stack;
	SmallSet< IListener* > m_listeners;
	Timer m_timer;
};

}
