/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Misc/ObjectStore.h"
#include "Database/Instance.h"
#include "Paper/BitmapFont/BitmapFont.h"
#include "Paper/BitmapFont/BitmapFontFactory.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.BitmapFontFactory", 0, BitmapFontFactory, resource::IResourceFactory)

bool BitmapFontFactory::initialize(const ObjectStore& objectStore)
{
	return true;
}

const TypeInfoSet BitmapFontFactory::getResourceTypes() const
{
	return makeTypeInfoSet< BitmapFont >();
}

const TypeInfoSet BitmapFontFactory::getProductTypes(const TypeInfo& resourceType) const
{
	return makeTypeInfoSet< BitmapFont >();
}

bool BitmapFontFactory::isCacheable(const TypeInfo& productType) const
{
	return true;
}

Ref< Object > BitmapFontFactory::create(resource::IResourceManager* resourceManager, const db::Database* database, const db::Instance* instance, const TypeInfo& productType, const Object* current) const
{
	// BitmapFont is a simple serialized resource, just return it directly
	return instance->getObject< BitmapFont >();
}

}
