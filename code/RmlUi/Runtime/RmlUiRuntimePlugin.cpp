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
#include "RmlUi/Runtime/RmlUiRuntimePlugin.h"
#include "RmlUi/RmlUi.h"

namespace traktor::rmlui
{

	using namespace traktor::runtime;

	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.rmlui.RmlUiRuntimePlugin", 0, RmlUiRuntimePlugin, IRuntimePlugin)

		bool RmlUiRuntimePlugin::create(IEnvironment* environment)
	{
		return RmlUi::getInstance().initialize(environment->getRender()->getRenderSystem());
	}

	void RmlUiRuntimePlugin::destroy(IEnvironment* environment)
	{
		if (RmlUi::getInstance().isInitialized())
			RmlUi::getInstance().shutdown();
	}

	Ref< IState > RmlUiRuntimePlugin::createInitialState(IEnvironment* environment)
	{
		return nullptr;
	}

}
