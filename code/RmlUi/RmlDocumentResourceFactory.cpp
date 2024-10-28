/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Database/Instance.h"
#include "RmlUi/RmlDocumentResource.h"
#include "RmlUi/RmlDocumentResourceFactory.h"

namespace traktor::rmlui
{

	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.rmlui.RmlDocumentResourceFactory", 0, RmlDocumentResourceFactory, resource::IResourceFactory)

		bool RmlDocumentResourceFactory::initialize(const ObjectStore& objectStore)
	{
		return true;
	}

	const TypeInfoSet RmlDocumentResourceFactory::getResourceTypes() const
	{
		return makeTypeInfoSet< RmlDocumentResource >();
	}

	const TypeInfoSet RmlDocumentResourceFactory::getProductTypes(const TypeInfo& resourceType) const
	{
		return makeTypeInfoSet< RmlDocumentResource >();
	}

	bool RmlDocumentResourceFactory::isCacheable(const TypeInfo& productType) const
	{
		return true;
	}

	Ref< Object > RmlDocumentResourceFactory::create(resource::IResourceManager* resourceManager, const db::Database* database, const db::Instance* instance, const TypeInfo& productType, const Object* current) const
	{
		return instance->getObject< RmlDocumentResource >();
	}

}