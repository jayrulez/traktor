/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Serialization/DeepClone.h"
#include "Paper/Editor/PaperEditorPageFactory.h"
#include "Paper/Editor/UIPageAsset.h"
#include "Paper/Editor/UIPageEditorPage.h"
#include "Ui/Command.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.PaperEditorPageFactory", 0, PaperEditorPageFactory, editor::IEditorPageFactory)

const TypeInfoSet PaperEditorPageFactory::getEditableTypes() const
{
	return makeTypeInfoSet< UIPageAsset >();
}

bool PaperEditorPageFactory::needOutputResources(const TypeInfo& typeInfo, std::set< Guid >& outDependencies) const
{
	return false;
}

Ref< editor::IEditorPage > PaperEditorPageFactory::createEditorPage(editor::IEditor* editor, editor::IEditorPageSite* site, editor::IDocument* document) const
{
	return new UIPageEditorPage(editor, site, document);
}

void PaperEditorPageFactory::getCommands(std::list< ui::Command >& outCommands) const
{
}

Ref< ISerializable > PaperEditorPageFactory::cloneAsset(const ISerializable* asset) const
{
	return DeepClone(asset).create();
}

}
