/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Paper/Editor/UIPagePipeline.h"

#include "Core/Log/Log.h"
#include "Core/Serialization/DeepHash.h"
#include "Core/Settings/PropertyString.h"
#include "Database/Instance.h"
#include "Editor/IPipelineBuilder.h"
#include "Editor/IPipelineDepends.h"
#include "Editor/IPipelineSettings.h"
#include "Paper/Editor/UIPageAsset.h"
#include "Paper/Ui/UIPage.h"
#include "Paper/Ui/Layouts/StackPanel.h"
#include "Paper/Ui/Controls/TextBlock.h"

namespace traktor::paper
{
namespace
{
const Guid c_idPaper2DShader(L"{A704DB1C-60E6-9D44-AD1D-F7822568242D}"); // System/Paper/Shaders/Paper2D
const Guid c_idPaper2DDefaultFont(L"{970AEEAF-EC6F-8F47-883C-700237C278C4}"); // System/Paper/Fonts/Roboto_Bold_16
}

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.UIPagePipeline", 0, UIPagePipeline, editor::IPipeline)

bool UIPagePipeline::create(const editor::IPipelineSettings* settings, db::Database* database)
{
	m_assetPath = settings->getPropertyExcludeHash< std::wstring >(L"Pipeline.AssetPath", L"");
	return true;
}

void UIPagePipeline::destroy()
{
}

TypeInfoSet UIPagePipeline::getAssetTypes() const
{
	return makeTypeInfoSet< UIPageAsset >();
}

bool UIPagePipeline::shouldCache() const
{
	return true;
}

uint32_t UIPagePipeline::hashAsset(const ISerializable* sourceAsset) const
{
	return DeepHash(sourceAsset).get();
}

bool UIPagePipeline::buildDependencies(
	editor::IPipelineDepends* pipelineDepends,
	const db::Instance* sourceInstance,
	const ISerializable* sourceAsset,
	const std::wstring& outputPath,
	const Guid& outputGuid) const
{
	// Add dependency on Paper2D shader
	pipelineDepends->addDependency(c_idPaper2DShader, editor::PdfBuild | editor::PdfResource);
	pipelineDepends->addDependency(c_idPaper2DDefaultFont, editor::PdfBuild | editor::PdfResource);
	return true;
}

bool UIPagePipeline::buildOutput(
	editor::IPipelineBuilder* pipelineBuilder,
	const editor::PipelineDependencySet* dependencySet,
	const editor::PipelineDependency* dependency,
	const db::Instance* sourceInstance,
	const ISerializable* sourceAsset,
	const std::wstring& outputPath,
	const Guid& outputGuid,
	const Object* buildParams,
	uint32_t reason) const
{
	const UIPageAsset* uiPageAsset = mandatory_non_null_type_cast< const UIPageAsset* >(sourceAsset);

	// Create UIPage with test UI hierarchy
	Ref< UIPage > uiPage = new UIPage();
	uiPage->setWidth(uiPageAsset->getWidth());
	uiPage->setHeight(uiPageAsset->getHeight());

	// Create root vertical stack panel
	Ref< StackPanel > rootPanel = new StackPanel();
	rootPanel->setOrientation(StackPanel::Orientation::Vertical);

	// Default font ID - replace with actual font GUID
	// TODO: Make this configurable from UIPageAsset
	const Guid defaultFontId = c_idPaper2DDefaultFont;

	// Create first vertical stack panel with text elements
	Ref< StackPanel > verticalPanel = new StackPanel();
	verticalPanel->setOrientation(StackPanel::Orientation::Vertical);

	Ref< TextBlock > text1 = new TextBlock();
	text1->setText(L"First Text");
	text1->setFontId(defaultFontId);
	verticalPanel->addChild(text1);

	Ref< TextBlock > text2 = new TextBlock();
	text2->setText(L"Second Text");
	text2->setFontId(defaultFontId);
	verticalPanel->addChild(text2);

	Ref< TextBlock > text3 = new TextBlock();
	text3->setText(L"Third Text");
	text3->setFontId(defaultFontId);
	verticalPanel->addChild(text3);

	rootPanel->addChild(verticalPanel);

	// Create second horizontal stack panel with text elements
	Ref< StackPanel > horizontalPanel = new StackPanel();
	horizontalPanel->setOrientation(StackPanel::Orientation::Horizontal);

	Ref< TextBlock > text4 = new TextBlock();
	text4->setText(L"Horizontal 1");
	text4->setFontId(defaultFontId);
	horizontalPanel->addChild(text4);

	Ref< TextBlock > text5 = new TextBlock();
	text5->setText(L"Horizontal 2");
	text5->setFontId(defaultFontId);
	horizontalPanel->addChild(text5);

	Ref< TextBlock > text6 = new TextBlock();
	text6->setText(L"Horizontal 3");
	text6->setFontId(defaultFontId);
	horizontalPanel->addChild(text6);

	rootPanel->addChild(horizontalPanel);

	// Set root
	uiPage->setRoot(rootPanel);

	log::info << L"UIPagePipeline: Created UIPage with root element" << Endl;
	if (uiPage->getRoot())
		log::info << L"UIPagePipeline: Root element is valid" << Endl;
	else
		log::error << L"UIPagePipeline: Root element is NULL!" << Endl;

	// Save UIPage
	Ref< db::Instance > instance = pipelineBuilder->createOutputInstance(outputPath, outputGuid);
	if (!instance)
	{
		log::error << L"Failed to create output instance." << Endl;
		return false;
	}

	instance->setObject(uiPage);

	if (!instance->commit())
	{
		log::error << L"Failed to commit instance." << Endl;
		return false;
	}

	log::info << L"UIPagePipeline: Successfully saved UIPage" << Endl;
	return true;
}

Ref< ISerializable > UIPagePipeline::buildProduct(
	editor::IPipelineBuilder* pipelineBuilder,
	const db::Instance* sourceInstance,
	const ISerializable* sourceAsset,
	const Object* buildParams) const
{
	T_FATAL_ERROR;
	return nullptr;
}

}
