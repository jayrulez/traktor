/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Class/IRuntimeClassFactory.h"

 // import/export mechanism.
#undef T_DLLCLASS
#if defined(T_RMLUI_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::rmlui
{
	/*! RmlUi runtime class factory.
		* \ingroup RmUi
		*/
	class T_DLLCLASS ClassFactory : public IRuntimeClassFactory
	{
		T_RTTI_CLASS;

	public:
		virtual void createClasses(IRuntimeClassRegistrar* registrar) const override final;
	};
}