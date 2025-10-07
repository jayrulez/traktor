/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Containers/AlignedVector.h"
#include "Shine/UIElement.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

/*!
 * \brief Spawner widget for dynamic UI element creation.
 *
 * Spawner creates instances of a prototype element at runtime,
 * useful for dynamic lists, pooling, procedural UI generation.
 *
 * Usage:
 *   Ref<Spawner> spawner = new Spawner(L"ItemSpawner");
 *   spawner->setPrototype(itemPrefab);
 *   spawner->setSpawnCount(10);
 *   spawner->spawn();
 */
class T_DLLCLASS Spawner : public UIElement
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit Spawner(const std::wstring& name = L"Spawner");

	/*!
	 * \brief Destructor
	 */
	virtual ~Spawner();

	// === Prototype ===

	/*!
	 * \brief Get prototype element
	 */
	UIElement* getPrototype() const { return m_prototype; }

	/*!
	 * \brief Set prototype element
	 */
	void setPrototype(UIElement* prototype);

	// === Spawning ===

	/*!
	 * \brief Spawn instances
	 * \param count Number of instances to spawn (0 = use spawn count)
	 */
	void spawn(int count = 0);

	/*!
	 * \brief Spawn single instance
	 * \return Spawned instance
	 */
	UIElement* spawnOne();

	/*!
	 * \brief Clear all spawned instances
	 */
	void clear();

	/*!
	 * \brief Get spawned instances
	 */
	const AlignedVector<Ref<UIElement>>& getSpawnedInstances() const { return m_spawnedInstances; }

	// === Configuration ===

	/*!
	 * \brief Get spawn count
	 */
	int getSpawnCount() const { return m_spawnCount; }

	/*!
	 * \brief Set spawn count
	 */
	void setSpawnCount(int count) { m_spawnCount = count; }

	/*!
	 * \brief Get whether to pool instances
	 */
	bool getPooling() const { return m_pooling; }

	/*!
	 * \brief Set whether to pool instances
	 */
	void setPooling(bool pooling) { m_pooling = pooling; }

	// === Events ===

	std::function<void(UIElement*)> onSpawn;
	std::function<void(UIElement*)> onDespawn;

private:
	Ref<UIElement> m_prototype;
	AlignedVector<Ref<UIElement>> m_spawnedInstances;
	AlignedVector<Ref<UIElement>> m_pool; // For pooling
	int m_spawnCount = 1;
	bool m_pooling = false;

	// Helper methods
	UIElement* createInstance();
	void returnToPool(UIElement* instance);
};

}
