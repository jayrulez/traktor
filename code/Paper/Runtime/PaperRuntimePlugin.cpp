/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Paper/Runtime/PaperRuntimePlugin.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.PaperRuntimePlugin", 0, PaperRuntimePlugin, runtime::IRuntimePlugin)

bool PaperRuntimePlugin::create(runtime::IEnvironment* environment)
{
	return true;
}

void PaperRuntimePlugin::destroy(runtime::IEnvironment* environment)
{
}

Ref< runtime::IState > PaperRuntimePlugin::createInitialState(runtime::IEnvironment* environment)
{
	return nullptr;
}

}
