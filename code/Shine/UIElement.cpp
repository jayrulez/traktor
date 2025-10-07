/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/UIElement.h"
#include "Shine/Canvas.h"
#include "Core/Math/MathUtils.h"
#include <algorithm>

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.UIElement", UIElement, Object)

UIElement::UIElement(const std::wstring& name)
:	m_name(name)
{
}

UIElement::~UIElement()
{
	if (!m_destroyed && onDestroy)
		onDestroy();
}

void UIElement::update(float deltaTime)
{
	// Update all children
	for (auto& child : m_children)
	{
		if (child && !child->isDestroyed())
			child->update(deltaTime);
	}
}

void UIElement::destroy()
{
	if (m_destroyed)
		return;

	m_destroyed = true;

	// Destroy all children first
	for (auto& child : m_children)
	{
		if (child)
			child->destroy();
	}
	m_children.clear();

	// Remove from parent
	if (m_parent)
		m_parent->removeChild(this);

	// Trigger callback
	if (onDestroy)
		onDestroy();
}

Canvas* UIElement::getCanvas() const
{
	// Traverse up to find root canvas
	UIElement* current = const_cast<UIElement*>(this);
	while (current)
	{
		Canvas* canvas = dynamic_type_cast<Canvas*>(current);
		if (canvas)
			return canvas;
		current = current->m_parent;
	}
	return nullptr;
}

void UIElement::addChild(UIElement* child)
{
	if (!child)
		return;

	// Remove from previous parent
	if (child->m_parent)
		child->m_parent->removeChild(child);

	// Add to this element
	m_children.push_back(child);
	child->setParent(this);
	child->applyAnchors();
}

void UIElement::removeChild(UIElement* child)
{
	if (!child)
		return;

	auto it = std::find_if(m_children.begin(), m_children.end(),
		[child](const Ref<UIElement>& elem) { return elem.ptr() == child; });

	if (it != m_children.end())
	{
		(*it)->setParent(nullptr);
		m_children.erase(it);
	}
}

void UIElement::removeChildAt(int index)
{
	if (index >= 0 && index < (int)m_children.size())
	{
		m_children[index]->setParent(nullptr);
		m_children.erase(m_children.begin() + index);
	}
}

UIElement* UIElement::getChild(int index) const
{
	if (index >= 0 && index < (int)m_children.size())
		return m_children[index];
	return nullptr;
}

UIElement* UIElement::findChild(const std::wstring& name) const
{
	for (const auto& child : m_children)
	{
		if (child && child->getName() == name)
			return child;
	}
	return nullptr;
}

UIElement* UIElement::findDescendant(const std::wstring& name) const
{
	// Check direct children first
	UIElement* result = findChild(name);
	if (result)
		return result;

	// Recursively check descendants
	for (const auto& child : m_children)
	{
		if (child)
		{
			result = child->findDescendant(name);
			if (result)
				return result;
		}
	}

	return nullptr;
}

void UIElement::setPosition(const Vector2& position)
{
	if (m_position != position)
	{
		m_position = position;
		markTransformDirty();
		onTransformChanged();

		if (onPositionChanged)
			onPositionChanged(position);
	}
}

void UIElement::setSize(const Vector2& size)
{
	if (m_size != size)
	{
		m_size = size;
		markTransformDirty();
		sizeChanged();

		if (onSizeChanged)
			onSizeChanged(size);

		// Update children anchors
		for (auto& child : m_children)
		{
			if (child)
				child->applyAnchors();
		}
	}
}

void UIElement::setScale(const Vector2& scale)
{
	if (m_scale != scale)
	{
		m_scale = scale;
		markTransformDirty();
		onTransformChanged();
	}
}

void UIElement::setRotation(float rotation)
{
	if (m_rotation != rotation)
	{
		m_rotation = rotation;
		markTransformDirty();
		onTransformChanged();
	}
}

void UIElement::setPivot(const Vector2& pivot)
{
	if (m_pivot != pivot)
	{
		m_pivot = pivot;
		markTransformDirty();
		onTransformChanged();
	}
}

Vector2 UIElement::getWorldPosition() const
{
	if (m_worldTransformDirty)
	{
		if (m_parent)
		{
			Vector2 parentPos = m_parent->getWorldPosition();
			Vector2 parentScale = m_parent->getWorldScale();
			float parentRot = m_parent->getWorldRotation();

			// Apply parent rotation and scale to local position
			Vector2 localPos = m_position;
			if (parentRot != 0.0f)
			{
				float rad = deg2rad(parentRot);
				float cosA = std::cos(rad);
				float sinA = std::sin(rad);
				Vector2 rotated(
					localPos.x * cosA - localPos.y * sinA,
					localPos.x * sinA + localPos.y * cosA
				);
				localPos = rotated;
			}

			m_cachedWorldPosition = parentPos + localPos * parentScale;
		}
		else
		{
			m_cachedWorldPosition = m_position;
		}
	}

	return m_cachedWorldPosition;
}

