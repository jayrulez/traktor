/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Shine/Interactable.h"
#include <vector>

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

class Button;
class Image;
class Text;
class ScrollView;
class DropdownOption;

/*!
 * \brief Dropdown widget for selecting from a list.
 *
 * Dropdown displays a button that, when clicked, shows a list of options.
 *
 * Usage:
 *   Ref<Dropdown> dropdown = new Dropdown(L"QualityDropdown");
 *   dropdown->addOption(L"Low");
 *   dropdown->addOption(L"Medium");
 *   dropdown->addOption(L"High");
 *   dropdown->setSelectedIndex(1);
 *   dropdown->onSelectionChanged = [](int index, const std::wstring& value) {
 *       setQuality(index);
 *   };
 */
class T_DLLCLASS Dropdown : public Interactable
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit Dropdown(const std::wstring& name = L"Dropdown");

	/*!
	 * \brief Destructor
	 */
	virtual ~Dropdown();

	// === Options ===

	/*!
	 * \brief Add option
	 */
	void addOption(const std::wstring& text);

	/*!
	 * \brief Remove option at index
	 */
	void removeOption(int index);

	/*!
	 * \brief Clear all options
	 */
	void clearOptions();

	/*!
	 * \brief Get number of options
	 */
	int getOptionCount() const { return (int)m_options.size(); }

	/*!
	 * \brief Get option text at index
	 */
	const std::wstring& getOptionText(int index) const;

	// === Selection ===

	/*!
	 * \brief Get selected index (-1 = none)
	 */
	int getSelectedIndex() const { return m_selectedIndex; }

	/*!
	 * \brief Set selected index
	 */
	void setSelectedIndex(int index);

	/*!
	 * \brief Get selected text
	 */
	const std::wstring& getSelectedText() const;

	// === Dropdown State ===

	/*!
	 * \brief Get whether dropdown is open
	 */
	bool isOpen() const { return m_isOpen; }

	/*!
	 * \brief Open dropdown
	 */
	void open();

	/*!
	 * \brief Close dropdown
	 */
	void close();

	/*!
	 * \brief Toggle dropdown
	 */
	void toggle();

	// === Visuals ===

	/*!
	 * \brief Get button widget
	 */
	Button* getButton() const { return m_button; }

	/*!
	 * \brief Get dropdown panel (list container)
	 */
	ScrollView* getDropdownPanel() const { return m_dropdownPanel; }

	// === Events ===

	std::function<void(int, const std::wstring&)> onSelectionChanged;

	// === Interaction ===

	virtual bool onMouseButtonDown(MouseButton button, const Vector2& position) override;

	virtual void update(float deltaTime) override;
	virtual void render(RenderContext* rc, float parentOpacity) override;

private:
	std::vector<std::wstring> m_options;
	int m_selectedIndex = -1;
	bool m_isOpen = false;

	// Child widgets
	Button* m_button = nullptr;
	Text* m_selectedText = nullptr;
	Image* m_arrow = nullptr;
	ScrollView* m_dropdownPanel = nullptr;
	std::vector<DropdownOption*> m_optionWidgets;

	// Helper methods
	void createDefaultWidgets();
	void updateSelectedText();
	void createOptionWidgets();
	void onOptionSelected(int index);
};

}
