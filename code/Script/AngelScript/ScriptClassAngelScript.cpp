/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Script/AngelScript/ScriptClassAngelScript.h"

#include "angelscript.h"
#include "Core/Class/IRuntimeDispatch.h"
#include "Core/Misc/String.h"
#include "Core/Misc/TString.h"
#include "Script/AngelScript/ScriptManagerAngelScript.h"
#include "Script/AngelScript/ScriptObjectAngelScript.h"

namespace traktor::script
{
namespace
{

class ScriptClassConstructorDispatch : public IRuntimeDispatch
{
public:
	explicit ScriptClassConstructorDispatch(ScriptManagerAngelScript* scriptManager, asITypeInfo* typeInfo)
		: m_scriptManager(scriptManager)
		, m_typeInfo(typeInfo)
	{
		if (m_typeInfo)
			m_typeInfo->AddRef();
	}

	virtual ~ScriptClassConstructorDispatch()
	{
		if (m_typeInfo)
			m_typeInfo->Release();
	}

#if defined(T_NEED_RUNTIME_SIGNATURE)
	virtual void signature(OutputStream& os) const override final
	{
		os << L"void constructor()";
	}
#endif

	virtual Any invoke(ITypedObject* self, uint32_t argc, const Any* argv) const override final
	{
		// Create AngelScript object instance
		void* obj = m_scriptManager->getEngine()->CreateScriptObject(m_typeInfo);
		if (obj)
		{
			// Wrap in Traktor script object
			return Any::fromObject(new ScriptObjectAngelScript(m_scriptManager, static_cast< asIScriptObject* >(obj)));
		}
		return Any();
	}

private:
	ScriptManagerAngelScript* m_scriptManager;
	asITypeInfo* m_typeInfo;
};

class ScriptClassMethodDispatch : public IRuntimeDispatch
{
public:
	explicit ScriptClassMethodDispatch(ScriptManagerAngelScript* scriptManager, asIScriptFunction* function)
		: m_scriptManager(scriptManager)
		, m_function(function)
	{
		if (m_function)
			m_function->AddRef();
	}

	virtual ~ScriptClassMethodDispatch()
	{
		if (m_function)
			m_function->Release();
	}

#if defined(T_NEED_RUNTIME_SIGNATURE)
	virtual void signature(OutputStream& os) const override final
	{
		if (m_function)
		{
			// Get AngelScript function signature and convert to wide string
			const char* decl = m_function->GetDeclaration();
			os << mbstows(decl ? decl : "void method()");
		}
		else
		{
			os << L"void method()";
		}
	}
#endif

	virtual Any invoke(ITypedObject* self, uint32_t argc, const Any* argv) const override final
	{
		if (!m_function)
			return Any();

		// Execute AngelScript method
		asIScriptContext* ctx = m_scriptManager->getEngine()->CreateContext();
		if (!ctx)
			return Any();

		int result = ctx->Prepare(m_function);
		if (result < 0)
		{
			ctx->Release();
			return Any();
		}

		// Set object instance
		ScriptObjectAngelScript* scriptObj = dynamic_type_cast< ScriptObjectAngelScript* >(self);
		if (scriptObj)
			ctx->SetObject(scriptObj->getScriptObject());

		// Set arguments
		for (uint32_t i = 0; i < argc; ++i)
			m_scriptManager->setArgumentFromAny(ctx, i, argv[i]);

		// Execute
		result = ctx->Execute();
		Any returnValue;

		if (result == asEXECUTION_FINISHED)
		{
			int returnTypeId = m_function->GetReturnTypeId();
			if (returnTypeId != asTYPEID_VOID)
				returnValue = m_scriptManager->getReturnValueAsAny(ctx, returnTypeId);
		}

		ctx->Release();
		return returnValue;
	}

private:
	ScriptManagerAngelScript* m_scriptManager;
	asIScriptFunction* m_function;
};

class ScriptClassPropertyDispatch : public IRuntimeDispatch
{
public:
	explicit ScriptClassPropertyDispatch(ScriptManagerAngelScript* scriptManager, asITypeInfo* typeInfo,
		const std::string& propertyName, bool isGetter)
		: m_scriptManager(scriptManager)
		, m_typeInfo(typeInfo)
		, m_propertyName(propertyName)
		, m_isGetter(isGetter)
	{
		if (m_typeInfo)
			m_typeInfo->AddRef();
	}

