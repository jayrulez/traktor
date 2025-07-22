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
#include "Core/RefArray.h"
#include "Core/Thread/Semaphore.h"
#include "Script/IScriptManager.h"
#include "Script/AngelScript/scriptstdstring.h" // For RegisterStdString

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SCRIPT_ANGELSCRIPT_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

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

	virtual Ref< IScriptContext > createContext(bool strict) override final;

	virtual Ref< IScriptDebugger > createDebugger() override final;

	virtual Ref< IScriptProfiler > createProfiler() override final;

	virtual void collectGarbage(bool full) override final;

	virtual void getStatistics(ScriptStatistics& outStatistics) const override final;

	// Argument/return value conversion methods
	void setArgumentFromAny(asIScriptContext* ctx, uint32_t argIndex, const Any& any);
	void setArgumentFromObject(asIScriptContext* ctx, uint32_t argIndex, ITypedObject* object);

	Any getReturnValueAsAny(asIScriptContext* ctx, int returnTypeId);
	ITypedObject* getReturnValueAsObject(asIScriptContext* ctx, int returnTypeId);

	// Generic wrapper conversion utilities
	Any convertAngelScriptArgToAny(asIScriptGeneric* gen, uint32_t argIndex);
	void setAngelScriptReturnValue(asIScriptGeneric* gen, const Any& value);
	ITypedObject* extractObjectFromGeneric(asIScriptGeneric* gen);

	asIScriptEngine* getEngine() const { return m_engine; }

	void lock(ScriptContextAngelScript* context)
	{
		m_lock.wait();
		m_lockContext = context;
	}

	void unlock()
	{
		m_lock.release();
	}

private:
	friend class ScriptContextAngelScript;
	friend class ScriptDebuggerAngelScript;
	friend class ScriptProfilerAngelScript;

	struct RegisteredClass
	{
		Ref< const IRuntimeClass > runtimeClass;
		asITypeInfo* typeInfo;
	};

	asIScriptEngine* m_engine;
	mutable Semaphore m_lock;
	static ScriptManagerAngelScript* ms_instance;
	ScriptContextAngelScript* m_lockContext;
	AlignedVector< RegisteredClass > m_classRegistry;
	RefArray< ScriptContextAngelScript > m_contexts;
	Ref< ScriptDebuggerAngelScript > m_debugger;
	Ref< ScriptProfilerAngelScript > m_profiler;

	void destroyContext(ScriptContextAngelScript* context);

	// AngelScript callbacks
	static void messageCallback(const asSMessageInfo* msg, void* param);
	static void* scriptAlloc(size_t size);
	static void scriptFree(void* ptr);

	// Generic method wrappers for runtime dispatch
	static void genericMethodWrapper(asIScriptGeneric* gen);
	static void genericConstructorWrapper(asIScriptGeneric* gen);
	static void genericPropertyGetWrapper(asIScriptGeneric* gen);
	static void genericPropertySetWrapper(asIScriptGeneric* gen);
	static void genericStaticMethodWrapper(asIScriptGeneric* gen);

	// Object lifecycle callbacks
	static void objectDestructor(void* obj);
	static void objectConstructor(void* obj);
	static void objectAddRef(void* obj);
	static void objectRelease(void* obj);

	// Utility functions
	// Signature conversion utilities
	std::string convertSignatureToAngelScript(const std::string& traktorSignature);
	std::string convertPropertySignatureToAngelScript(const std::string& traktorSignature, const std::string& propertyName, bool isGetter);
	std::string convertTypeToAngelScript(const std::string& traktorType);
	std::string convertReferenceParameters(const std::string& signature);
	std::string extractReturnTypeFromSignature(const std::string& signature);
	std::string extractParameterTypeFromSignature(const std::string& signature);

	// Registration helpers
	void registerMethodsFromReflection(const std::string& className, IRuntimeClass* runtimeClass);
	void registerPropertiesFromReflection(const std::string& className, IRuntimeClass* runtimeClass);
	void registerStaticMethodsFromReflection(const std::string& className, IRuntimeClass* runtimeClass);
	void registerObjectBehaviors(const std::string& className, IRuntimeClass* runtimeClass);

	std::string convertTypeName(const std::wstring& traktorName) const;
};

}