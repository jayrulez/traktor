/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Class/AutoVerify.h"
#include "Core/Class/Boxes/BoxedTypeInfo.h"
#include "Core/Class/IRuntimeClass.h"
#include "Core/Class/IRuntimeDispatch.h"
#include "Core/Math/MathUtils.h"
#include "Core/Misc/Save.h"
#include "Core/Misc/Split.h"
#include "Core/Misc/TString.h"
#include "Core/Serialization/ISerializable.h"
#include "Core/Thread/Acquire.h"
#include "Core/Timer/Timer.h"
#include "Script/AngelScript/ScriptClassAngelScript.h"
#include "Script/AngelScript/ScriptContextAngelScript.h"
#include "Script/AngelScript/ScriptDebuggerAngelScript.h"
#include "Script/AngelScript/ScriptDelegateAngelScript.h"
#include "Script/AngelScript/ScriptManagerAngelScript.h"
#include "Script/AngelScript/ScriptObjectAngelScript.h"
#include "Script/AngelScript/ScriptProfilerAngelScript.h"
#include "Script/AngelScript/ScriptUtilitiesAngelScript.h"

#include <cstring>

// Resources
//#include "Resources/Initialization.h"

#define T_USE_ALLOCATOR 1
#define T_LOG_OBJECT_GC 0

