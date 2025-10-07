/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Animation/UiAnimSequence.h"
#include "Shine/UIElement.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.UiAnimSequence", UiAnimSequence, Object)

UiAnimSequence::UiAnimSequence()
:	m_id(0)
,	m_timeRange(0.0f, 0.0f)
{
}

UiAnimSequence::~UiAnimSequence()
{
}

UiAnimNode* UiAnimSequence::createNode(UIElement* target, const std::wstring& name)
{
	if (!target)
		return nullptr;

	Ref<UiAnimNode> node = new UiAnimNode();
	node->setTarget(target);
	node->setName(name.empty() ? target->getName() : name);

	m_nodes.push_back(node);
	return node;
}

void UiAnimSequence::addNode(UiAnimNode* node)
{
	if (node)
		m_nodes.push_back(node);
}

void UiAnimSequence::removeNode(UiAnimNode* node)
{
	for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it)
	{
		if (*it == node)
		{
			m_nodes.erase(it);
			return;
		}
	}
}

UiAnimNode* UiAnimSequence::findNode(UIElement* target) const
{
	for (const auto& node : m_nodes)
	{
		if (node->getTarget() == target)
			return node;
	}
	return nullptr;
}

UiAnimNode* UiAnimSequence::findNodeByName(const std::wstring& name) const
{
	for (const auto& node : m_nodes)
	{
		if (node->getName() == name)
			return node;
	}
	return nullptr;
}

UiAnimNode* UiAnimSequence::getNode(int index) const
{
	if (index >= 0 && index < (int)m_nodes.size())
		return m_nodes[index];
	return nullptr;
}

void UiAnimSequence::animate(float time)
{
	// Clamp time to range
	time = clamp(time, m_timeRange.x, m_timeRange.y);

	// Animate all nodes
	for (const auto& node : m_nodes)
	{
		node->animate(time);
	}
}

void UiAnimSequence::reset()
{
	for (const auto& node : m_nodes)
	{
		node->reset();
	}
}

}
