/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/Editor/TurboBadgerUiEditorPageFactory.h"
#include "TurboBadgerUi/Editor/TurboBadgerUiEditorPage.h"
#include "TurboBadgerUi/Editor/TurboBadgerUiAsset.h"

#include "Core/Serialization/DeepClone.h"
#include "Ui/Command.h"

namespace traktor::turbobadgerui
{
	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.turbobadgerui.TurboBadgerUiEditorPageFactory", 0, TurboBadgerUiEditorPageFactory, editor::IEditorPageFactory)

		const TypeInfoSet TurboBadgerUiEditorPageFactory::getEditableTypes() const
	{
		return makeTypeInfoSet< TurboBadgerUiAsset >();
	}

	bool TurboBadgerUiEditorPageFactory::needOutputResources(const TypeInfo& typeInfo, std::set< Guid >& outDependencies) const
	{
		outDependencies.insert(Guid(L"{7F092F0F-C5FB-834F-B9EE-C08E03A844B4}"));	// System/TurboBadger/Shaders/ShaderAssets
		return true;
	}

	Ref< editor::IEditorPage > TurboBadgerUiEditorPageFactory::createEditorPage(editor::IEditor* editor, editor::IEditorPageSite* site, editor::IDocument* document) const
	{
		return new TurboBadgerUiEditorPage(editor, site, document);
	}

	void TurboBadgerUiEditorPageFactory::getCommands(std::list< ui::Command >& outCommands) const
	{
		outCommands.push_back(ui::Command(L"TurboBadgerUi.Editor.Reload"));
	}

	Ref< ISerializable > TurboBadgerUiEditorPageFactory::cloneAsset(const ISerializable* asset) const
	{
		return DeepClone(asset).create();
	}
}