/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Object.h"
#include "Core/Ref.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

class Interactable;
class UIElement;

/*!
 * \brief Navigation system for keyboard/gamepad UI traversal.
 *
 * Navigation manages focus movement between UI elements using keyboard
 * or gamepad input. Supports automatic navigation (spatial) or explicit
 * navigation targets.
 *
 * Usage:
 *   Ref<Navigation> nav = new Navigation();
 *   nav->setNavigationMode(Navigation::Mode::Explicit);
 *   nav->setNavigationUp(upButton);
 *   nav->setNavigationDown(downButton);
 *   nav->setNavigationLeft(leftButton);
 *   nav->setNavigationRight(rightButton);
 *
 *   // Or use automatic mode
 *   nav->setNavigationMode(Navigation::Mode::Automatic);
 *   nav->findNavigationTargets(canvas);
 */
class T_DLLCLASS Navigation : public Object
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Navigation mode
	 */
	enum class Mode
	{
		None,       // No navigation
		Automatic,  // Find nearest element in direction
		Explicit    // Use explicitly set targets
	};

	/*!
	 * \brief Navigation direction
	 */
	enum class Direction
	{
		Up,
		Down,
		Left,
		Right
	};

	/*!
	 * \brief Constructor
	 */
	Navigation();

	/*!
	 * \brief Destructor
	 */
	virtual ~Navigation();

	// === Mode Configuration ===

	/*!
	 * \brief Get navigation mode
	 */
	Mode getNavigationMode() const { return m_mode; }

	/*!
	 * \brief Set navigation mode
	 */
	void setNavigationMode(Mode mode) { m_mode = mode; }

	// === Explicit Navigation Targets ===

	/*!
	 * \brief Get navigation target in direction
	 */
	Interactable* getNavigationTarget(Direction direction) const;

	/*!
	 * \brief Set navigation target
	 */
	void setNavigationTarget(Direction direction, Interactable* target);

	/*!
	 * \brief Get navigation up target
	 */
	Interactable* getNavigationUp() const { return m_navigationUp; }

	/*!
	 * \brief Set navigation up target
	 */
	void setNavigationUp(Interactable* target) { m_navigationUp = target; }

	/*!
	 * \brief Get navigation down target
	 */
	Interactable* getNavigationDown() const { return m_navigationDown; }

	/*!
	 * \brief Set navigation down target
	 */
	void setNavigationDown(Interactable* target) { m_navigationDown = target; }

	/*!
	 * \brief Get navigation left target
	 */
	Interactable* getNavigationLeft() const { return m_navigationLeft; }

	/*!
	 * \brief Set navigation left target
	 */
	void setNavigationLeft(Interactable* target) { m_navigationLeft = target; }

	/*!
	 * \brief Get navigation right target
	 */
	Interactable* getNavigationRight() const { return m_navigationRight; }

	/*!
	 * \brief Set navigation right target
	 */
	void setNavigationRight(Interactable* target) { m_navigationRight = target; }

	// === Automatic Navigation ===

	/*!
	 * \brief Find navigation target automatically
	 * \param current Current focused element
	 * \param direction Direction to navigate
	 * \param root Root element to search within
	 * \return Best navigation target, or nullptr if none found
	 */
	Interactable* findNavigationTarget(
		Interactable* current,
		Direction direction,
		UIElement* root
	) const;

	// === Wrapping ===

	/*!
	 * \brief Get whether navigation wraps
	 */
	bool getWrapNavigation() const { return m_wrapNavigation; }

	/*!
	 * \brief Set whether navigation wraps
	 */
	void setWrapNavigation(bool wrap) { m_wrapNavigation = wrap; }

	// === Helper ===

	/*!
	 * \brief Navigate from current element in direction
	 * \param current Current focused element
	 * \param direction Direction to navigate
	 * \param root Root element for automatic mode
	 * \return Navigation target, or nullptr if none
	 */
	Interactable* navigate(
		Interactable* current,
		Direction direction,
		UIElement* root = nullptr
	) const;

private:
	Mode m_mode = Mode::Automatic;
	bool m_wrapNavigation = false;

	// Explicit targets
	Ref<Interactable> m_navigationUp;
	Ref<Interactable> m_navigationDown;
	Ref<Interactable> m_navigationLeft;
	Ref<Interactable> m_navigationRight;

	// Helper methods
	static float calculateNavigationScore(
		const Vector2& currentPos,
		const Vector2& targetPos,
		Direction direction
	);
};

}