	virtual ~ScriptClassPropertyDispatch()
	{
		if (m_typeInfo)
			m_typeInfo->Release();
	}

#if defined(T_NEED_RUNTIME_SIGNATURE)
	virtual void signature(OutputStream& os) const override final
	{
		if (m_isGetter)
			os << L"PropertyType get_" << mbstows(m_propertyName) << L"() const";
		else
			os << L"void set_" << mbstows(m_propertyName) << L"(PropertyType value)";
	}
#endif

	virtual Any invoke(ITypedObject* self, uint32_t argc, const Any* argv) const override final
	{
		// Property access would need to be implemented based on how AngelScript
		// handles property access - this is a simplified placeholder
		return Any();
	}

private:
	ScriptManagerAngelScript* m_scriptManager;
	asITypeInfo* m_typeInfo;
	std::string m_propertyName;
	bool m_isGetter;
};

}

T_IMPLEMENT_RTTI_CLASS(L"traktor.script.ScriptClassAngelScript", ScriptClassAngelScript, IRuntimeClass)

Ref< ScriptClassAngelScript > ScriptClassAngelScript::createFromTypeInfo(ScriptManagerAngelScript* scriptManager, asITypeInfo* typeInfo)
{
	if (!typeInfo)
		return nullptr;

	Ref< ScriptClassAngelScript > sc = new ScriptClassAngelScript(scriptManager, typeInfo);

	// Add constructor
	asUINT factoryCount = typeInfo->GetFactoryCount();
	if (factoryCount > 0)
		sc->m_constructor = new ScriptClassConstructorDispatch(scriptManager, typeInfo);

	// Add methods
	asUINT methodCount = typeInfo->GetMethodCount();
	for (asUINT i = 0; i < methodCount; ++i)
	{
		asIScriptFunction* func = typeInfo->GetMethodByIndex(i);
		if (func)
		{
			Method& m = sc->m_methods.push_back();
			m.name = func->GetName();
			m.dispatch = new ScriptClassMethodDispatch(scriptManager, func);
		}
	}

	// Add properties
	asUINT propertyCount = typeInfo->GetPropertyCount();
	for (asUINT i = 0; i < propertyCount; ++i)
	{
		const char* name;
		int typeId;
		if (typeInfo->GetProperty(i, &name, &typeId) >= 0 && name)
		{
			Property& p = sc->m_properties.push_back();
			p.name = name;
			// Create getter and setter dispatches
			p.getDispatch = new ScriptClassPropertyDispatch(scriptManager, typeInfo, name, true);
			p.setDispatch = new ScriptClassPropertyDispatch(scriptManager, typeInfo, name, false);
		}
	}

	return sc;
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
	return uint32_t(m_staticMethods.size());
}

std::string ScriptClassAngelScript::getStaticMethodName(uint32_t methodId) const
{
	if (methodId < m_staticMethods.size())
		return m_staticMethods[methodId].name;
	return "";
}

const IRuntimeDispatch* ScriptClassAngelScript::getStaticMethodDispatch(uint32_t methodId) const
{
	if (methodId < m_staticMethods.size())
		return m_staticMethods[methodId].dispatch;
	return nullptr;
}

uint32_t ScriptClassAngelScript::getPropertiesCount() const
{
	return uint32_t(m_properties.size());
}

std::string ScriptClassAngelScript::getPropertyName(uint32_t propertyId) const
{
	if (propertyId < m_properties.size())
		return m_properties[propertyId].name;
	return "";
}

const IRuntimeDispatch* ScriptClassAngelScript::getPropertyGetDispatch(uint32_t propertyId) const
{
	if (propertyId < m_properties.size())
		return m_properties[propertyId].getDispatch;
	return nullptr;
}

const IRuntimeDispatch* ScriptClassAngelScript::getPropertySetDispatch(uint32_t propertyId) const
{
	if (propertyId < m_properties.size())
		return m_properties[propertyId].setDispatch;
	return nullptr;
}

const IRuntimeDispatch* ScriptClassAngelScript::getOperatorDispatch(Operator op) const
{
	return nullptr;
}

ScriptClassAngelScript::ScriptClassAngelScript(ScriptManagerAngelScript* scriptManager, asITypeInfo* typeInfo)
	: m_scriptManager(scriptManager)
	, m_typeInfo(typeInfo)
{
	if (m_typeInfo)
		m_typeInfo->AddRef();
}

}