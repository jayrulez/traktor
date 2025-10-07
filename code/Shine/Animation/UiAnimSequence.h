/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <functional>
#include "Core/Object.h"
#include "Core/Ref.h"
#include "Core/Containers/AlignedVector.h"
#include "Shine/Animation/UiAnimNode.h"

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
 * \brief Animation sequence - a timeline containing multiple animated nodes
 *
 * Sequences can animate multiple UI elements simultaneously over a time range.
 */
class UiAnimSequence : public Object
{
	T_RTTI_CLASS;

public:
	UiAnimSequence();
	virtual ~UiAnimSequence();

	// === Sequence Info ===

	const std::wstring& getName() const { return m_name; }
	void setName(const std::wstring& name) { m_name = name; }

	uint32_t getId() const { return m_id; }
	void setId(uint32_t id) { m_id = id; }

	// === Time Range ===

	float getStartTime() const { return m_timeRange.x; }
	float getEndTime() const { return m_timeRange.y; }
	void setTimeRange(const Vector2& range) { m_timeRange = range; }

	// === Node Management ===

	/*!
	 * \brief Create and add a node for a UI element
	 */
	UiAnimNode* createNode(UIElement* target, const std::wstring& name = L"");

	/*!
	 * \brief Add existing node
	 */
	void addNode(UiAnimNode* node);

	/*!
	 * \brief Remove node
	 */
	void removeNode(UiAnimNode* node);

	/*!
	 * \brief Find node by target
	 */
	UiAnimNode* findNode(UIElement* target) const;

	/*!
	 * \brief Find node by name
	 */
	UiAnimNode* findNodeByName(const std::wstring& name) const;

	/*!
	 * \brief Get node by index
	 */
	UiAnimNode* getNode(int index) const;

	/*!
	 * \brief Get number of nodes
	 */
	int getNodeCount() const { return (int)m_nodes.size(); }

	// === Playback ===

	/*!
	 * \brief Animate all nodes at given time
	 */
	void animate(float time);

	/*!
	 * \brief Reset all nodes
	 */
	void reset();

	// === Events ===

	std::function<void()> onSequenceStart;
	std::function<void()> onSequenceEnd;

private:
	std::wstring m_name;
	uint32_t m_id;
	Vector2 m_timeRange;  // x = start, y = end
	AlignedVector<Ref<UiAnimNode>> m_nodes;
};

}
