/*
 * TRAKTOR
 * Copyright (c) 2022-2025 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Misc/String.h"
#include "Ui/Application.h"
#include "Ui/Autocomplete/AutocompletePopup.h"
#include "Ui/Events/KeyDownEvent.h"
#include "Ui/Events/MouseButtonDownEvent.h"
#include "Ui/Events/MouseWheelEvent.h"
#include "Ui/ListBox/ListBox.h"
#include "Ui/TableLayout.h"

namespace traktor::ui
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.ui.AutocompleteSelectEvent", AutocompleteSelectEvent, Event)
T_IMPLEMENT_RTTI_CLASS(L"traktor.ui.AutocompletePopup", AutocompletePopup, Form)

AutocompletePopup::AutocompletePopup()
{
}

bool AutocompletePopup::create(Widget* parent, IAutocompleteProvider* provider)
{
	if (!Form::create(L"", Unit(600), Unit(400), WsTop, nullptr, parent))
		return false;

	m_provider = provider;

	setLayout(new TableLayout(L"100%", L"100%", 2_ut, 2_ut));

	m_listBox = new ListBox();
	if (!m_listBox->create(this, ListBox::WsSingle))
		return false;

	m_listBox->addEventHandler< SelectionChangeEvent >(this, &AutocompletePopup::eventListSelect);
	m_listBox->addEventHandler< MouseDoubleClickEvent >(this, &AutocompletePopup::eventListDoubleClick);

	addEventHandler< KeyDownEvent >(this, &AutocompletePopup::eventKeyDown);
	addEventHandler< MouseButtonDownEvent >(this, &AutocompletePopup::eventMouseDown);
	addEventHandler< MouseWheelEvent >(this, &AutocompletePopup::eventMouseWheel);

	return true;
}

void AutocompletePopup::showSuggestions(const Point& position, const std::vector< AutocompleteSuggestion >& suggestions)
{
	m_allSuggestions = suggestions;
	m_filteredIndices.clear();

	// Initially show all suggestions
	for (int32_t i = 0; i < (int32_t)suggestions.size(); ++i)
		m_filteredIndices.push_back(i);

	m_selectedIndex = m_filteredIndices.empty() ? -1 : 0;
	updateDisplay();

	if (!m_filteredIndices.empty())
	{
		// Calculate height with better minimum and padding
		const int32_t itemHeight = 22;
		const int32_t padding = 20; // More generous padding for borders/margins
		const int32_t minHeight = 50; // Minimum height even for single items
		const int32_t calculatedHeight = (int32_t)m_filteredIndices.size() * itemHeight + padding;
		const int32_t finalHeight = std::max(minHeight, std::min(400, calculatedHeight));

		setRect(Rect(position, Size(600, finalHeight)));
		show();
		// Don't steal focus - let the editor keep focus so typing continues to work
	}
	else
	{
		hide();
	}
}

void AutocompletePopup::hide()
{
	Form::hide();
	m_allSuggestions.clear();
	m_filteredIndices.clear();
	m_selectedIndex = -1;
}

void AutocompletePopup::selectPrevious()
{
	if (!m_filteredIndices.empty())
	{
		if (m_selectedIndex <= 0)
			m_selectedIndex = (int32_t)m_filteredIndices.size() - 1; // Wrap to last item
		else
			m_selectedIndex--;

		m_listBox->select(m_selectedIndex);
		m_listBox->update(); // Force update to show selection
	}
}

void AutocompletePopup::selectNext()
{
	if (!m_filteredIndices.empty())
	{
		if (m_selectedIndex >= (int32_t)m_filteredIndices.size() - 1)
			m_selectedIndex = 0; // Wrap to first item
		else
			m_selectedIndex++;

		m_listBox->select(m_selectedIndex);
		m_listBox->update(); // Force update to show selection
	}
}

const AutocompleteSuggestion* AutocompletePopup::getSelectedSuggestion() const
{
	if (m_selectedIndex >= 0 && m_selectedIndex < (int32_t)m_filteredIndices.size())
	{
		int32_t suggestionIndex = m_filteredIndices[m_selectedIndex];
		if (suggestionIndex >= 0 && suggestionIndex < (int32_t)m_allSuggestions.size())
			return &m_allSuggestions[suggestionIndex];
	}
	return nullptr;
}

void AutocompletePopup::acceptSelection()
{
	const AutocompleteSuggestion* suggestion = getSelectedSuggestion();
	if (suggestion)
	{
		AutocompleteSelectEvent selectEvent(this, *suggestion);
		raiseEvent(&selectEvent);
		hide();
	}
}

void AutocompletePopup::filterSuggestions(const std::wstring& prefix)
{
	m_filteredIndices.clear();

	if (prefix.empty())
	{
		// Show all suggestions if no prefix
		for (int32_t i = 0; i < (int32_t)m_allSuggestions.size(); ++i)
			m_filteredIndices.push_back(i);
	}
	else
	{
		// Filter suggestions that match prefix
		for (int32_t i = 0; i < (int32_t)m_allSuggestions.size(); ++i)
		{
			if (matchesPrefix(m_allSuggestions[i].name, prefix))
				m_filteredIndices.push_back(i);
		}
	}

	// Update selection
	if (m_filteredIndices.empty())
	{
		m_selectedIndex = -1;
		hide();
	}
	else
	{
		m_selectedIndex = 0;
		updateDisplay();
	}
}

void AutocompletePopup::updateDisplay()
{
	m_listBox->removeAll();

	for (int32_t filteredIndex : m_filteredIndices)
	{
		const AutocompleteSuggestion& suggestion = m_allSuggestions[filteredIndex];

		std::wstring displayText = suggestion.name;
		if (!suggestion.description.empty())
			displayText += L" - " + suggestion.description;

		m_listBox->add(displayText);
	}

	// Ensure we clear any existing selection state first
	m_listBox->deselectAll();

	// Then set the proper selection
	if (m_selectedIndex >= 0 && m_selectedIndex < (int32_t)m_filteredIndices.size())
		m_listBox->select(m_selectedIndex);

	m_listBox->update();
}

bool AutocompletePopup::matchesPrefix(const std::wstring& text, const std::wstring& prefix) const
{
	if (prefix.length() > text.length())
		return false;

	// Case-insensitive prefix match
	for (size_t i = 0; i < prefix.length(); ++i)
	{
		if (towlower(text[i]) != towlower(prefix[i]))
			return false;
	}

	return true;
}

void AutocompletePopup::eventKeyDown(KeyDownEvent* event)
{
	switch (event->getVirtualKey())
	{
	case VkUp:
		selectPrevious();
		event->consume();
		break;

	case VkDown:
		selectNext();
		event->consume();
		break;

	case VkReturn:
		acceptSelection();
		event->consume();
		break;

	case VkEscape:
		hide();
		event->consume();
		break;

	case VkTab:
		acceptSelection();
		event->consume();
		break;
	}
}

void AutocompletePopup::eventMouseDown(MouseButtonDownEvent* event)
{
	// Click outside popup should close it
	if (!getRect().inside(event->getPosition()))
	{
		hide();
		event->consume();
	}
}

void AutocompletePopup::eventListSelect(SelectionChangeEvent* event)
{
	m_selectedIndex = m_listBox->getSelected();
}

void AutocompletePopup::eventMouseWheel(MouseWheelEvent* event)
{
	// Consume scroll events to prevent them from propagating to the underlying editor
	event->consume();
}

void AutocompletePopup::eventListDoubleClick(MouseDoubleClickEvent* event)
{
	acceptSelection();
}

}