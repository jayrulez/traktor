/*
 * TRAKTOR
 * Copyright (c) 2022-2025 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Render/Editor/Shader/Facades/DefaultNodeFacade.h"

#include "Core/Misc/String.h"
#include "I18N/Text.h"
#include "Render/Editor/InputPin.h"
#include "Render/Editor/OutputPin.h"
#include "Render/Editor/Shader/Nodes.h"
#include "Ui/Application.h"
#include "Ui/Graph/DefaultNodeShape.h"
#include "Ui/Graph/GraphControl.h"
#include "Ui/Graph/InputNodeShape.h"
#include "Ui/Graph/Node.h"
#include "Ui/Graph/OutputNodeShape.h"
#include "Ui/Graph/Pin.h"

namespace traktor::render
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.render.DefaultNodeFacade", DefaultNodeFacade, INodeFacade)

DefaultNodeFacade::DefaultNodeFacade()
{
	m_nodeShapes[0] = new ui::DefaultNodeShape(ui::DefaultNodeShape::StDefault);
	m_nodeShapes[1] = new ui::InputNodeShape();
	m_nodeShapes[2] = new ui::OutputNodeShape();
}

Ref< Node > DefaultNodeFacade::createShaderNode(
	const TypeInfo* nodeType,
	editor::IEditor* editor)
{
	return checked_type_cast< Node* >(nodeType->createInstance());
}

Ref< ui::Node > DefaultNodeFacade::createEditorNode(
	editor::IEditor* editor,
	ui::GraphControl* graphControl,
	ShaderGraph* shaderGraph,
	Node* shaderNode)
{
	std::wstring title = type_name(shaderNode);
	size_t p = title.find_last_of(L'.');
	if (p > 0)
		title = i18n::Text(L"SHADERGRAPH_NODE_" + toUpper(title.substr(p + 1)));

	Ref< ui::INodeShape > shape;
	if (shaderNode->getInputPinCount() == 1 && shaderNode->getOutputPinCount() == 0)
		shape = m_nodeShapes[2];
	else if (shaderNode->getInputPinCount() == 0 && shaderNode->getOutputPinCount() == 1)
		shape = m_nodeShapes[1];
	else
		shape = m_nodeShapes[0];

	Ref< ui::Node > editorNode = graphControl->createNode(
		title,
		shaderNode->getInformation(),
		ui::UnitPoint(
			ui::Unit(shaderNode->getPosition().first),
			ui::Unit(shaderNode->getPosition().second)),
		shape);

	for (int j = 0; j < shaderNode->getInputPinCount(); ++j)
	{
		const InputPin* inputPin = shaderNode->getInputPin(j);
		editorNode->createInputPin(
			inputPin->getName(),
			inputPin->getId(),
			!inputPin->isOptional(),
			false);
	}

	for (int j = 0; j < shaderNode->getOutputPinCount(); ++j)
	{
		const OutputPin* outputPin = shaderNode->getOutputPin(j);
		editorNode->createOutputPin(
			outputPin->getName(),
			outputPin->getId());
	}

	editorNode->setComment(shaderNode->getComment());
	return editorNode;
}

void DefaultNodeFacade::editShaderNode(
	editor::IEditor* editor,
	ui::GraphControl* graphControl,
	ui::Node* editorNode,
	ShaderGraph* shaderGraph,
	Node* shaderNode)
{
}

void DefaultNodeFacade::refreshEditorNode(
	editor::IEditor* editor,
	ui::GraphControl* graphControl,
	ui::Node* editorNode,
	ShaderGraph* shaderGraph,
	Node* shaderNode)
{
	editorNode->setComment(shaderNode->getComment());
	editorNode->setInfo(shaderNode->getInformation());

	// Add or rename editor pins.
	for (int j = 0; j < shaderNode->getInputPinCount(); ++j)
	{
		const InputPin* inputPin = shaderNode->getInputPin(j);
		ui::Pin* editorInputPin = editorNode->findInputPin(inputPin->getId());
		if (editorInputPin)
			editorInputPin->setName(inputPin->getName());
		else
			editorNode->createInputPin(inputPin->getName(), inputPin->getId(), !inputPin->isOptional(), false);
	}

	for (int j = 0; j < shaderNode->getOutputPinCount(); ++j)
	{
		const OutputPin* outputPin = shaderNode->getOutputPin(j);
		ui::Pin* editorOutputPin = editorNode->findOutputPin(outputPin->getId());
		if (editorOutputPin)
			editorOutputPin->setName(outputPin->getName());
		else
			editorNode->createOutputPin(outputPin->getName(), outputPin->getId());
	}

	// Remove editors pins.
	RefArray< ui::Pin > removeEditorInputPins;
	for (auto editorInputPin : editorNode->getInputPins())
	{
		bool valid = false;
		for (int j = 0; j < shaderNode->getInputPinCount(); ++j)
		{
			if (shaderNode->getInputPin(j)->getId() == editorInputPin->getId())
			{
				valid = true;
				break;
			}
		}
		if (!valid)
			removeEditorInputPins.push_back(editorInputPin);
	}
	for (auto editorInputPin : removeEditorInputPins)
		editorNode->removeInputPin(editorInputPin);

	RefArray< ui::Pin > removeEditorOutputPins;
	for (auto editorOutputPin : editorNode->getOutputPins())
	{
		bool valid = false;
		for (int j = 0; j < shaderNode->getOutputPinCount(); ++j)
		{
			if (shaderNode->getOutputPin(j)->getId() == editorOutputPin->getId())
			{
				valid = true;
				break;
			}
		}
		if (!valid)
			removeEditorOutputPins.push_back(editorOutputPin);
	}
	for (auto editorOutputPin : removeEditorOutputPins)
		editorNode->removeOutputPin(editorOutputPin);
}

void DefaultNodeFacade::setValidationIndicator(
	ui::Node* editorNode,
	bool validationSucceeded)
{
	editorNode->setState(validationSucceeded ? 0 : 1);
}

}
