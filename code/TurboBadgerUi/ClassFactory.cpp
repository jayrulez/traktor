/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/ClassFactory.h"

#include "TurboBadgerUi/TurboBadgerUiResource.h"
#include "TurboBadgerUi/TurboBadgerUi.h"

#include "Core/Class/AutoRuntimeClass.h"
#include "Core/Class/IRuntimeClassRegistrar.h"

namespace traktor::turbobadgerui
{
	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.turbobadgerui.ClassFactory", 0, ClassFactory, IRuntimeClassFactory)

		void ClassFactory::createClasses(IRuntimeClassRegistrar* registrar) const
	{
		// TurboBadgerUiResource
		auto classTurboBadgerUiResource = new AutoRuntimeClass< TurboBadgerUiResource >();
		registrar->registerClass(classTurboBadgerUiResource);

		// TurboBadgerUi
		auto classTurboBadgerUi = new AutoRuntimeClass< TurboBadgerUi >();
		registrar->registerClass(classTurboBadgerUi);
	}
}