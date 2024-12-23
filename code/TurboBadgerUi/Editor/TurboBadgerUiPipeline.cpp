/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/Editor/TurboBadgerUiPipeline.h"

#include "TurboBadgerUi/TurboBadgerUiViewResource.h"
#include "TurboBadgerUi/Editor/TurboBadgerUiViewAsset.h"

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
		const Guid c_idTurboBadgerUiShaderAssets(L"{54D126B8-1E0A-024D-94D7-7D30C5E99F2F}"); // System/TurboBadgerUi/Shaders/ShaderAssets
		const Guid c_idTurboBadgerUiDefaultResources(L"{2719C20D-0066-0347-BD57-9866C5F7650B}"); // System/TurboBadgerUi/DefaultResources
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
			TurboBadgerUiViewAsset
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
		if (const TurboBadgerUiViewAsset* uiViewAsset = dynamic_type_cast<const TurboBadgerUiViewAsset*>(sourceAsset))
		{
			pipelineDepends->addDependency(traktor::Path(m_assetPath), uiViewAsset->getFileName().getOriginal());
		}
		pipelineDepends->addDependency(c_idTurboBadgerUiShaderAssets, editor::PdfBuild | editor::PdfResource);
		pipelineDepends->addDependency(c_idTurboBadgerUiDefaultResources, editor::PdfBuild | editor::PdfResource);
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
		Ref< TurboBadgerUiViewResource > uiViewResource;
		bool optimize = false;

		if (const TurboBadgerUiViewAsset* uiViewAsset = dynamic_type_cast<const TurboBadgerUiViewAsset*>(sourceAsset))
		{
			const traktor::Path filePath = FileSystem::getInstance().getAbsolutePath(traktor::Path(m_assetPath) + uiViewAsset->getFileName());
			Ref< IStream > sourceStream = FileSystem::getInstance().open(filePath, File::FmRead);
			if (!sourceStream)
			{
				log::error << L"Failed to import TurboBadgerUi asset; unable to open file \"" << uiViewAsset->getFileName().getOriginal() << L"\"." << Endl;
				return false;
			}

			uiViewResource = new TurboBadgerUiViewResource(
				uiViewAsset->getFileName(),
				uiViewAsset->getWidth(),
				uiViewAsset->getHeight()
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

		instance->setObject(uiViewResource);

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