/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Class/AutoRuntimeClass.h"
#include "Core/Class/IRuntimeClassRegistrar.h"
#include "Core/Class/IRuntimeDelegate.h"
#include "Core/Io/IStream.h"
#include "RmlUi/ClassFactory.h"
#include "RmlUi/RmlDocumentResource.h"
#include "RmlUi/RmlUi.h"

namespace traktor::rmlui
{
	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.rmlui.ClassFactory", 0, ClassFactory, IRuntimeClassFactory)

		void ClassFactory::createClasses(IRuntimeClassRegistrar* registrar) const
	{
		// RmlDocumentResource
		auto classRmlDocument = new AutoRuntimeClass< RmlDocumentResource >();
		registrar->registerClass(classRmlDocument);

		// RmlUi
		auto classRmlUi = new AutoRuntimeClass< RmlUi >();
		registrar->registerClass(classRmlUi);
	}

}