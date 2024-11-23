/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/Runtime/TurboBadgerUiLayerData.h"
#include "TurboBadgerUi/Runtime/TurboBadgerUiLayer.h"
#include "TurboBadgerUi/TurboBadgerUiViewResource.h"

#include "Resource/IResourceManager.h"
#include "Resource/Member.h"
#include "Runtime/IEnvironment.h"

namespace traktor::turbobadgerui
{
	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.turbobadgerui.TurboBadgerUiLayerData", 1, TurboBadgerUiLayerData, runtime::LayerData)

		TurboBadgerUiLayerData::TurboBadgerUiLayerData()
		: runtime::LayerData(L"turbobadgerui")
	{
	}

	Ref< runtime::Layer > TurboBadgerUiLayerData::createInstance(runtime::Stage* stage, runtime::IEnvironment* environment) const
	{
		resource::IResourceManager* resourceManager = environment->getResource()->getResourceManager();
		resource::Proxy< TurboBadgerUiViewResource > uiViewResource;

		// Bind proxies to resource manager.
		if (!resourceManager->bind(m_uiViewResource, uiViewResource))
			return nullptr;

		// Create layer instance.
		return new TurboBadgerUiLayer(
			stage,
			m_name,
			m_permitTransition,
			environment,
			uiViewResource
		);
	}

	void TurboBadgerUiLayerData::serialize(ISerializer& s)
	{
		LayerData::serialize(s);

		s >> resource::Member< TurboBadgerUiViewResource >(L"resource", m_uiViewResource);
	}
}