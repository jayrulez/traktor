/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <algorithm>
#include "Core/Serialization/ISerializer.h"
#include "Core/Serialization/Member.h"
#include "Core/Serialization/MemberRef.h"
#include "Paper/Ui/UIPage.h"
#include "Paper/Ui/UIElement.h"
#include "Paper/Ui/UITheme.h"
#include "Paper/Ui/UIContext.h"
#include "Paper/Ui/Controls/Border.h"
#include "Paper/Ui/Controls/TextBox.h"
#include "Paper/Ui/Controls/ScrollViewer.h"
#include "Paper/Ui/Layouts/Panel.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.UIPage", 0, UIPage, ISerializable)

void UIPage::updateLayout(UIContext* context)
{
	if (!m_root || !context)
		return;

	// Measure and arrange UI using page dimensions
	Vector2 availableSize((float)m_width, (float)m_height);
	m_root->measure(availableSize, context);
	m_root->arrange(Vector2::zero(), availableSize);
}

void UIPage::update(double deltaTime)
{
	if (!m_root)
		return;

	// Recursively update all elements
	updateElementRecursive(m_root, deltaTime);
}

void UIPage::updateElementRecursive(UIElement* element, double deltaTime)
{
	if (!element)
		return;

	// Check if this is a TextBox and call its update method
	if (TextBox* textBox = dynamic_type_cast<TextBox*>(element))
	{
		textBox->update(deltaTime);
	}

	// Recurse into children
	if (Border* border = dynamic_type_cast<Border*>(element))
	{
		if (border->getChild())
			updateElementRecursive(border->getChild(), deltaTime);
	}
	else if (Panel* panel = dynamic_type_cast<Panel*>(element))
	{
		for (auto child : panel->getChildren())
			updateElementRecursive(child, deltaTime);
	}
	else if (ScrollViewer* scrollViewer = dynamic_type_cast<ScrollViewer*>(element))
	{
		if (scrollViewer->getContent())
			updateElementRecursive(scrollViewer->getContent(), deltaTime);
	}
}

void UIPage::render(UIContext* context, bool debugVisualization)
{
	if (!m_root || !context)
		return;

	if (debugVisualization)
		m_root->renderDebug(context);
	else
		m_root->render(context);
}

void UIPage::handleMouseMove(const Vector2& position)
{
	if (!m_root)
		return;

	// If an element has captured the mouse, send all events to it
	UIElement* hitElement = m_capturedElement ? m_capturedElement : m_root->hitTest(position);

	// Build ancestor chain for new hovered element (includes the element itself)
	RefArray< UIElement > newAncestors;
	if (hitElement)
	{
		UIElement* current = hitElement;
		while (current)
		{
			newAncestors.push_back(current);
			current = current->getParent();
		}
	}

	// Find elements that need onMouseLeave (in old chain but not in new chain)
	for (auto oldElement : m_hoveredAncestors)
	{
		if (std::find(newAncestors.begin(), newAncestors.end(), oldElement) == newAncestors.end())
		{
			MouseEvent leaveEvent;
			leaveEvent.position = position;
			leaveEvent.button = MouseButton::Left;
			oldElement->onMouseLeave(leaveEvent);
		}
	}

	// Find elements that need onMouseEnter (in new chain but not in old chain)
	for (auto newElement : newAncestors)
	{
		if (std::find(m_hoveredAncestors.begin(), m_hoveredAncestors.end(), newElement) == m_hoveredAncestors.end())
		{
			MouseEvent enterEvent;
			enterEvent.position = position;
			enterEvent.button = MouseButton::Left;
			newElement->onMouseEnter(enterEvent);
		}
	}

	// Update tracked state
	m_hoveredElement = hitElement;
	m_hoveredAncestors = newAncestors;

	// Send mouse move event to currently hovered element (deepest only)
	if (m_hoveredElement)
	{
		MouseEvent moveEvent;
		moveEvent.position = position;
		moveEvent.button = MouseButton::Left;
		m_hoveredElement->onMouseMove(moveEvent);
	}
}

void UIPage::handleMouseDown(const Vector2& position, MouseButton button)
{
	if (!m_root)
		return;

	// Perform hit test to find element under mouse
	UIElement* hitElement = m_root->hitTest(position);

	// Set focus to the hit element (or clear focus if clicking empty space)
	setFocus(hitElement);

	// Remember which element was pressed for click detection
	m_pressedElement = hitElement;

	if (hitElement)
	{
		MouseEvent downEvent;
		downEvent.position = position;
		downEvent.button = button;
		downEvent.capture = false;
		hitElement->onMouseDown(downEvent);

		// If element requested mouse capture, grant it
		if (downEvent.capture)
			m_capturedElement = hitElement;
	}
}

