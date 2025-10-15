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
#include "Paper/Ui/UITheme.h"
#include "Paper/Ui/UIStyle.h"
#include "Paper/Ui/Layouts/StackPanel.h"
#include "Paper/Ui/Controls/TextBlock.h"
#include "Paper/Ui/Controls/Rectangle.h"
#include "Paper/Ui/Controls/Border.h"
#include "Paper/Ui/Controls/Button.h"
#include "Paper/Ui/Controls/TextBox.h"
#include "Paper/Ui/Controls/ScrollViewer.h"
#include "Paper/Ui/Controls/ScrollBar.h"

namespace traktor::paper
{
namespace
{
const Guid c_idPaper2DShader(L"{A704DB1C-60E6-9D44-AD1D-F7822568242D}"); // System/Paper/Shaders/Paper2D
const Guid c_idPaper2DDefaultFont(L"{970AEEAF-EC6F-8F47-883C-700237C278C4}"); // System/Paper/Fonts/Roboto_Bold_16

// Create a nice modern dark theme
Ref< UITheme > createDarkTheme(const Guid& defaultFontId)
{
	Ref< UITheme > theme = new UITheme();

	// Primary Text Style - White text
	Ref< UIStyle > primaryText = new UIStyle();
	primaryText->setColor(L"Foreground", Color4f(1.0f, 1.0f, 1.0f, 1.0f));
	primaryText->setFont(L"Font", defaultFontId);
	theme->setStyle(L"PrimaryText", primaryText);

	// Secondary Text Style - Light gray text
	Ref< UIStyle > secondaryText = new UIStyle();
	secondaryText->setColor(L"Foreground", Color4f(0.7f, 0.7f, 0.7f, 1.0f));
	secondaryText->setFont(L"Font", defaultFontId);
	theme->setStyle(L"SecondaryText", secondaryText);

	// Accent Text Style - Cyan accent
	Ref< UIStyle > accentText = new UIStyle();
	accentText->setColor(L"Foreground", Color4f(0.3f, 0.8f, 1.0f, 1.0f));
	accentText->setFont(L"Font", defaultFontId);
	theme->setStyle(L"AccentText", accentText);

	// Button Style - Dark blue with border
	Ref< UIStyle > button = new UIStyle();
	button->setColor(L"Background", Color4f(0.15f, 0.25f, 0.35f, 1.0f));
	button->setColor(L"BorderBrush", Color4f(0.3f, 0.5f, 0.7f, 1.0f));
	button->setDimension(L"BorderThickness", 2.0f);
	button->setVector(L"Padding", Vector2(20.0f, 10.0f));
	theme->setStyle(L"Button", button);

	// Panel Style - Dark background with subtle border
	Ref< UIStyle > panel = new UIStyle();
	panel->setColor(L"Background", Color4f(0.1f, 0.1f, 0.12f, 1.0f));
	panel->setColor(L"BorderBrush", Color4f(0.2f, 0.2f, 0.25f, 1.0f));
	panel->setDimension(L"BorderThickness", 1.0f);
	panel->setVector(L"Padding", Vector2(15.0f, 15.0f));
	theme->setStyle(L"Panel", panel);

	// Card Style - Lighter background with accent border
	Ref< UIStyle > card = new UIStyle();
	card->setColor(L"Background", Color4f(0.15f, 0.15f, 0.18f, 1.0f));
	card->setColor(L"BorderBrush", Color4f(0.3f, 0.8f, 1.0f, 0.5f));
	card->setDimension(L"BorderThickness", 2.0f);
	card->setVector(L"Padding", Vector2(12.0f, 12.0f));
	theme->setStyle(L"Card", card);

	// Separator Style - Thin accent line
	Ref< UIStyle > separator = new UIStyle();
	separator->setColor(L"Fill", Color4f(0.3f, 0.8f, 1.0f, 0.3f));
	separator->setDimension(L"Height", 2.0f);
	theme->setStyle(L"Separator", separator);

	// Status Indicator Styles
	Ref< UIStyle > statusSuccess = new UIStyle();
	statusSuccess->setColor(L"Fill", Color4f(0.2f, 0.8f, 0.3f, 1.0f));
	statusSuccess->setDimension(L"Width", 8.0f);
	statusSuccess->setDimension(L"Height", 8.0f);
	theme->setStyle(L"StatusSuccess", statusSuccess);

	Ref< UIStyle > statusWarning = new UIStyle();
	statusWarning->setColor(L"Fill", Color4f(1.0f, 0.7f, 0.2f, 1.0f));
	statusWarning->setDimension(L"Width", 8.0f);
	statusWarning->setDimension(L"Height", 8.0f);
	theme->setStyle(L"StatusWarning", statusWarning);

	Ref< UIStyle > statusError = new UIStyle();
	statusError->setColor(L"Fill", Color4f(0.9f, 0.2f, 0.2f, 1.0f));
	statusError->setDimension(L"Width", 8.0f);
	statusError->setDimension(L"Height", 8.0f);
	theme->setStyle(L"StatusError", statusError);

	// TextBox Style - Dark background with subtle border
	Ref< UIStyle > textBox = new UIStyle();
	textBox->setColor(L"Foreground", Color4f(1.0f, 1.0f, 1.0f, 1.0f));
	textBox->setColor(L"Background", Color4f(0.08f, 0.08f, 0.10f, 1.0f));
	textBox->setColor(L"BorderBrush", Color4f(0.3f, 0.3f, 0.35f, 1.0f));
	textBox->setDimension(L"BorderThickness", 1.0f);
	textBox->setVector(L"Padding", Vector2(8.0f, 6.0f));
	textBox->setFont(L"Font", defaultFontId);
	theme->setStyle(L"TextBox", textBox);

	return theme;
}
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

