/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Rtti/TypeInfo.h"

#if defined(T_STATIC)
#	include "Paper/Runtime/PaperRuntimeClassFactory.h"
#	include "Paper/Runtime/PaperRuntimePlugin.h"

namespace traktor::paper
{

extern "C" void __module__Traktor_Paper_Runtime()
{
	T_FORCE_LINK_REF(PaperRuntimeClassFactory);
	T_FORCE_LINK_REF(PaperRuntimePlugin);
}

}

#endif
