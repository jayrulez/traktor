/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Math/Vector2.h"
#include "Shine/Widgets/Navigation.h"
#include "Shine/Interactable.h"
#include "Shine/UIElement.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Navigation", Navigation, Object)

Navigation::Navigation()
{
}

Navigation::~Navigation()
{
}

Interactable* Navigation::getNavigationTarget(Direction direction) const
{
	switch (direction)
	{
	case Direction::Up:
		return m_navigationUp;
	case Direction::Down:
		return m_navigationDown;
	case Direction::Left:
		return m_navigationLeft;
	case Direction::Right:
		return m_navigationRight;
	default:
		return nullptr;
	}
}

void Navigation::setNavigationTarget(Direction direction, Interactable* target)
{
	switch (direction)
	{
	case Direction::Up:
		m_navigationUp = target;
		break;
	case Direction::Down:
		m_navigationDown = target;
		break;
	case Direction::Left:
		m_navigationLeft = target;
		break;
	case Direction::Right:
		m_navigationRight = target;
		break;
	}
}

Interactable* Navigation::findNavigationTarget(
	Interactable* current,
	Direction direction,
	UIElement* root
) const
{
	if (!current || !root)
		return nullptr;

	// Get current element's center position
	Vector2 currentPos = current->getWorldPosition() + current->getSize() * 0.5f;

	// Find all interactable elements
	Interactable* bestTarget = nullptr;
	float bestScore = std::numeric_limits<float>::max();

	// Recursively search for candidates
	std::function<void(UIElement*)> searchElements = [&](UIElement* element)
	{
		// Check if this element is an interactable
		Interactable* interactable = dynamic_type_cast<Interactable*>(element);
		if (interactable && interactable != current &&
			interactable->isEnabled() && interactable->isVisible() &&
			interactable->isInteractable())
		{
			Vector2 targetPos = interactable->getWorldPosition() + interactable->getSize() * 0.5f;
			float score = calculateNavigationScore(currentPos, targetPos, direction);

			if (score >= 0.0f && score < bestScore)
			{
				bestScore = score;
				bestTarget = interactable;
			}
		}

		// Recurse through children
		const auto& children = element->getChildren();
		for (auto child : children)
		{
			if (child->isEnabled() && child->isVisible())
				searchElements(child);
		}
	};

	searchElements(root);
	return bestTarget;
}

Interactable* Navigation::navigate(
	Interactable* current,
	Direction direction,
	UIElement* root
) const
{
	if (m_mode == Mode::None)
		return nullptr;

	if (m_mode == Mode::Explicit)
	{
		return getNavigationTarget(direction);
	}
	else if (m_mode == Mode::Automatic)
	{
		return findNavigationTarget(current, direction, root);
	}

	return nullptr;
}

float Navigation::calculateNavigationScore(
	const Vector2& currentPos,
	const Vector2& targetPos,
	Direction direction
)
{
	Vector2 delta = targetPos - currentPos;

	// Check if target is in the correct direction
	switch (direction)
	{
	case Direction::Up:
		if (delta.y >= 0.0f)
			return -1.0f; // Wrong direction
		break;

	case Direction::Down:
		if (delta.y <= 0.0f)
			return -1.0f;
		break;

	case Direction::Left:
		if (delta.x >= 0.0f)
			return -1.0f;
		break;

	case Direction::Right:
		if (delta.x <= 0.0f)
			return -1.0f;
		break;
	}

	// Calculate score based on distance
	// Lower score = better candidate
	// Prioritize primary axis distance, with secondary axis as tiebreaker
	float primaryDistance = 0.0f;
	float secondaryDistance = 0.0f;

	switch (direction)
	{
	case Direction::Up:
	case Direction::Down:
		primaryDistance = std::abs(delta.y);
		secondaryDistance = std::abs(delta.x) * 0.5f; // Secondary axis weighted less
		break;

	case Direction::Left:
	case Direction::Right:
		primaryDistance = std::abs(delta.x);
		secondaryDistance = std::abs(delta.y) * 0.5f;
		break;
	}

	return primaryDistance + secondaryDistance;
}

}
