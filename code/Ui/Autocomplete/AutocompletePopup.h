/*
 * TRAKTOR
 * Copyright (c) 2022-2025 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <vector>
#include "Core/RefArray.h"
#include "Ui/Form.h"
#include "Ui/Events/KeyDownEvent.h"
#include "Ui/Events/MouseButtonDownEvent.h"
#include "Ui/Events/MouseWheelEvent.h"
#include "Ui/Autocomplete/IAutocompleteProvider.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_UI_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::ui
{

class Canvas;
class ListBox;
class ScrollBar;

/*! Autocomplete selection event.
 * \ingroup UI
 */
class T_DLLCLASS AutocompleteSelectEvent : public Event
{
	T_RTTI_CLASS;

public:
	AutocompleteSelectEvent(EventSubject* sender, const AutocompleteSuggestion& suggestion)
		: Event(sender), m_suggestion(suggestion) {}

	const AutocompleteSuggestion& getSuggestion() const { return m_suggestion; }

private:
	AutocompleteSuggestion m_suggestion;
};

/*! Autocomplete popup window.
 * \ingroup UI
 */
class T_DLLCLASS AutocompletePopup : public Form
{
	T_RTTI_CLASS;

public:
	AutocompletePopup();

	bool create(Widget* parent, IAutocompleteProvider* provider);

	/*! Show popup with suggestions at given screen position. */
	void showSuggestions(const Point& position, const std::vector< AutocompleteSuggestion >& suggestions);

	/*! Hide popup. */
	void hide();

	/*! Navigate selection up/down. */
	void selectPrevious();
	void selectNext();

	/*! Get currently selected suggestion. */
	const AutocompleteSuggestion* getSelectedSuggestion() const;

	/*! Accept current selection and hide popup. */
	void acceptSelection();

	/*! Filter suggestions based on new prefix. */
	void filterSuggestions(const std::wstring& prefix);

private:
	Ref< IAutocompleteProvider > m_provider;
	Ref< ListBox > m_listBox;
	std::vector< AutocompleteSuggestion > m_allSuggestions;
	std::vector< int32_t > m_filteredIndices;
	int32_t m_selectedIndex = -1;

	void updateDisplay();

	bool matchesPrefix(const std::wstring& text, const std::wstring& prefix) const;

	void eventKeyDown(KeyDownEvent* event);

	void eventMouseDown(MouseButtonDownEvent* event);

	void eventMouseWheel(MouseWheelEvent* event);

	void eventListSelect(SelectionChangeEvent* event);

	void eventListDoubleClick(MouseDoubleClickEvent* event);
};

}