/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Canvas.h"
#include "Shine/RenderContext.h"
#include "Shine/Interactable.h"
#include "Shine/Visual.h"
#include "Shine/Font/FontManager.h"
#include "Shine/Widgets/Text.h"
#include "Shine/IRenderGraph.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Canvas", Canvas, Layout)

Canvas::Canvas(IRenderGraph* renderGraph, float width, float height, const std::wstring& name)
:	Layout(name)
,	m_renderGraph(renderGraph)
{
	// Create render context
	m_renderContext = new RenderContext(renderGraph, width, height);

	// Set canvas size
	setSize(Vector2(width, height));

	// Canvas is always at origin
	setPosition(Vector2(0.0f, 0.0f));
}

Canvas::~Canvas()
{
}

void Canvas::update(float deltaTime)
{
	// Update layout first
	Layout::update(deltaTime);
}

void Canvas::render()
{
	if (!m_renderContext)
		return;

	// Begin rendering frame
	m_renderContext->beginFrame();

	// Render all children recursively
	const AlignedVector<Ref<UIElement>>& children = getChildren();
	for (const auto& child : children)
	{
		if (!child)
			continue;

		Visual* visualChild = dynamic_type_cast<Visual*>(child.ptr());
		if (visualChild && visualChild->isVisibleInHierarchy())
		{
			visualChild->renderHierarchy(m_renderContext);
		}
	}

	// End rendering frame (submits all draw calls)
	m_renderContext->endFrame();
}

void Canvas::setViewportSize(float width, float height)
{
	// Update canvas size
	setSize(Vector2(width, height));

	// TODO: Update render context viewport size
	// This requires adding a method to RenderContext
}

bool Canvas::handleMouseButtonDown(MouseButton button, const Vector2& position)
{
	if (!m_interactive)
		return false;

	// Find interactable element at position
	Interactable* element = findInteractableAt(position);
	if (element)
	{
		return element->onMouseButtonDown(button, position);
	}

	return false;
}

bool Canvas::handleMouseButtonUp(MouseButton button, const Vector2& position)
{
	if (!m_interactive)
		return false;

	// Find interactable element at position
	Interactable* element = findInteractableAt(position);
	if (element)
	{
		return element->onMouseButtonUp(button, position);
	}

	return false;
}

bool Canvas::handleMouseMove(const Vector2& position)
{
	if (!m_interactive)
		return false;

	// Find element at position
	Interactable* element = findInteractableAt(position);

	// Handle hover state changes
	if (element != m_hoveredElement)
	{
		if (m_hoveredElement)
			m_hoveredElement->onMouseLeave();

		m_hoveredElement = element;

		if (m_hoveredElement)
			m_hoveredElement->onMouseEnter();
	}

	// Route move event
	if (element)
	{
		return element->onMouseMove(position);
	}

	return false;
}

bool Canvas::handleMouseWheel(float delta)
{
	if (!m_interactive)
		return false;

	// Route to focused element first
	if (m_focusedElement)
	{
		if (m_focusedElement->onMouseWheel(delta))
			return true;
	}

	// Otherwise route to hovered element
	if (m_hoveredElement)
	{
		return m_hoveredElement->onMouseWheel(delta);
	}

	return false;
}

bool Canvas::handleKeyDown(int key)
{
	if (!m_interactive)
		return false;

	// Route to focused element
	if (m_focusedElement)
	{
		return m_focusedElement->onKeyDown(key);
	}

	return false;
}

bool Canvas::handleKeyUp(int key)
{
	if (!m_interactive)
		return false;

	// Route to focused element
	if (m_focusedElement)
	{
		return m_focusedElement->onKeyUp(key);
	}

	return false;
}

bool Canvas::handleTextInput(wchar_t character)
{
	if (!m_interactive)
		return false;

	// Route to focused element
	if (m_focusedElement)
	{
		return m_focusedElement->onTextInput(character);
	}

	return false;
}

void Canvas::setFocusedElement(Interactable* element)
{
	if (m_focusedElement == element)
		return;

	// Remove focus from previous element
	if (m_focusedElement)
		m_focusedElement->unfocus();

	m_focusedElement = element;

	// Set focus on new element
	if (m_focusedElement)
		m_focusedElement->focus();
}

void Canvas::clearFocus()
{
	setFocusedElement(nullptr);
}

Interactable* Canvas::findInteractableAt(const Vector2& position)
{
	// Recursively search children (from back to front for proper Z-order)
	const AlignedVector<Ref<UIElement>>& children = getChildren();
	for (auto it = children.rbegin(); it != children.rend(); ++it)
	{
		const Ref<UIElement>& child = *it;
		if (!child)
			continue;

		Interactable* result = findInteractableAtRecursive(child.ptr(), position);
		if (result)
			return result;
	}

	return nullptr;
}

Interactable* Canvas::findInteractableAtRecursive(UIElement* element, const Vector2& position)
{
	if (!element || !element->isVisibleInHierarchy())
		return nullptr;

	// Check children first (reverse order for Z-ordering)
	const AlignedVector<Ref<UIElement>>& children = element->getChildren();
	for (auto it = children.rbegin(); it != children.rend(); ++it)
	{
		const Ref<UIElement>& child = *it;
		if (!child)
			continue;

		Interactable* result = findInteractableAtRecursive(child.ptr(), position);
		if (result)
			return result;
	}

	// Check this element if it's interactable
	Interactable* interactable = dynamic_type_cast<Interactable*>(element);
	if (interactable && interactable->isInteractable() && interactable->hitTest(position))
	{
		return interactable;
	}

	return nullptr;
}

void Canvas::setFontManager(FontManager* manager)
{
	m_fontManager = manager;

	// Propagate to all Text widgets recursively
	std::function<void(UIElement*)> propagate = [&](UIElement* element)
	{
		if (Text* text = dynamic_type_cast<Text*>(element))
		{
			text->setFontManager(manager);
		}

		// Recurse to children
		for (auto& child : element->getChildren())
		{
			if (child)
				propagate(child.ptr());
		}
	};

	propagate(this);
}

}
