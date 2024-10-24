/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Runtime/IEnvironment.h"
#include "Runtime/UpdateInfo.h"
#include "Core/Log/Log.h"
#include "Database/Database.h"
#include "RmlUi/Runtime/RuntimePlugin.h"
#include "RmlUi/Backend/RmlUi.h"

namespace traktor::rmlui
{

	using namespace traktor::runtime;

	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.rmlui.RuntimePlugin", 0, RuntimePlugin, IRuntimePlugin)

		bool RuntimePlugin::create(IEnvironment* environment)
	{
		return RmlUi::getInstance().Initialize(environment->getRender()->getRenderSystem(), environment->getRender()->getRenderView());
	}

	void RuntimePlugin::destroy(IEnvironment* environment)
	{
		RmlUi::getInstance().Shutdown();
	}

	Ref< IState > RuntimePlugin::createInitialState(IEnvironment* environment)
	{
		return nullptr;
	}

}
