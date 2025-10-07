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

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

class Image;
class Text;
class Dropdown;

/*!
 * \brief DropdownOption represents a single option in a Dropdown menu.
 *
 * This is typically created internally by Dropdown, but can be customized.
 */
class T_DLLCLASS DropdownOption : public Interactable
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit DropdownOption(const std::wstring& name = L"DropdownOption");

	/*!
	 * \brief Destructor
	 */
	virtual ~DropdownOption();

	// === Option Properties ===

	/*!
	 * \brief Get option text
	 */
	const std::wstring& getText() const;

	/*!
	 * \brief Set option text
	 */
	void setText(const std::wstring& text);

	/*!
	 * \brief Get option index in dropdown
	 */
	int getOptionIndex() const { return m_optionIndex; }

	/*!
	 * \brief Set option index
	 */
	void setOptionIndex(int index) { m_optionIndex = index; }

	/*!
	 * \brief Get parent dropdown
	 */
	Dropdown* getDropdown() const { return m_dropdown; }

	/*!
	 * \brief Set parent dropdown
	 */
	void setDropdown(Dropdown* dropdown) { m_dropdown = dropdown; }

	// === Visuals ===

	/*!
	 * \brief Get background image
	 */
	Image* getBackgroundImage() const { return m_backgroundImage; }

	/*!
	 * \brief Get text widget
	 */
	Text* getTextWidget() const { return m_textWidget; }

	// === Interaction ===

	virtual bool onMouseButtonDown(MouseButton button, const Vector2& position) override;
	virtual void onInteractionStateChanged() override;

	virtual void render(RenderContext* rc, float parentOpacity) override;

private:
	int m_optionIndex = -1;
	Dropdown* m_dropdown = nullptr;

	// Child widgets
	Image* m_backgroundImage = nullptr;
	Text* m_textWidget = nullptr;

	// Helper methods
	void createDefaultWidgets();
	void updateVisualState();
};

}
