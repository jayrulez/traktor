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
#include "Paper/Runtime/PaperRuntimeClassFactory.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.PaperRuntimeClassFactory", 0, PaperRuntimeClassFactory, IRuntimeClassFactory)

void PaperRuntimeClassFactory::createClasses(IRuntimeClassRegistrar* registrar) const
{
}

}