Vector2 UIElement::getWorldScale() const
{
	if (m_worldTransformDirty)
	{
		if (m_parent)
			m_cachedWorldScale = m_parent->getWorldScale() * m_scale;
		else
			m_cachedWorldScale = m_scale;
	}

	return m_cachedWorldScale;
}

float UIElement::getWorldRotation() const
{
	if (m_worldTransformDirty)
	{
		if (m_parent)
			m_cachedWorldRotation = m_parent->getWorldRotation() + m_rotation;
		else
			m_cachedWorldRotation = m_rotation;

		m_worldTransformDirty = false;
	}

	return m_cachedWorldRotation;
}

Aabb2 UIElement::getWorldBounds() const
{
	Vector2 worldPos = getWorldPosition();
	Vector2 worldScale = getWorldScale();
	Vector2 scaledSize = m_size * worldScale;

	// Simple AABB (doesn't account for rotation)
	Vector2 pivotOffset = m_pivot * scaledSize;
	Vector2 min = worldPos - pivotOffset;
	Vector2 max = min + scaledSize;

	return Aabb2(min, max);
}

void UIElement::setAnchorMin(const Vector2& anchorMin)
{
	if (m_anchorMin != anchorMin)
	{
		m_anchorMin = anchorMin;
		applyAnchors();
	}
}

void UIElement::setAnchorMax(const Vector2& anchorMax)
{
	if (m_anchorMax != anchorMax)
	{
		m_anchorMax = anchorMax;
		applyAnchors();
	}
}

void UIElement::setOffsetMin(const Vector2& offsetMin)
{
	if (m_offsetMin != offsetMin)
	{
		m_offsetMin = offsetMin;
		applyAnchors();
	}
}

void UIElement::setOffsetMax(const Vector2& offsetMax)
{
	if (m_offsetMax != offsetMax)
	{
		m_offsetMax = offsetMax;
		applyAnchors();
	}
}

void UIElement::applyAnchors()
{
	if (!m_parent)
		return;

	Vector2 parentSize = m_parent->getSize();

	// Calculate anchor points in parent space
	Vector2 anchorMinPos = parentSize * m_anchorMin;
	Vector2 anchorMaxPos = parentSize * m_anchorMax;

	// Apply offsets
	Vector2 min = anchorMinPos + m_offsetMin;
	Vector2 max = anchorMaxPos + m_offsetMax;

	// Compute position and size
	Vector2 newSize = max - min;
	Vector2 newPosition = min;

	// Update without triggering anchor recalculation
	if (m_size != newSize)
	{
		m_size = newSize;
		markTransformDirty();
		sizeChanged();

		if (onSizeChanged)
			onSizeChanged(newSize);
	}

	if (m_position != newPosition)
	{
		m_position = newPosition;
		markTransformDirty();
		onTransformChanged();

		if (onPositionChanged)
			onPositionChanged(newPosition);
	}
}

void UIElement::setVisible(bool visible)
{
	if (m_visible != visible)
	{
		m_visible = visible;
	}
}

bool UIElement::isVisibleInHierarchy() const
{
	if (!m_visible)
		return false;

	if (m_parent)
		return m_parent->isVisibleInHierarchy();

	return true;
}

void UIElement::setEnabled(bool enabled)
{
	if (m_enabled != enabled)
	{
		m_enabled = enabled;
	}
}

bool UIElement::isEnabledInHierarchy() const
{
	if (!m_enabled)
		return false;

	if (m_parent)
		return m_parent->isEnabledInHierarchy();

	return true;
}

void UIElement::setParent(UIElement* parent)
{
	if (m_parent != parent)
	{
		m_parent = parent;
		markTransformDirty();

		if (onParentChanged)
			onParentChanged(parent);
	}
}

void UIElement::markTransformDirty()
{
	m_worldTransformDirty = true;

	// Recursively mark children as dirty
	for (auto& child : m_children)
	{
		if (child)
			child->markTransformDirty();
	}
}

void UIElement::onTransformChanged()
{
	// Override in derived classes if needed
}

void UIElement::sizeChanged()
{
	// Override in derived classes if needed
}

}