void UIPage::handleMouseUp(const Vector2& position, MouseButton button)
{
	if (!m_root)
		return;

	// Perform hit test to find element under mouse
	UIElement* hitElement = m_root->hitTest(position);

	if (hitElement)
	{
		MouseEvent upEvent;
		upEvent.position = position;
		upEvent.button = button;
		hitElement->onMouseUp(upEvent);

		// For click detection, we need to check if the pressed element is in the ancestor chain
		// of the released element (this allows clicking on children to trigger parent callbacks)
		if (m_pressedElement && button == MouseButton::Left)
		{
			// Build ancestor chain for the hit element
			UIElement* current = hitElement;
			while (current)
			{
				// Check if this ancestor was the pressed element
				if (current == m_pressedElement)
				{
					// Walk up from the hit element and fire the first callback we find
					UIElement* callbackElement = hitElement;
					while (callbackElement)
					{
						const ClickCallback& callback = callbackElement->getClickCallback();
						if (callback)
						{
							MouseEvent clickEvent;
							clickEvent.position = position;
							clickEvent.button = button;
							callback(callbackElement, clickEvent);
							break; // Only fire the first callback found
						}
						callbackElement = callbackElement->getParent();
					}
					break;
				}
				current = current->getParent();
			}
		}
	}

	// Clear pressed element
	m_pressedElement = nullptr;

	// Release mouse capture
	m_capturedElement = nullptr;
}

UIElement* UIPage::findElementByName(const std::wstring& name)
{
	if (!m_root)
		return nullptr;
	return findElementByNameRecursive(m_root, name);
}

UIElement* UIPage::findElementByNameRecursive(UIElement* element, const std::wstring& name)
{
	if (!element)
		return nullptr;

	// Check if this element matches
	if (element->getName() == name)
		return element;

	// Check children - need to handle different container types
	// For Border
	if (Border* border = dynamic_type_cast<Border*>(element))
	{
		if (border->getChild())
		{
			UIElement* found = findElementByNameRecursive(border->getChild(), name);
			if (found)
				return found;
		}
	}

	// For Panel (and StackPanel)
	if (Panel* panel = dynamic_type_cast<Panel*>(element))
	{
		for (auto child : panel->getChildren())
		{
			UIElement* found = findElementByNameRecursive(child, name);
			if (found)
				return found;
		}
	}

	// For ScrollViewer
	if (ScrollViewer* scrollViewer = dynamic_type_cast<ScrollViewer*>(element))
	{
		if (scrollViewer->getContent())
		{
			UIElement* found = findElementByNameRecursive(scrollViewer->getContent(), name);
			if (found)
				return found;
		}
	}

	return nullptr;
}

void UIPage::handleKeyDown(wchar_t character, int virtualKey, bool shift, bool control, bool alt)
{
	// Only send keyboard events to the focused element
	if (!m_focusedElement)
		return;

	KeyEvent keyEvent;
	keyEvent.character = character;
	keyEvent.virtualKey = virtualKey;
	keyEvent.shift = shift;
	keyEvent.control = control;
	keyEvent.alt = alt;
	keyEvent.handled = false;

	m_focusedElement->onKeyDown(keyEvent);
}

void UIPage::handleKeyUp(int virtualKey)
{
	// Only send keyboard events to the focused element
	if (!m_focusedElement)
		return;

	KeyEvent keyEvent;
	keyEvent.character = 0;
	keyEvent.virtualKey = virtualKey;
	keyEvent.handled = false;

	m_focusedElement->onKeyUp(keyEvent);
}

void UIPage::handleMouseWheel(const Vector2& position, int32_t delta)
{
	if (!m_root)
		return;

	// Perform hit test to find element under mouse
	UIElement* hitElement = m_root->hitTest(position);

	if (hitElement)
	{
		MouseWheelEvent wheelEvent;
		wheelEvent.position = position;
		wheelEvent.delta = delta;
		wheelEvent.handled = false;

		// Send to hit element and bubble up until handled
		UIElement* current = hitElement;
		while (current && !wheelEvent.handled)
		{
			current->onMouseWheel(wheelEvent);
			current = current->getParent();
		}
	}
}

void UIPage::setFocus(UIElement* element)
{
	if (m_focusedElement == element)
		return;

	// Blur the previously focused element
	if (m_focusedElement)
		m_focusedElement->onBlur();

	// Focus the new element
	m_focusedElement = element;
	if (m_focusedElement)
		m_focusedElement->onFocus();
}

void UIPage::captureMouse(UIElement* element)
{
	m_capturedElement = element;
}

void UIPage::serialize(ISerializer& s)
{
	s >> MemberRef< UIElement >(L"root", m_root);
	s >> MemberRef< UITheme >(L"theme", m_theme);
	s >> Member< int32_t >(L"width", m_width);
	s >> Member< int32_t >(L"height", m_height);
}

}
