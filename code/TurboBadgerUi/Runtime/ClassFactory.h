/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "TurboBadgerUi/Runtime/Types.h"

#include "Core/Class/IRuntimeClassFactory.h"

 // import/export mechanism.
#undef T_DLLCLASS
#if defined(T_TURBOBADGERUI_RUNTIME_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::turbobadgerui
{
	/*!
 * \ingroup TurboBadgerUi
 */
	class T_DLLCLASS ClassFactory : public IRuntimeClassFactory
	{
		T_RTTI_CLASS;

	public:
		virtual void createClasses(IRuntimeClassRegistrar* registrar) const override final;
	};
}