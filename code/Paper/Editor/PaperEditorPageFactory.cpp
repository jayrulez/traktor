/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Serialization/DeepClone.h"
#include "Editor/IDocument.h"
#include "Paper/Editor/BitmapFontAsset.h"
#include "Paper/Editor/BitmapFontEditorPage.h"
#include "Paper/Editor/PaperEditorPageFactory.h"
#include "Paper/Editor/UIPageAsset.h"
#include "Paper/Editor/UIPageEditorPage.h"
#include "Ui/Command.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.PaperEditorPageFactory", 0, PaperEditorPageFactory, editor::IEditorPageFactory)

const TypeInfoSet PaperEditorPageFactory::getEditableTypes() const
{
	return makeTypeInfoSet< UIPageAsset, BitmapFontAsset >();
}

bool PaperEditorPageFactory::needOutputResources(const TypeInfo& typeInfo, std::set< Guid >& outDependencies) const
{
	return false;
}

Ref< editor::IEditorPage > PaperEditorPageFactory::createEditorPage(editor::IEditor* editor, editor::IEditorPageSite* site, editor::IDocument* document) const
{
	Ref< const ISerializable > asset = document->getObject(0);
	if (!asset)
		return nullptr;

	if (is_a< BitmapFontAsset >(asset))
		return new BitmapFontEditorPage(editor, site, document);
	else if (is_a< UIPageAsset >(asset))
		return new UIPageEditorPage(editor, site, document);

	return nullptr;
}

void PaperEditorPageFactory::getCommands(std::list< ui::Command >& outCommands) const
{
}

Ref< ISerializable > PaperEditorPageFactory::cloneAsset(const ISerializable* asset) const
{
	return DeepClone(asset).create();
}

}
