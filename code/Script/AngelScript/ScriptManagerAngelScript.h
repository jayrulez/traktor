/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/RefArray.h"
#include "Core/Containers/AlignedVector.h"
#include "Script/IScriptManager.h"
#include "Core/Thread/Semaphore.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SCRIPT_ANGELSCRIPT_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

class asIScriptEngine;

namespace traktor
{

class Any;

}

namespace traktor::script
{

class ScriptContextAngelScript;
class ScriptDebuggerAngelScript;
class ScriptProfilerAngelScript;

/*! AngelScript script manager.
 * \ingroup Script
 */
class T_DLLCLASS ScriptManagerAngelScript : public IScriptManager
{
	T_RTTI_CLASS;

public:
	ScriptManagerAngelScript();

	virtual ~ScriptManagerAngelScript();

	virtual void destroy() override final;

	virtual void registerClass(IRuntimeClass* runtimeClass) override final;

	void completeRegistration();

	virtual Ref< IScriptContext > createContext(bool strict) override final;

	virtual Ref< IScriptDebugger > createDebugger() override final;

	virtual Ref< IScriptProfiler > createProfiler() override final;

	virtual void collectGarbage(bool full) override final;

	virtual void getStatistics(ScriptStatistics& outStatistics) const override final;

	void pushObject(ITypedObject* object);

	void pushAny(const Any& any);

	void pushAny(const Any* anys, int32_t count);

	Any toAny(int32_t index);

	void toAny(int32_t base, int32_t count, Any* outAnys);

	void lock(ScriptContextAngelScript* context)
	{
		m_lock.wait();
		m_lockContext = context;
	}

	void unlock()
	{
		m_lock.release();
	}

	asIScriptEngine* getEngine() { return m_scriptEngine; }

private:
	friend class ScriptContextAngelScript;
	friend class ScriptDebuggerAngelScript;
	friend class ScriptProfilerAngelScript;

	struct RegisteredClass
	{
		Ref< const IRuntimeClass > runtimeClass;
		int32_t typeId;
	};

	asIScriptEngine* m_scriptEngine;
	mutable Semaphore m_lock;
	static ScriptManagerAngelScript* ms_instance;
	ScriptContextAngelScript* m_lockContext;
	AlignedVector< RegisteredClass > m_classRegistry;
	RefArray< ScriptContextAngelScript > m_contexts;
	Ref< ScriptDebuggerAngelScript > m_debugger;
	Ref< ScriptProfilerAngelScript > m_profiler;
	size_t m_totalMemoryUse;

	void destroyContext(ScriptContextAngelScript* context);

	void collectGarbageFull();

	void collectGarbagePartial();
};

}
