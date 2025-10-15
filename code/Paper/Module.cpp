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
#	include "Paper/ClassFactory.h"
#	include "Paper/UIPage.h"
#	include "Paper/UITheme.h"
#	include "Paper/UIStyle.h"

namespace traktor::paper
{

extern "C" void __module__Traktor_Paper()
{
	T_FORCE_LINK_REF(ClassFactory);
	T_FORCE_LINK_REF(UIPage);
	T_FORCE_LINK_REF(UITheme);
	T_FORCE_LINK_REF(UIStyle);
}

}

#endif
