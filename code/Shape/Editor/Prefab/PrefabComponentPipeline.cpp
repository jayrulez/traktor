#include "Core/Settings/PropertyBoolean.h"
#include "Core/Settings/PropertyFloat.h"
#include "Core/Settings/PropertyString.h"
#include "Editor/IPipelineBuilder.h"
#include "Editor/IPipelineDepends.h"
#include "Editor/IPipelineSettings.h"
#include "Shape/Editor/Prefab/PrefabComponentData.h"
#include "Shape/Editor/Prefab/PrefabComponentPipeline.h"

namespace traktor
{
	namespace shape
	{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.shape.PrefabComponentPipeline", 1, PrefabComponentPipeline, world::EntityPipeline)

PrefabComponentPipeline::PrefabComponentPipeline()
:	m_editor(false)
{
}

bool PrefabComponentPipeline::create(const editor::IPipelineSettings* settings)
{
	m_editor = settings->getProperty< bool >(L"Pipeline.TargetEditor", false);
	return true;
}

TypeInfoSet PrefabComponentPipeline::getAssetTypes() const
{
	return makeTypeInfoSet< PrefabComponentData >();
}

bool PrefabComponentPipeline::buildDependencies(
	editor::IPipelineDepends* pipelineDepends,
	const db::Instance* sourceInstance,
	const ISerializable* sourceAsset,
	const std::wstring& outputPath,
	const Guid& outputGuid
) const
{
	return world::EntityPipeline::buildDependencies(
		pipelineDepends,
		sourceInstance,
		sourceAsset,
		outputPath,
		outputGuid
	);
}

Ref< ISerializable > PrefabComponentPipeline::buildOutput(
	editor::IPipelineBuilder* pipelineBuilder,
	const db::Instance* sourceInstance,
	const ISerializable* sourceAsset,
	const Object* buildParams
) const
{
	// Do not build prefab in editor since it's time consuming and not
	// strictly necessary for editing purposes.
	if (m_editor)
	{
		return world::EntityPipeline::buildOutput(
			pipelineBuilder,
			sourceInstance,
			sourceAsset,
			buildParams
		);
	}

	// Transform component into a plain group component.
	const PrefabComponentData* prefabComponent = mandatory_non_null_type_cast< const PrefabComponentData* >(sourceAsset);
	return world::EntityPipeline::buildOutput(
		pipelineBuilder,
		sourceInstance,
		new world::GroupComponentData(prefabComponent->getEntityData()),
		buildParams
	);
}

	}
}