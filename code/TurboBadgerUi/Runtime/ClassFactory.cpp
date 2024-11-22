/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/Runtime/ClassFactory.h"

#include "TurboBadgerUi/Runtime/TurboBadgerUiLayer.h"

#include "Core/Class/AutoRuntimeClass.h"
#include "Core/Class/IRuntimeClassRegistrar.h"

namespace traktor::turbobadgerui
{
	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.turbobadgerui.TurboBadgerUiClassFactory", 0, ClassFactory, IRuntimeClassFactory)

		void ClassFactory::createClasses(IRuntimeClassRegistrar* registrar) const
	{
		auto classTurboBadgerUiLayer = new AutoRuntimeClass< TurboBadgerUiLayer >();
		registrar->registerClass(classTurboBadgerUiLayer);
	}
}