/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/TurboBadgerUiViewResourceFactory.h"
#include "TurboBadgerUi/TurboBadgerUiViewResource.h"

#include "Database/Instance.h"

namespace traktor::turbobadgerui
{
	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.turbobadgerui.TurboBadgerUiViewResourceFactory", 0, TurboBadgerUiViewResourceFactory, resource::IResourceFactory)

		bool TurboBadgerUiViewResourceFactory::initialize(const ObjectStore& objectStore)
	{
		return true;
	}

	const TypeInfoSet TurboBadgerUiViewResourceFactory::getResourceTypes() const
	{
		return makeTypeInfoSet< TurboBadgerUiViewResource >();
	}

	const TypeInfoSet TurboBadgerUiViewResourceFactory::getProductTypes(const TypeInfo& resourceType) const
	{
		return makeTypeInfoSet< TurboBadgerUiViewResource >();
	}

	bool TurboBadgerUiViewResourceFactory::isCacheable(const TypeInfo& productType) const
	{
		return true;
	}

	Ref< Object > TurboBadgerUiViewResourceFactory::create(resource::IResourceManager* resourceManager, const db::Database* database, const db::Instance* instance, const TypeInfo& productType, const Object* current) const
	{
		return instance->getObject< TurboBadgerUiViewResource >();
	}
}