/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Config.h"
#include "Runtime/IRuntimePlugin.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_RMLUI_RUNTIME_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::rmlui
{

/*! RmlUi runtime plugin.
 * \ingroup RmlUi
 */
class T_DLLCLASS RuntimePlugin : public runtime::IRuntimePlugin
{
	T_RTTI_CLASS;

public:
	virtual bool create(runtime::IEnvironment* environment) override final;

	virtual void destroy(runtime::IEnvironment* environment) override final;

	virtual Ref< runtime::IState > createInitialState(runtime::IEnvironment* environment) override final;
};

}
