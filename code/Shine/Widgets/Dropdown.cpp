/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/Dropdown.h"
#include "Shine/Widgets/DropdownOption.h"
#include "Shine/Widgets/Button.h"
#include "Shine/Widgets/Image.h"
#include "Shine/Widgets/Text.h"
#include "Shine/Widgets/ScrollView.h"
#include "Shine/Widgets/Panel.h"
#include "Shine/Widgets/VerticalLayout.h"
#include "Shine/RenderContext.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Dropdown", Dropdown, Interactable)

Dropdown::Dropdown(const std::wstring& name)
:	Interactable(name)
{
	createDefaultWidgets();
}

Dropdown::~Dropdown()
{
}

void Dropdown::addOption(const std::wstring& text)
{
	m_options.push_back(text);
	createOptionWidgets();
}

void Dropdown::removeOption(int index)
{
	if (index >= 0 && index < (int)m_options.size())
	{
		m_options.erase(m_options.begin() + index);

		// Adjust selected index
		if (m_selectedIndex == index)
			m_selectedIndex = -1;
		else if (m_selectedIndex > index)
			m_selectedIndex--;

		createOptionWidgets();
	}
}

void Dropdown::clearOptions()
{
	m_options.clear();
	m_selectedIndex = -1;
	createOptionWidgets();
	updateSelectedText();
}

const std::wstring& Dropdown::getOptionText(int index) const
{
	if (index >= 0 && index < (int)m_options.size())
		return m_options[index];

	static std::wstring empty;
	return empty;
}

void Dropdown::setSelectedIndex(int index)
{
	if (index >= -1 && index < (int)m_options.size() && m_selectedIndex != index)
	{
		m_selectedIndex = index;
		updateSelectedText();

		if (onSelectionChanged && index >= 0)
			onSelectionChanged(index, m_options[index]);
	}
}

const std::wstring& Dropdown::getSelectedText() const
{
	return getOptionText(m_selectedIndex);
}

void Dropdown::open()
{
	if (!m_isOpen && m_dropdownPanel)
	{
		m_isOpen = true;
		m_dropdownPanel->setVisible(true);
	}
}

void Dropdown::close()
{
	if (m_isOpen && m_dropdownPanel)
	{
		m_isOpen = false;
		m_dropdownPanel->setVisible(false);
	}
}

void Dropdown::toggle()
{
	if (m_isOpen)
		close();
	else
		open();
}

bool Dropdown::onMouseButtonDown(MouseButton button, const Vector2& position)
{
	if (!Interactable::onMouseButtonDown(button, position))
		return false;

	// Check if clicking outside dropdown panel to close it
	if (m_isOpen && m_dropdownPanel)
	{
		if (!m_dropdownPanel->hitTest(position) && !m_button->hitTest(position))
		{
			close();
			return true;
		}
	}

	return false;
}

void Dropdown::update(float deltaTime)
{
	Interactable::update(deltaTime);
}

void Dropdown::render(RenderContext* rc, float parentOpacity)
{
	// Let base class handle rendering (will render children)
	Interactable::render(rc, parentOpacity);
}

void Dropdown::createDefaultWidgets()
{
	Vector2 size = getSize();
	if (size.x < 20) size.x = 200;
	if (size.y < 20) size.y = 30;
	setSize(size);

	// Create button
	Ref<Button> button = new Button(L"DropdownButton");
	button->setPosition(Vector2(0, 0));
	button->setSize(size);
	button->onClick = [this]() { toggle(); };
	addChild(button);
	m_button = button;

	// Button anchoring
	button->setAnchorMin(Vector2(0, 0));
	button->setAnchorMax(Vector2(1, 0));
	button->setOffsetMin(Vector2(0, 0));
	button->setOffsetMax(Vector2(0, size.y));

	// Get button's text widget
	m_selectedText = button->getTextWidget();
	if (m_selectedText)
		m_selectedText->setText(L"Select...");

	// Create arrow indicator (child of button)
	Ref<Image> arrow = new Image(L"DropdownArrow");
	arrow->setPosition(Vector2(size.x - 20, 5));
	arrow->setSize(Vector2(15, 15));
	arrow->setColor(Color4f(1, 1, 1, 1));
	// TODO: Set arrow texture
	button->addChild(arrow);
	m_arrow = arrow;

	// Create dropdown panel (initially hidden)
	Ref<ScrollView> panel = new ScrollView(L"DropdownPanel");
	panel->setPosition(Vector2(0, size.y));
	panel->setSize(Vector2(size.x, 150));
	panel->setVisible(false);
	panel->setScrollMode(ScrollView::ScrollMode::Vertical);
	panel->setScrollbarVisibility(ScrollView::ScrollbarVisibility::AutoHide);
	addChild(panel);
	m_dropdownPanel = panel;

	updateSelectedText();
}

void Dropdown::updateSelectedText()
{
	if (m_selectedText)
	{
		if (m_selectedIndex >= 0 && m_selectedIndex < (int)m_options.size())
			m_selectedText->setText(m_options[m_selectedIndex]);
		else
			m_selectedText->setText(L"Select...");
	}
}

void Dropdown::createOptionWidgets()
{
	if (!m_dropdownPanel)
		return;

	// Clear existing options
	UIElement* contentArea = m_dropdownPanel->getContentArea();
	if (!contentArea)
		return;

	// Remove old option widgets
	for (DropdownOption* option : m_optionWidgets)
	{
		contentArea->removeChild(option);
	}
	m_optionWidgets.clear();

	// Create vertical layout for options
	Ref<VerticalLayout> layout = new VerticalLayout(L"OptionsLayout");
	layout->setPosition(Vector2(0, 0));
	layout->setSpacing(2);
	contentArea->addChild(layout);

	// Create option widgets
	for (size_t i = 0; i < m_options.size(); ++i)
	{
		Ref<DropdownOption> option = new DropdownOption(L"Option");
		option->setText(m_options[i]);
		option->setSize(Vector2(getSize().x - 20, 25));
		option->setDropdown(this);
		option->setOptionIndex((int)i);
		layout->addChild(option);
		m_optionWidgets.push_back(option);
	}

	// Update content size
	float contentHeight = m_options.size() * 27.0f; // 25 + 2 spacing
	m_dropdownPanel->setContentSize(Vector2(getSize().x - 20, contentHeight));
}

void Dropdown::onOptionSelected(int index)
{
	setSelectedIndex(index);
	close();
}

}