	// Default font ID - replace with actual font GUID
	// TODO: Make this configurable from UIPageAsset
	const Guid defaultFontId = c_idPaper2DDefaultFont;

	// Create and set theme
	Ref< UITheme > theme = createDarkTheme(defaultFontId);
	uiPage->setTheme(theme);

	// Create root vertical stack panel
	Ref< StackPanel > rootPanel = new StackPanel();
	rootPanel->setOrientation(StackPanel::Orientation::Vertical);

	// --- Header Section ---
	Ref< Border > headerPanel = new Border();
	headerPanel->applyStyle(theme->getStyle(L"Panel"));
	Ref< StackPanel > headerContent = new StackPanel();
	headerContent->setOrientation(StackPanel::Orientation::Vertical);

	Ref< TextBlock > title = new TextBlock();
	title->setText(L"UI Theme Demo");
	title->applyStyle(theme->getStyle(L"AccentText"));
	headerContent->addChild(title);

	Ref< TextBlock > subtitle = new TextBlock();
	subtitle->setText(L"Showcasing the modern dark theme");
	subtitle->applyStyle(theme->getStyle(L"SecondaryText"));
	headerContent->addChild(subtitle);

	headerPanel->setChild(headerContent);
	rootPanel->addChild(headerPanel);

	// --- Separator ---
	Ref< Rectangle > separator1 = new Rectangle();
	separator1->applyStyle(theme->getStyle(L"Separator"));
	rootPanel->addChild(separator1);

	// --- Cards Section ---
	Ref< StackPanel > cardsRow = new StackPanel();
	cardsRow->setOrientation(StackPanel::Orientation::Horizontal);

	// Card 1 - Status Success
	Ref< Border > card1 = new Border();
	card1->applyStyle(theme->getStyle(L"Card"));
	Ref< StackPanel > card1Content = new StackPanel();
	card1Content->setOrientation(StackPanel::Orientation::Horizontal);
	Ref< Rectangle > statusSuccess = new Rectangle();
	statusSuccess->applyStyle(theme->getStyle(L"StatusSuccess"));
	card1Content->addChild(statusSuccess);
	Ref< TextBlock > card1Text = new TextBlock();
	card1Text->setText(L" System Online");
	card1Text->applyStyle(theme->getStyle(L"PrimaryText"));
	card1Content->addChild(card1Text);
	card1->setChild(card1Content);
	cardsRow->addChild(card1);

	// Card 2 - Status Warning
	Ref< Border > card2 = new Border();
	card2->applyStyle(theme->getStyle(L"Card"));
	Ref< StackPanel > card2Content = new StackPanel();
	card2Content->setOrientation(StackPanel::Orientation::Horizontal);
	Ref< Rectangle > statusWarning = new Rectangle();
	statusWarning->applyStyle(theme->getStyle(L"StatusWarning"));
	card2Content->addChild(statusWarning);
	Ref< TextBlock > card2Text = new TextBlock();
	card2Text->setText(L" High Memory Usage");
	card2Text->applyStyle(theme->getStyle(L"PrimaryText"));
	card2Content->addChild(card2Text);
	card2->setChild(card2Content);
	cardsRow->addChild(card2);

	// Card 3 - Status Error
	Ref< Border > card3 = new Border();
	card3->applyStyle(theme->getStyle(L"Card"));
	Ref< StackPanel > card3Content = new StackPanel();
	card3Content->setOrientation(StackPanel::Orientation::Horizontal);
	Ref< Rectangle > statusError = new Rectangle();
	statusError->applyStyle(theme->getStyle(L"StatusError"));
	card3Content->addChild(statusError);
	Ref< TextBlock > card3Text = new TextBlock();
	card3Text->setText(L" Connection Lost");
	card3Text->applyStyle(theme->getStyle(L"PrimaryText"));
	card3Content->addChild(card3Text);
	card3->setChild(card3Content);
	cardsRow->addChild(card3);

	rootPanel->addChild(cardsRow);

	// --- Separator ---
	Ref< Rectangle > separator2 = new Rectangle();
	separator2->applyStyle(theme->getStyle(L"Separator"));
	rootPanel->addChild(separator2);

	// --- Buttons Section ---
	Ref< Border > buttonsPanel = new Border();
	buttonsPanel->applyStyle(theme->getStyle(L"Panel"));
	Ref< StackPanel > buttonsRow = new StackPanel();
	buttonsRow->setOrientation(StackPanel::Orientation::Horizontal);

