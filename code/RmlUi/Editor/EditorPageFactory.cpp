/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Serialization/DeepClone.h"
#include "RmlUi/Editor/EditorPage.h"
#include "RmlUi/Editor/EditorPageFactory.h"
#include "RmlUi/Editor/RmlDocumentAsset.h"
#include "Ui/Command.h"

namespace traktor
{
	namespace rmlui
	{

		T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.rmlui.EditorPageFactory", 0, EditorPageFactory, editor::IEditorPageFactory)

			const TypeInfoSet EditorPageFactory::getEditableTypes() const
		{
			return makeTypeInfoSet< RmlDocumentAsset >();
		}

		bool EditorPageFactory::needOutputResources(const TypeInfo& typeInfo, std::set< Guid >& outDependencies) const
		{
			outDependencies.insert(Guid(L"{7F092F0F-C5FB-834F-B9EE-C08E03A844B4}"));	// System/RmlUi/Shaders/ShaderAssets
			return true;
		}

		Ref< editor::IEditorPage > EditorPageFactory::createEditorPage(editor::IEditor* editor, editor::IEditorPageSite* site, editor::IDocument* document) const
		{
			return new EditorPage(editor, site, document);
		}

		void EditorPageFactory::getCommands(std::list< ui::Command >& outCommands) const
		{
			outCommands.push_back(ui::Command(L"RmlUi.Editor.Reload"));
		}

		Ref< ISerializable > EditorPageFactory::cloneAsset(const ISerializable* asset) const
		{
			return DeepClone(asset).create();
		}

	}
}