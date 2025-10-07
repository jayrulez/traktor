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
#include "Core/Containers/AlignedVector.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Aabb2.h"
#include <functional>
#include <string>

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

class Canvas;

/*!
 * \brief Base class for all UI elements in the inheritance-based UI framework.
 *
 * UIElement provides the core functionality for all UI elements:
 * - Hierarchy management (parent/children)
 * - Transform properties (position, size, scale, rotation, pivot)
 * - Anchoring and offsetting for responsive layouts
 * - Visibility and enabled state
 * - Lifecycle management (update, destroy)
 * - Event callbacks
 *
 * This is an abstract base class. Use derived classes like Visual, Layout, etc.
 */
class T_DLLCLASS UIElement : public Object
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for the element (useful for debugging)
	 */
	explicit UIElement(const std::wstring& name = L"");

	/*!
	 * \brief Destructor - triggers onDestroy callback
	 */
	virtual ~UIElement();

	// === Lifecycle ===

	/*!
	 * \brief Update the element (called every frame)
	 * \param deltaTime Time since last frame in seconds
	 *
	 * Override this to add per-frame logic. Remember to call base class!
	 */
	virtual void update(float deltaTime);

	/*!
	 * \brief Destroy the element and remove from parent
	 *
	 * Triggers onDestroy callback. Recursively destroys all children.
	 */
	virtual void destroy();

	/*!
	 * \brief Check if element has been destroyed
	 */
	bool isDestroyed() const { return m_destroyed; }

	// === Hierarchy ===

	/*!
	 * \brief Get parent element
	 */
	UIElement* getParent() const { return m_parent; }

	/*!
	 * \brief Get root canvas (traverses up to root)
	 */
	Canvas* getCanvas() const;

	/*!
	 * \brief Add child element
	 * \param child Child to add (takes ownership)
	 *
	 * Sets child's parent to this element.
	 */
	void addChild(UIElement* child);

	/*!
	 * \brief Remove child element
	 * \param child Child to remove
	 */
	void removeChild(UIElement* child);

	/*!
	 * \brief Remove child at index
	 */
	void removeChildAt(int index);

	/*!
	 * \brief Get number of children
	 */
	int getChildCount() const { return (int)m_children.size(); }

	/*!
	 * \brief Get child at index
	 */
	UIElement* getChild(int index) const;

	/*!
	 * \brief Get all children
	 */
	const AlignedVector<Ref<UIElement>>& getChildren() const { return m_children; }

	/*!
	 * \brief Find child by name (non-recursive)
	 */
	UIElement* findChild(const std::wstring& name) const;

	/*!
	 * \brief Find descendant by name (recursive)
	 */
	UIElement* findDescendant(const std::wstring& name) const;

	// === Transform - Local Space ===

	/*!
	 * \brief Get local position (relative to parent)
	 */
	Vector2 getPosition() const { return m_position; }

	/*!
	 * \brief Set local position
	 */
	void setPosition(const Vector2& position);

	/*!
	 * \brief Get size in pixels
	 */
	Vector2 getSize() const { return m_size; }

	/*!
	 * \brief Set size in pixels
	 */
	void setSize(const Vector2& size);

	/*!
	 * \brief Get scale factor
	 */
	Vector2 getScale() const { return m_scale; }

	/*!
	 * \brief Set scale factor
	 */
	void setScale(const Vector2& scale);

	/*!
	 * \brief Get rotation in degrees (counter-clockwise)
	 */
	float getRotation() const { return m_rotation; }

	/*!
	 * \brief Set rotation in degrees
	 */
	void setRotation(float rotation);

	/*!
	 * \brief Get pivot point (0,0 = top-left, 0.5,0.5 = center, 1,1 = bottom-right)
	 */
	Vector2 getPivot() const { return m_pivot; }

	/*!
	 * \brief Set pivot point
	 */
	void setPivot(const Vector2& pivot);

	// === Transform - World Space ===

	/*!
	 * \brief Get world position (absolute canvas coordinates)
	 */
	Vector2 getWorldPosition() const;

	/*!
	 * \brief Get world scale (accumulated from parents)
	 */
	Vector2 getWorldScale() const;

	/*!
	 * \brief Get world rotation (accumulated from parents)
	 */
	float getWorldRotation() const;

	/*!
	 * \brief Get axis-aligned bounding box in world space
	 */
	Aabb2 getWorldBounds() const;

	// === Anchoring ===

	/*!
	 * \brief Get anchor min (0,0 = parent's top-left)
	 */
	Vector2 getAnchorMin() const { return m_anchorMin; }

	/*!
	 * \brief Set anchor min
	 */
	void setAnchorMin(const Vector2& anchorMin);

	/*!
	 * \brief Get anchor max (1,1 = parent's bottom-right)
	 */
	Vector2 getAnchorMax() const { return m_anchorMax; }

	/*!
	 * \brief Set anchor max
	 */
	void setAnchorMax(const Vector2& anchorMax);

	/*!
	 * \brief Get offset min (pixels from anchor min)
	 */
	Vector2 getOffsetMin() const { return m_offsetMin; }

	/*!
	 * \brief Set offset min
	 */
	void setOffsetMin(const Vector2& offsetMin);

	/*!
	 * \brief Get offset max (pixels from anchor max)
	 */
	Vector2 getOffsetMax() const { return m_offsetMax; }

	/*!
	 * \brief Set offset max
	 */
	void setOffsetMax(const Vector2& offsetMax);

	/*!
	 * \brief Apply anchoring to compute actual position and size based on parent
	 *
	 * Called automatically when parent changes or parent size changes.
	 */
	void applyAnchors();

	// === State ===

	/*!
	 * \brief Get visibility
	 */
	bool isVisible() const { return m_visible; }

	/*!
	 * \brief Set visibility (affects rendering and input)
	 */
	void setVisible(bool visible);

	/*!
	 * \brief Check if element is visible in hierarchy (all parents visible)
	 */
	bool isVisibleInHierarchy() const;

	/*!
	 * \brief Get enabled state
	 */
	bool isEnabled() const { return m_enabled; }

	/*!
	 * \brief Set enabled state (affects input handling)
	 */
	void setEnabled(bool enabled);

	/*!
	 * \brief Check if element is enabled in hierarchy (all parents enabled)
	 */
	bool isEnabledInHierarchy() const;

	// === Identification ===

	/*!
	 * \brief Get element name
	 */
	const std::wstring& getName() const { return m_name; }

	/*!
	 * \brief Set element name
	 */
	void setName(const std::wstring& name) { m_name = name; }

	// === Events ===

	std::function<void()> onDestroy;
	std::function<void(UIElement*)> onParentChanged;
	std::function<void(const Vector2&)> onPositionChanged;
	std::function<void(const Vector2&)> onSizeChanged;

