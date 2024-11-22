/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/Editor/TurboBadgerUiPipeline.h"

#include "TurboBadgerUi/TurboBadgerUiResource.h"
#include "TurboBadgerUi/Editor/TurboBadgerUiAsset.h"

#include <cstring>
#include <list>
#include "Core/Io/FileSystem.h"
#include "Core/Io/IStream.h"
#include "Core/Log/Log.h"
#include "Core/Misc/SafeDestroy.h"
#include "Core/Misc/String.h"
#include "Core/Serialization/DeepHash.h"
#include "Core/Settings/PropertyBoolean.h"
#include "Core/Settings/PropertyInteger.h"
#include "Core/Settings/PropertyString.h"
#include "Database/Instance.h"
#include "Editor/IPipelineBuilder.h"
#include "Editor/IPipelineDepends.h"
#include "Editor/IPipelineSettings.h"
#include "Render/Shader.h"
#include "Render/Editor/Texture/TextureOutput.h"
#include "Resource/Id.h"

namespace traktor::turbobadgerui
{
	namespace
	{

		const Guid c_idTurboBadgerUiShaderAssets(L"{7F092F0F-C5FB-834F-B9EE-C08E03A844B4}");

	}

	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.turbobadgerui.TurboBadgerUiPipeline", 0, TurboBadgerUiPipeline, editor::IPipeline)

		TurboBadgerUiPipeline::TurboBadgerUiPipeline()
	{
	}

	bool TurboBadgerUiPipeline::create(const editor::IPipelineSettings* settings, db::Database* database)
	{
		m_assetPath = settings->getPropertyExcludeHash< std::wstring >(L"Pipeline.AssetPath", L"");
		return true;
	}

	void TurboBadgerUiPipeline::destroy()
	{
	}

	TypeInfoSet TurboBadgerUiPipeline::getAssetTypes() const
	{
		return makeTypeInfoSet<
			TurboBadgerUiAsset
		>();
	}

	bool TurboBadgerUiPipeline::shouldCache() const
	{
		return true;
	}

	uint32_t TurboBadgerUiPipeline::hashAsset(const ISerializable* sourceAsset) const
	{
		return DeepHash(sourceAsset).get();
	}

	bool TurboBadgerUiPipeline::buildDependencies(
		editor::IPipelineDepends* pipelineDepends,
		const db::Instance* sourceInstance,
		const ISerializable* sourceAsset,
		const std::wstring& outputPath,
		const Guid& outputGuid
	) const
	{
		if (const TurboBadgerUiAsset* uiAsset = dynamic_type_cast<const TurboBadgerUiAsset*>(sourceAsset))
		{
			pipelineDepends->addDependency(traktor::Path(m_assetPath), uiAsset->getFileName().getOriginal());
		}
		pipelineDepends->addDependency(c_idTurboBadgerUiShaderAssets, editor::PdfBuild | editor::PdfResource);
		//pipelineDepends->addDependency< render::TextureOutput >();
		return true;
	}

	bool TurboBadgerUiPipeline::buildOutput(
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
		Ref< TurboBadgerUiResource > uiResource;
		bool optimize = false;

		if (const TurboBadgerUiAsset* uiAsset = dynamic_type_cast<const TurboBadgerUiAsset*>(sourceAsset))
		{
			const traktor::Path filePath = FileSystem::getInstance().getAbsolutePath(traktor::Path(m_assetPath) + uiAsset->getFileName());
			Ref< IStream > sourceStream = FileSystem::getInstance().open(filePath, File::FmRead);
			if (!sourceStream)
			{
				log::error << L"Failed to import TurboBadgerUi asset; unable to open file \"" << uiAsset->getFileName().getOriginal() << L"\"." << Endl;
				return false;
			}

			uiResource = new TurboBadgerUiResource(
				uiAsset->getFileName()
			);

			safeClose(sourceStream);
		}

		// Show some information about the ui asset.
		log::info << L"TurboBadgerUi asset successfully loaded," << Endl;
		log::info << IncreaseIndent;
		log::info << DecreaseIndent;

		Ref< db::Instance > instance = pipelineBuilder->createOutputInstance(
			outputPath,
			outputGuid
		);
		if (!instance)
		{
			log::error << L"Failed to import TurboBadgerUi asset; unable to create instance." << Endl;
			return false;
		}

		instance->setObject(uiResource);

		if (!instance->commit())
		{
			log::info << L"Failed to import TurboBadgerUi asset; unable to commit instance." << Endl;
			return false;
		}

		return true;
	}

	Ref< ISerializable > TurboBadgerUiPipeline::buildProduct(
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