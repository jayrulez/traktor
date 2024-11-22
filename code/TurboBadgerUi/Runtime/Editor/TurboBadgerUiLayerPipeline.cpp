/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/Runtime/Editor/TurboBadgerUiLayerPipeline.h"

#include "TurboBadgerUi/Runtime/TurboBadgerUiLayerData.h"

#include "Editor/IPipelineDepends.h"

namespace traktor::turbobadgerui
{
	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.turbobadgerui.TurboBadgerUiLayerPipeline", 0, TurboBadgerUiLayerPipeline, editor::DefaultPipeline)

		TypeInfoSet TurboBadgerUiLayerPipeline::getAssetTypes() const
	{
		return makeTypeInfoSet< TurboBadgerUiLayerData >();
	}

	bool TurboBadgerUiLayerPipeline::buildDependencies(
		editor::IPipelineDepends* pipelineDepends,
		const db::Instance* sourceInstance,
		const ISerializable* sourceAsset,
		const std::wstring& outputPath,
		const Guid& outputGuid
	) const
	{
		const TurboBadgerUiLayerData* layerData = mandatory_non_null_type_cast<const TurboBadgerUiLayerData*>(sourceAsset);

		pipelineDepends->addDependency(layerData->m_uiResource, editor::PdfBuild | editor::PdfResource);

		return true;
	}
}