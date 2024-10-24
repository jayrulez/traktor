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
#include "RmlUi/RmlDocument.h"
#include "RmlUi/RmlDocumentFactory.h"

namespace traktor::rmlui
{
	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.rmlui.ClassFactory", 0, ClassFactory, IRuntimeClassFactory)

		void ClassFactory::createClasses(IRuntimeClassRegistrar* registrar) const
	{
		// RmlDocument
		auto classRmlDocument = new AutoRuntimeClass< RmlDocument >();
		registrar->registerClass(classRmlDocument);

		// RmlDocumentFactory
		auto classRmlDocumentFactory = new AutoRuntimeClass< RmlDocumentFactory >();
		classRmlDocumentFactory->addConstructor();
		classRmlDocumentFactory->addMethod("createRmlDocument", &RmlDocumentFactory::createRmlDocument);
		registrar->registerClass(classRmlDocumentFactory);
	}

}