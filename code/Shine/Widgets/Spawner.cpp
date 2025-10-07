/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/Spawner.h"
#include "Core/Misc/SafeDestroy.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Spawner", Spawner, UIElement)

Spawner::Spawner(const std::wstring& name)
:	UIElement(name)
{
}

Spawner::~Spawner()
{
	clear();
}

void Spawner::setPrototype(UIElement* prototype)
{
	m_prototype = prototype;
}

void Spawner::spawn(int count)
{
	if (!m_prototype)
		return;

	if (count <= 0)
		count = m_spawnCount;

	for (int i = 0; i < count; ++i)
		spawnOne();
}

UIElement* Spawner::spawnOne()
{
	if (!m_prototype)
		return nullptr;

	UIElement* instance = createInstance();
	if (instance)
	{
		m_spawnedInstances.push_back(instance);
		addChild(instance);

		if (onSpawn)
			onSpawn(instance);
	}

	return instance;
}

void Spawner::clear()
{
	// Despawn all instances
	for (auto& instance : m_spawnedInstances)
	{
		if (onDespawn)
			onDespawn(instance);

		if (m_pooling)
			returnToPool(instance);
		else
			removeChild(instance);
	}

	m_spawnedInstances.clear();

	// Clear pool if not pooling
	if (!m_pooling)
	{
		m_pool.clear();
	}
}

UIElement* Spawner::createInstance()
{
	UIElement* instance = nullptr;

	// Try to get from pool first
	if (m_pooling && !m_pool.empty())
	{
		instance = m_pool.back();
		m_pool.pop_back();
	}
	else
	{
		// Clone prototype
		// Note: This is a simplified implementation
		// A real implementation would need a proper cloning system
		// For now, we just return nullptr to indicate cloning is not implemented
		// TODO: Implement proper UI element cloning
		instance = nullptr;
	}

	return instance;
}

void Spawner::returnToPool(UIElement* instance)
{
	if (instance)
	{
		removeChild(instance);
		m_pool.push_back(instance);
	}
}

}
