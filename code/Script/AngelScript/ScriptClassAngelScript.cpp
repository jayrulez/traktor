/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <angelscript.h>
#include "Core/Misc/TString.h"
#include "Script/AngelScript/ScriptClassAngelScript.h"
#include "Script/AngelScript/ScriptManagerAngelScript.h"
#include "Script/AngelScript/ScriptContextAngelScript.h"

namespace traktor::script
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.script.ScriptClassAngelScript", ScriptClassAngelScript, IRuntimeClass)

ScriptClassAngelScript::ScriptClassAngelScript(ScriptManagerAngelScript* scriptManager, asITypeInfo* typeInfo)
:	m_scriptManager(scriptManager)
,	m_typeInfo(typeInfo)
{
	if (m_typeInfo)
		m_typeInfo->AddRef();
}

Ref< ScriptClassAngelScript > ScriptClassAngelScript::createFromTypeInfo(ScriptManagerAngelScript* scriptManager, ScriptContextAngelScript* scriptContext, asITypeInfo* typeInfo)
{
	if (!typeInfo)
		return nullptr;

	return new ScriptClassAngelScript(scriptManager, typeInfo);
}

const TypeInfo& ScriptClassAngelScript::getExportType() const
{
	return type_of< Object >();
}

const IRuntimeDispatch* ScriptClassAngelScript::getConstructorDispatch() const
{
	return m_constructor;
}

uint32_t ScriptClassAngelScript::getConstantCount() const
{
	return 0;
}

std::string ScriptClassAngelScript::getConstantName(uint32_t constId) const
{
	return "";
}

Any ScriptClassAngelScript::getConstantValue(uint32_t constId) const
{
	return Any();
}

uint32_t ScriptClassAngelScript::getMethodCount() const
{
	return uint32_t(m_methods.size());
}

std::string ScriptClassAngelScript::getMethodName(uint32_t methodId) const
{
	if (methodId < m_methods.size())
		return m_methods[methodId].name;
	return "";
}

const IRuntimeDispatch* ScriptClassAngelScript::getMethodDispatch(uint32_t methodId) const
{
	if (methodId < m_methods.size())
		return m_methods[methodId].dispatch;
	return nullptr;
}

uint32_t ScriptClassAngelScript::getStaticMethodCount() const
{
	return 0;
}

std::string ScriptClassAngelScript::getStaticMethodName(uint32_t methodId) const
{
	return "";
}

const IRuntimeDispatch* ScriptClassAngelScript::getStaticMethodDispatch(uint32_t methodId) const
{
	return nullptr;
}

uint32_t ScriptClassAngelScript::getPropertiesCount() const
{
	return 0;
}

std::string ScriptClassAngelScript::getPropertyName(uint32_t propertyId) const
{
	return "";
}

const IRuntimeDispatch* ScriptClassAngelScript::getPropertyGetDispatch(uint32_t propertyId) const
{
	return nullptr;
}

const IRuntimeDispatch* ScriptClassAngelScript::getPropertySetDispatch(uint32_t propertyId) const
{
	return nullptr;
}

const IRuntimeDispatch* ScriptClassAngelScript::getOperatorDispatch(Operator op) const
{
	return nullptr;
}

}
