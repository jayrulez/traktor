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
#include "Core/Class/IRuntimeClass.h"
#include "Core/Containers/AlignedVector.h"

namespace traktor::script
{

class ScriptManagerAngelScript;

/*! Runtime representation of a script-side AngelScript class.
 * \ingroup Script
 */
class ScriptClassAngelScript : public IRuntimeClass
{
	T_RTTI_CLASS;

public:
	static Ref< ScriptClassAngelScript > createFromTypeInfo(ScriptManagerAngelScript* scriptManager, asITypeInfo* typeInfo);

	virtual const TypeInfo& getExportType() const override final;

	virtual const IRuntimeDispatch* getConstructorDispatch() const override final;

	virtual uint32_t getConstantCount() const override final;

	virtual std::string getConstantName(uint32_t constId) const override final;

	virtual Any getConstantValue(uint32_t constId) const override final;

	virtual uint32_t getMethodCount() const override final;

	virtual std::string getMethodName(uint32_t methodId) const override final;

	virtual const IRuntimeDispatch* getMethodDispatch(uint32_t methodId) const override final;

	virtual uint32_t getStaticMethodCount() const override final;

	virtual std::string getStaticMethodName(uint32_t methodId) const override final;

	virtual const IRuntimeDispatch* getStaticMethodDispatch(uint32_t methodId) const override final;

	virtual uint32_t getPropertiesCount() const override final;

	virtual std::string getPropertyName(uint32_t propertyId) const override final;

	virtual const IRuntimeDispatch* getPropertyGetDispatch(uint32_t propertyId) const override final;

	virtual const IRuntimeDispatch* getPropertySetDispatch(uint32_t propertyId) const override final;

	virtual const IRuntimeDispatch* getOperatorDispatch(Operator op) const override final;

private:
	struct Method
	{
		std::string name;
		Ref< const IRuntimeDispatch > dispatch;
	};

	struct Property
	{
		std::string name;
		Ref< const IRuntimeDispatch > getDispatch;
		Ref< const IRuntimeDispatch > setDispatch;
	};

	ScriptManagerAngelScript* m_scriptManager;
	asITypeInfo* m_typeInfo;
	Ref< const IRuntimeDispatch > m_constructor;
	AlignedVector< Method > m_methods;
	AlignedVector< Method > m_staticMethods;
	AlignedVector< Property > m_properties;

	explicit ScriptClassAngelScript(ScriptManagerAngelScript* scriptManager, asITypeInfo* typeInfo);
};

}