	// Button 1 - Success
	Ref< Button > button1 = new Button();
	button1->setName(L"SuccessButton");
	button1->setText(L"Success");
	button1->setFontId(defaultFontId);
	button1->applyStyle(theme->getStyle(L"Button"));
	buttonsRow->addChild(button1);

	// Button 2 - Warning
	Ref< Button > button2 = new Button();
	button2->setName(L"WarningButton");
	button2->setText(L"Warning");
	button2->setFontId(defaultFontId);
	button2->applyStyle(theme->getStyle(L"Button"));
	buttonsRow->addChild(button2);

	// Button 3 - Error
	Ref< Button > button3 = new Button();
	button3->setName(L"ErrorButton");
	button3->setText(L"Error");
	button3->setFontId(defaultFontId);
	button3->applyStyle(theme->getStyle(L"Button"));
	buttonsRow->addChild(button3);

	buttonsPanel->setChild(buttonsRow);
	rootPanel->addChild(buttonsPanel);

	// --- Separator ---
	Ref< Rectangle > separator3 = new Rectangle();
	separator3->applyStyle(theme->getStyle(L"Separator"));
	rootPanel->addChild(separator3);

	// --- Feedback Section ---
	Ref< Border > feedbackPanel = new Border();
	feedbackPanel->applyStyle(theme->getStyle(L"Panel"));
	Ref< TextBlock > feedbackText = new TextBlock();
	feedbackText->setName(L"FeedbackText");
	feedbackText->setText(L"Click a button to see feedback here...");
	feedbackText->applyStyle(theme->getStyle(L"AccentText"));
	feedbackPanel->setChild(feedbackText);
	rootPanel->addChild(feedbackPanel);

	// --- Separator ---
	Ref< Rectangle > separator4 = new Rectangle();
	separator4->applyStyle(theme->getStyle(L"Separator"));
	rootPanel->addChild(separator4);

	// --- TextBox Demo Section ---
	Ref< Border > textBoxPanel = new Border();
	textBoxPanel->applyStyle(theme->getStyle(L"Panel"));
	Ref< StackPanel > textBoxContent = new StackPanel();
	textBoxContent->setOrientation(StackPanel::Orientation::Vertical);

	Ref< TextBlock > textBoxLabel = new TextBlock();
	textBoxLabel->setText(L"TextBox Control (click to focus):");
	textBoxLabel->applyStyle(theme->getStyle(L"PrimaryText"));
	textBoxContent->addChild(textBoxLabel);

	Ref< TextBox > textBox1 = new TextBox();
	textBox1->setName(L"DemoTextBox");
	textBox1->setText(L"Hello, World!");
	textBox1->setPlaceholder(L"Enter text here...");
	textBox1->setFontId(defaultFontId);
	textBox1->applyStyle(theme->getStyle(L"TextBox"));
	textBoxContent->addChild(textBox1);

	textBoxPanel->setChild(textBoxContent);
	rootPanel->addChild(textBoxPanel);

	// --- Separator ---
	Ref< Rectangle > separator5 = new Rectangle();
	separator5->applyStyle(theme->getStyle(L"Separator"));
	rootPanel->addChild(separator5);

	// --- ScrollViewer Demo Section ---
	Ref< Border > scrollPanel = new Border();
	scrollPanel->applyStyle(theme->getStyle(L"Panel"));
	Ref< StackPanel > scrollPanelContent = new StackPanel();
	scrollPanelContent->setOrientation(StackPanel::Orientation::Vertical);

	Ref< TextBlock > scrollLabel = new TextBlock();
	scrollLabel->setText(L"ScrollViewer Control (drag scrollbar or click track):");
	scrollLabel->applyStyle(theme->getStyle(L"PrimaryText"));
	scrollPanelContent->addChild(scrollLabel);

	// Create ScrollViewer with large content
	Ref< ScrollViewer > scrollViewer = new ScrollViewer();
	scrollViewer->setVerticalScrollBarVisibility(ScrollBarVisibility::Auto);
	scrollViewer->setHorizontalScrollBarVisibility(ScrollBarVisibility::Auto);
	// Set explicit height to constrain the viewport and enable scrolling
	scrollViewer->setHeight(200.0f);

	// Create content that's larger than viewport
	Ref< StackPanel > scrollContent = new StackPanel();
	scrollContent->setOrientation(StackPanel::Orientation::Vertical);

	// Add multiple text blocks to demonstrate scrolling
	for (int i = 1; i <= 20; ++i)
	{
		Ref< TextBlock > item = new TextBlock();
		item->setText(std::wstring(L"Scrollable Item #") + std::to_wstring(i));
		item->applyStyle(theme->getStyle(L"SecondaryText"));
		scrollContent->addChild(item);
	}

	scrollViewer->setContent(scrollContent);
	scrollPanelContent->addChild(scrollViewer);

	scrollPanel->setChild(scrollPanelContent);
	rootPanel->addChild(scrollPanel);

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