namespace traktor::script
{
//namespace
//{
//
//Timer s_timer;
//
//const int32_t c_tableKey_class = -1;
//const int32_t c_tableKey_instance = -2;
//
//inline ITypedObject* toTypedObject(lua_State* luaState, int32_t index)
//{
//	lua_rawgeti(luaState, index, c_tableKey_instance);
//	if (!lua_islightuserdata(luaState, -1))
//	{
//		lua_pop(luaState, 1);
//		return nullptr;
//	}
//
//	ITypedObject* object = (ITypedObject*)lua_touserdata(luaState, -1);
//	if (!object)
//	{
//		lua_pop(luaState, 1);
//		return nullptr;
//	}
//
//	lua_pop(luaState, 1);
//	return object;
//}
//
//inline void getObjectRef(lua_State* L, int32_t objectTableRef, ITypedObject* object)
//{
//	CHECK_LUA_STACK(L, 1);
//	lua_rawgeti(L, LUA_REGISTRYINDEX, objectTableRef);
//	lua_pushinteger(L, lua_Integer(object));
//	lua_rawget(L, -2);
//	lua_remove(L, -2);
//}
//
//inline void putObjectRef(lua_State* L, int32_t objectTableRef, ITypedObject* object)
//{
//	CHECK_LUA_STACK(L, 0);
//	lua_rawgeti(L, LUA_REGISTRYINDEX, objectTableRef);
//	lua_pushinteger(L, lua_Integer(object));
//	lua_pushvalue(L, -3);
//	lua_rawset(L, -3);
//	lua_pop(L, 1);
//}
//
//}

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.script.ScriptManagerAngelScript", 0, ScriptManagerAngelScript, IScriptManager)

ScriptManagerAngelScript* ScriptManagerAngelScript::ms_instance = nullptr;

ScriptManagerAngelScript::ScriptManagerAngelScript()
	: m_defaultAllocFn(nullptr)
	, m_defaultAllocOpaque(nullptr)
	, m_collectStepFrequency(10.0)
	, m_collectSteps(-1)
	, m_collectTargetSteps(0.0f)
	, m_totalMemoryUse(0)
	, m_lastMemoryUse(0)
{
	T_FATAL_ASSERT(ms_instance == nullptr);
	ms_instance = this;

	//s_timer.reset();
}

ScriptManagerAngelScript::~ScriptManagerAngelScript()
{
	T_FATAL_ASSERT(ms_instance == this);
	ms_instance = nullptr;
}

void ScriptManagerAngelScript::destroy()
{
	T_ANONYMOUS_VAR(Ref< ScriptManagerAngelScript >)(this);
	//T_FATAL_ASSERT(m_contexts.empty());

	// Discard all tags from C++ rtti types.
	for (auto& rc : m_classRegistry)
		for (auto& derivedType : rc.runtimeClass->getExportType().findAllOf())
			derivedType->setTag(0);

	//m_debugger = nullptr;
	//m_profiler = nullptr;
}

void ScriptManagerAngelScript::registerClass(IRuntimeClass* runtimeClass)
{
	const TypeInfo& exportType = runtimeClass->getExportType();
	const int32_t classRegistryIndex = int32_t(m_classRegistry.size());

	RegisteredClass& rc = m_classRegistry.push_back();
	rc.runtimeClass = runtimeClass;

	// Create new class.
	if (exportType.getSuper())
	{
		const int32_t superClassId = int32_t(exportType.getSuper()->getTag()) - 1;
		if (superClassId >= 0)
		{
			const RegisteredClass& superClass = m_classRegistry[superClassId];
		}
		else
		{
		}
	}
	else
	{
	}

	// Add static methods.
	const uint32_t staticMethodCount = runtimeClass->getStaticMethodCount();
	for (uint32_t i = 0; i < staticMethodCount; ++i)
	{
		const std::string methodName = runtimeClass->getStaticMethodName(i);
	}

	// Add methods.
	const uint32_t methodCount = runtimeClass->getMethodCount();
	for (uint32_t i = 0; i < methodCount; ++i)
	{
		const std::string methodName = runtimeClass->getMethodName(i);
	}

	// Add properties.
	const uint32_t propertyCount = runtimeClass->getPropertiesCount();
	for (uint32_t i = 0; i < propertyCount; ++i)
	{
		const std::string propertyName = runtimeClass->getPropertyName(i);

	}

	// Add operators.
	{
		const IRuntimeDispatch* addDispatch = runtimeClass->getOperatorDispatch(IRuntimeClass::Operator::Add);
		if (addDispatch)
		{
		}
	}

	{
		const IRuntimeDispatch* subDispatch = runtimeClass->getOperatorDispatch(IRuntimeClass::Operator::Subtract);
		if (subDispatch)
		{
		}
	}

	{
		const IRuntimeDispatch* mulDispatch = runtimeClass->getOperatorDispatch(IRuntimeClass::Operator::Multiply);
		if (mulDispatch)
		{
		}
	}

	{
		const IRuntimeDispatch* divDispatch = runtimeClass->getOperatorDispatch(IRuntimeClass::Operator::Divide);
		if (divDispatch)
		{
		}
	}

	// Export class in global scope.
	std::wstring exportName = exportType.getName();
	std::vector< std::wstring > exportPath;
	Split< std::wstring >::any(exportName, L".", exportPath);

	if (exportPath.size() > 1)
	{
		for (size_t i = 0; i < exportPath.size() - 1; ++i)
		{
		}
	}


	// Store index of registered script class in C++ rtti type; used
	// to accelerate lookup of C++ class when constructing new instance from script.
	// Need to propagate index into derived types as well in order to
	// be able to skip traversing class hierarchy while constructing.
	for (auto derivedType : exportType.findAllOf())
	{
		if (derivedType->getTag() != 0)
		{
			const RegisteredClass& rc2 = m_classRegistry[derivedType->getTag() - 1];
			const TypeInfo& exportType2 = rc2.runtimeClass->getExportType();
			if (is_type_of(exportType, exportType2))
				continue;
		}
		derivedType->setTag(classRegistryIndex + 1);
	}

	// Add constants last as constants might be instances of this class, i.e. singletons etc.
	for (uint32_t i = 0; i < runtimeClass->getConstantCount(); ++i)
	{
	}
}

Ref< IScriptContext > ScriptManagerAngelScript::createContext(bool strict)
{
	// Create context.
	//Ref< ScriptContextAngelScript > context = new ScriptContextAngelScript(this, strict);
	//m_contexts.push_back(context);
	//return context;
	return nullptr;
}

Ref< IScriptDebugger > ScriptManagerAngelScript::createDebugger()
{
	//if (!m_debugger)
	//	m_debugger = new ScriptDebuggerAngelScript(this);

	//return m_debugger;
	return nullptr;
}

Ref< IScriptProfiler > ScriptManagerAngelScript::createProfiler()
{
	//if (!m_profiler)
	//	m_profiler = new ScriptProfilerAngelScript(this);

	//return m_profiler;
	return nullptr;
}

void ScriptManagerAngelScript::collectGarbage(bool full)
{
}

void ScriptManagerAngelScript::getStatistics(ScriptStatistics& outStatistics) const
{
	outStatistics.memoryUsage = uint32_t(m_totalMemoryUse);
}

}
