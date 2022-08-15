#include "Core/Serialization/DeepClone.h"
#include "Render/Editor/Texture/TextureAsset.h"
#include "Render/Editor/Texture/TextureAssetEditorPage.h"
#include "Render/Editor/Texture/TextureAssetEditorPageFactory.h"

namespace traktor
{
	namespace render
	{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.render.TextureAssetEditorPageFactory", 0, TextureAssetEditorPageFactory, editor::IEditorPageFactory)

const TypeInfoSet TextureAssetEditorPageFactory::getEditableTypes() const
{
	return makeTypeInfoSet< TextureAsset >();
}

bool TextureAssetEditorPageFactory::needOutputResources(const TypeInfo& typeInfo, std::set< Guid >& outDependencies) const
{
	return false;
}

Ref< editor::IEditorPage > TextureAssetEditorPageFactory::createEditorPage(editor::IEditor* editor, editor::IEditorPageSite* site, editor::IDocument* document) const
{
	return new TextureAssetEditorPage(editor, site, document);
}

void TextureAssetEditorPageFactory::getCommands(std::list< ui::Command >& outCommands) const
{
}

Ref< ISerializable > TextureAssetEditorPageFactory::cloneAsset(const ISerializable* asset) const
{
	return DeepClone(asset).create();
}

	}
}