/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Editor/IPipelineDepends.h"
#include "RmlUi/Runtime/RmlUiLayerData.h"
#include "RmlUi/Runtime/Editor/RmlUiLayerPipeline.h"

namespace traktor::rmlui
{

	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.rmlui.RmlUiLayerPipeline", 0, RmlUiLayerPipeline, editor::DefaultPipeline)

		TypeInfoSet RmlUiLayerPipeline::getAssetTypes() const
	{
		return makeTypeInfoSet< RmlUiLayerData >();
	}

	bool RmlUiLayerPipeline::buildDependencies(
		editor::IPipelineDepends* pipelineDepends,
		const db::Instance* sourceInstance,
		const ISerializable* sourceAsset,
		const std::wstring& outputPath,
		const Guid& outputGuid
	) const
	{
		const RmlUiLayerData* layerData = mandatory_non_null_type_cast<const RmlUiLayerData*>(sourceAsset);

		pipelineDepends->addDependency(layerData->m_rmlDocument, editor::PdfBuild | editor::PdfResource);

		return true;
	}

}