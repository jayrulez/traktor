/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
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
#include "RmlUi/RmlDocumentResource.h"
#include "RmlUi/Editor/RmlDocumentAsset.h"
#include "RmlUi/Editor/Pipeline.h"

namespace traktor::rmlui
{
	namespace
	{

		const Guid c_idRmlUiShaderAssets(L"{7F092F0F-C5FB-834F-B9EE-C08E03A844B4}");

	}

	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.rmlui.Pipeline", 0, Pipeline, editor::IPipeline)

		Pipeline::Pipeline()
	{
	}

	bool Pipeline::create(const editor::IPipelineSettings* settings, db::Database* database)
	{
		m_assetPath = settings->getPropertyExcludeHash< std::wstring >(L"Pipeline.AssetPath", L"");
		return true;
	}

	void Pipeline::destroy()
	{
	}

	TypeInfoSet Pipeline::getAssetTypes() const
	{
		return makeTypeInfoSet<
			RmlDocumentAsset
		>();
	}

	bool Pipeline::shouldCache() const
	{
		return true;
	}

	uint32_t Pipeline::hashAsset(const ISerializable* sourceAsset) const
	{
		return DeepHash(sourceAsset).get();
	}

	bool Pipeline::buildDependencies(
		editor::IPipelineDepends* pipelineDepends,
		const db::Instance* sourceInstance,
		const ISerializable* sourceAsset,
		const std::wstring& outputPath,
		const Guid& outputGuid
	) const
	{
		if (const RmlDocumentAsset* rmlDocumentAsset = dynamic_type_cast<const RmlDocumentAsset*>(sourceAsset))
		{
			pipelineDepends->addDependency(traktor::Path(m_assetPath), rmlDocumentAsset->getFileName().getOriginal());
			//for (const auto& font : rmlDocumentAsset->getFonts())
			//	pipelineDepends->addDependency(traktor::Path(m_assetPath), font.fileName.getOriginal());
		}
		pipelineDepends->addDependency(c_idRmlUiShaderAssets, editor::PdfBuild | editor::PdfResource);
		pipelineDepends->addDependency< render::TextureOutput >();
		return true;
	}

	bool Pipeline::buildOutput(
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
		Ref< RmlDocumentResource > rmlDocument;
		bool optimize = false;

		if (const RmlDocumentAsset* rmlDocumentAsset = dynamic_type_cast<const RmlDocumentAsset*>(sourceAsset))
		{
			const traktor::Path filePath = FileSystem::getInstance().getAbsolutePath(traktor::Path(m_assetPath) + rmlDocumentAsset->getFileName());
			Ref< IStream > sourceStream = FileSystem::getInstance().open(filePath, File::FmRead);
			if (!sourceStream)
			{
				log::error << L"Failed to import Rml document; unable to open file \"" << rmlDocumentAsset->getFileName().getOriginal() << L"\"." << Endl;
				return false;
			}

			rmlDocument = new RmlDocumentResource();// RmlDocumentFactory().createRmlDocument(traktor::Path(m_assetPath), sourceInstance, sourceStream);

			safeClose(sourceStream);
		}

		// Show some information about the document.
		log::info << L"Rml document successfully loaded," << Endl;
		log::info << IncreaseIndent;
		log::info << DecreaseIndent;

		Ref< db::Instance > instance = pipelineBuilder->createOutputInstance(
			outputPath,
			outputGuid
		);
		if (!instance)
		{
			log::error << L"Failed to import Rml document; unable to create instance." << Endl;
			return false;
		}

		instance->setObject(rmlDocument);

		if (!instance->commit())
		{
			log::info << L"Failed to import Rml document; unable to commit instance." << Endl;
			return false;
		}

		return true;
	}

	Ref< ISerializable > Pipeline::buildProduct(
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