protected:
	/*!
	 * \brief Set parent (called by addChild/removeChild)
	 */
	void setParent(UIElement* parent);

	/*!
	 * \brief Mark transform as dirty (needs recalculation)
	 */
	void markTransformDirty();

	/*!
	 * \brief Called when transform changes
	 */
	virtual void onTransformChanged();

	/*!
	 * \brief Called when size changes (useful for layouts)
	 */
	virtual void sizeChanged();

private:
	// Hierarchy
	UIElement* m_parent = nullptr;
	AlignedVector<Ref<UIElement>> m_children;

	// Transform - local space
	Vector2 m_position = Vector2(0.0f, 0.0f);
	Vector2 m_size = Vector2(100.0f, 100.0f);
	Vector2 m_scale = Vector2(1.0f, 1.0f);
	float m_rotation = 0.0f;
	Vector2 m_pivot = Vector2(0.5f, 0.5f);

	// Anchoring
	Vector2 m_anchorMin = Vector2(0.0f, 0.0f);
	Vector2 m_anchorMax = Vector2(0.0f, 0.0f);
	Vector2 m_offsetMin = Vector2(0.0f, 0.0f);
	Vector2 m_offsetMax = Vector2(0.0f, 0.0f);

	// State
	bool m_visible = true;
	bool m_enabled = true;
	bool m_destroyed = false;

	// Identification
	std::wstring m_name;

	// Cached transform (for performance)
	mutable bool m_worldTransformDirty = true;
	mutable Vector2 m_cachedWorldPosition;
	mutable Vector2 m_cachedWorldScale;
	mutable float m_cachedWorldRotation;
};

}
