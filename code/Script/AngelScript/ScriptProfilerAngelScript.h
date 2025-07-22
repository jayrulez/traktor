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
#include "Core/Containers/AlignedVector.h"
#include "Core/Containers/SmallMap.h"
#include "Core/Containers/SmallSet.h"
#include "Core/Thread/Semaphore.h"
#include "Core/Timer/Timer.h"
#include "Script/IScriptProfiler.h"

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
	explicit ScriptProfilerAngelScript(ScriptManagerAngelScript* scriptManager, asIScriptEngine* engine);

	virtual ~ScriptProfilerAngelScript();

	virtual void addListener(IListener* listener) override final;

	virtual void removeListener(IListener* listener) override final;

private:
	friend class ScriptManagerAngelScript;

	struct FunctionProfile
	{
		std::wstring functionName;
		std::wstring fileName;
		int32_t line;
		uint32_t callCount;
		double inclusiveTime;
		double exclusiveTime;
		double lastEnterTime;
		uint32_t recursionDepth;

		FunctionProfile()
			: line(0)
			, callCount(0)
			, inclusiveTime(0.0)
			, exclusiveTime(0.0)
			, lastEnterTime(0.0)
			, recursionDepth(0)
		{
		}
	};

	struct CallStackEntry
	{
		asIScriptFunction* function;
		double enterTime;
		double childTime; // Time spent in child functions

		CallStackEntry()
			: function(nullptr)
			, enterTime(0.0)
			, childTime(0.0)
		{
		}
	};

	ScriptManagerAngelScript* m_scriptManager;
	asIScriptEngine* m_engine;
	mutable Semaphore m_lock;
	SmallMap< asIScriptFunction*, FunctionProfile > m_profiles;
	AlignedVector< CallStackEntry > m_callStack;
	SmallSet< IListener* > m_listeners;
	Timer m_timer;
	bool m_enabled;
	bool m_capturing;

	// AngelScript profiling callbacks
	static void enterFunctionCallback(asIScriptContext* ctx, void* obj);
	static void leaveFunctionCallback(asIScriptContext* ctx, void* obj);

	void enable(bool enable);
	void setupProfilingCallbacks();
	void clearProfilingCallbacks();
	void sendMeasurements();

	// Helper methods
	double getCurrentTime() const;
	std::wstring getFunctionName(asIScriptFunction* func) const;
	std::wstring getFileName(asIScriptFunction* func) const;
	int32_t getLineNumber(asIScriptFunction* func) const;
};

}