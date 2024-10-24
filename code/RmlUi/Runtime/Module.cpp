/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Rtti/TypeInfo.h"

#if defined(T_STATIC)
#	include "RmlUi/Runtime/RmlUiClassFactory.h"
#	include "RmlUi/Runtime/RmlUiLayerData.h"
#	include "RmlUi/Runtime/RuntimePlugin.h"

namespace traktor::rmlui
{

	extern "C" void __module__Traktor_RmlUi_Runtime()
	{
		T_FORCE_LINK_REF(RmlUiClassFactory);
		T_FORCE_LINK_REF(RmlUiLayerData);
		T_FORCE_LINK_REF(RuntimePlugin);
	}

}

#endif