/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/TurboBadgerUiResourceFactory.h"
#include "TurboBadgerUi/TurboBadgerUiResource.h"

#include "Database/Instance.h"

namespace traktor::turbobadgerui
{
	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.turbobadgerui.TurboBadgerUiResourceFactory", 0, TurboBadgerUiResourceFactory, resource::IResourceFactory)

		bool TurboBadgerUiResourceFactory::initialize(const ObjectStore& objectStore)
	{
		return true;
	}

	const TypeInfoSet TurboBadgerUiResourceFactory::getResourceTypes() const
	{
		return makeTypeInfoSet< TurboBadgerUiResource >();
	}

	const TypeInfoSet TurboBadgerUiResourceFactory::getProductTypes(const TypeInfo& resourceType) const
	{
		return makeTypeInfoSet< TurboBadgerUiResource >();
	}

	bool TurboBadgerUiResourceFactory::isCacheable(const TypeInfo& productType) const
	{
		return true;
	}

	Ref< Object > TurboBadgerUiResourceFactory::create(resource::IResourceManager* resourceManager, const db::Database* database, const db::Instance* instance, const TypeInfo& productType, const Object* current) const
	{
		return instance->getObject< TurboBadgerUiResource >();
	}
}