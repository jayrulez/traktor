/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Serialization/DeepHash.h"
#include "Core/Settings/PropertyString.h"
#include "Database/Instance.h"
#include "Editor/IPipelineBuilder.h"
#include "Editor/IPipelineDepends.h"
#include "Editor/IPipelineSettings.h"
#include "Paper/UIPage.h"
#include "Paper/Editor/PaperPipeline.h"
#include "Paper/Editor/UIPageAsset.h"

namespace traktor::paper
{
namespace
{
const Guid c_idPaper2DShader(L"{A704DB1C-60E6-9D44-AD1D-F7822568242D}"); // System/Paper/Shaders/Paper2D
}

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.PaperPipeline", 0, PaperPipeline, editor::IPipeline)

bool PaperPipeline::create(const editor::IPipelineSettings* settings, db::Database* database)
{
	m_assetPath = settings->getPropertyExcludeHash< std::wstring >(L"Pipeline.AssetPath", L"");
	return true;
}

void PaperPipeline::destroy()
{
}

TypeInfoSet PaperPipeline::getAssetTypes() const
{
	return makeTypeInfoSet< UIPageAsset >();
}

bool PaperPipeline::shouldCache() const
{
	return true;
}

uint32_t PaperPipeline::hashAsset(const ISerializable* sourceAsset) const
{
	return DeepHash(sourceAsset).get();
}

bool PaperPipeline::buildDependencies(
	editor::IPipelineDepends* pipelineDepends,
	const db::Instance* sourceInstance,
	const ISerializable* sourceAsset,
	const std::wstring& outputPath,
	const Guid& outputGuid
) const
{
	pipelineDepends->addDependency(c_idPaper2DShader, editor::PdfBuild | editor::PdfResource);
	return true;
}

bool PaperPipeline::buildOutput(
	editor::IPipelineBuilder* pipelineBuilder,
	const editor::PipelineDependencySet* dependencySet,
	const editor::PipelineDependency* dependency,
	const db::Instance* sourceInstance,
	const ISerializable* sourceAsset,
	const std::wstring& outputPath,
	const Guid& outputGuid,
	const Object* buildParams,
	uint32_t reason
) const
{
	Ref< UIPage > uiPage = new UIPage();

	Ref< db::Instance > instance = pipelineBuilder->createOutputInstance(outputPath, outputGuid);
	if (!instance)
		return false;

	instance->setObject(uiPage);

	if (!instance->commit())
		return false;

	return true;
}

Ref< ISerializable > PaperPipeline::buildProduct(
	editor::IPipelineBuilder* pipelineBuilder,
	const db::Instance* sourceInstance,
	const ISerializable* sourceAsset,
	const Object* buildParams
) const
{
	T_FATAL_ERROR;
	return nullptr;
}

}
