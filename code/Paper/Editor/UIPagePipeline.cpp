/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Paper/Editor/UIPagePipeline.h"

#include "Core/Log/Log.h"
#include "Core/Serialization/DeepHash.h"
#include "Core/Settings/PropertyString.h"
#include "Database/Instance.h"
#include "Editor/IPipelineBuilder.h"
#include "Editor/IPipelineDepends.h"
#include "Editor/IPipelineSettings.h"
#include "Paper/Editor/UIPageAsset.h"
#include "Paper/Ui/UIPage.h"

namespace traktor::paper
{
namespace
{
const Guid c_idPaper2DShader(L"{A704DB1C-60E6-9D44-AD1D-F7822568242D}"); // System/Paper/Shaders/Paper2D
}

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.UIPagePipeline", 0, UIPagePipeline, editor::IPipeline)

bool UIPagePipeline::create(const editor::IPipelineSettings* settings, db::Database* database)
{
	m_assetPath = settings->getPropertyExcludeHash< std::wstring >(L"Pipeline.AssetPath", L"");
	return true;
}

void UIPagePipeline::destroy()
{
}

TypeInfoSet UIPagePipeline::getAssetTypes() const
{
	return makeTypeInfoSet< UIPageAsset >();
}

bool UIPagePipeline::shouldCache() const
{
	return true;
}

uint32_t UIPagePipeline::hashAsset(const ISerializable* sourceAsset) const
{
	return DeepHash(sourceAsset).get();
}

bool UIPagePipeline::buildDependencies(
	editor::IPipelineDepends* pipelineDepends,
	const db::Instance* sourceInstance,
	const ISerializable* sourceAsset,
	const std::wstring& outputPath,
	const Guid& outputGuid) const
{
	// Add dependency on Paper2D shader
	pipelineDepends->addDependency(c_idPaper2DShader, editor::PdfBuild | editor::PdfResource);
	return true;
}

bool UIPagePipeline::buildOutput(
	editor::IPipelineBuilder* pipelineBuilder,
	const editor::PipelineDependencySet* dependencySet,
	const editor::PipelineDependency* dependency,
	const db::Instance* sourceInstance,
	const ISerializable* sourceAsset,
	const std::wstring& outputPath,
	const Guid& outputGuid,
	const Object* buildParams,
	uint32_t reason) const
{
	const UIPageAsset* uiPageAsset = mandatory_non_null_type_cast< const UIPageAsset* >(sourceAsset);

	// Create an empty UIPage for now
	// Later we'll manually construct UI in code during development
	Ref< UIPage > uiPage = new UIPage();
	uiPage->setWidth(uiPageAsset->getWidth());
	uiPage->setHeight(uiPageAsset->getHeight());

	// Save UIPage
	Ref< db::Instance > instance = pipelineBuilder->createOutputInstance(outputPath, outputGuid);
	if (!instance)
	{
		log::error << L"Failed to create output instance." << Endl;
		return false;
	}

	instance->setObject(uiPage);

	if (!instance->commit())
	{
		log::error << L"Failed to commit instance." << Endl;
		return false;
	}

	return true;
}

Ref< ISerializable > UIPagePipeline::buildProduct(
	editor::IPipelineBuilder* pipelineBuilder,
	const db::Instance* sourceInstance,
	const ISerializable* sourceAsset,
	const Object* buildParams) const
{
	T_FATAL_ERROR;
	return nullptr;
}

}
