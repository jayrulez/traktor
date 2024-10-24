/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Serialization/ISerializer.h"
#include "RmlUi/RmlDocument.h"
#include "RmlUi/Runtime/RmlUiLayer.h"
#include "RmlUi/Runtime/RmlUiLayerData.h"
#include "Resource/IResourceManager.h"
#include "Resource/Member.h"
#include "Runtime/IEnvironment.h"

namespace traktor::rmlui
{

	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.rmlui.RmlUiLayerData", 0, RmlUiLayerData, runtime::LayerData)

		RmlUiLayerData::RmlUiLayerData()
		: runtime::LayerData(L"rmlui")
	{
	}

	Ref< runtime::Layer > RmlUiLayerData::createInstance(runtime::Stage* stage, runtime::IEnvironment* environment) const
	{
		resource::IResourceManager* resourceManager = environment->getResource()->getResourceManager();
		resource::Proxy< RmlDocument > rmlDocument;

		// Bind proxies to resource manager.
		if (!resourceManager->bind(m_rmlDocument, rmlDocument))
			return nullptr;

		// Create layer instance.
		return new RmlUiLayer(
			stage,
			m_name,
			m_permitTransition,
			environment,
			rmlDocument,
			m_clearBackground
		);
	}

	void RmlUiLayerData::serialize(ISerializer& s)
	{
		LayerData::serialize(s);

		s >> resource::Member< RmlDocument >(L"document", m_rmlDocument);


		s >> Member< bool >(L"clearBackground", m_clearBackground);
	}

}