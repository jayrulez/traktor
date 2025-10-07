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
class RadioButtonGroup;

/*!
 * \brief Radio button widget for exclusive selection.
 *
 * Radio buttons are grouped together, only one can be selected at a time.
 * Use RadioButtonGroup to manage a group of radio buttons.
 *
 * Usage:
 *   Ref<RadioButtonGroup> group = new RadioButtonGroup();
 *
 *   Ref<RadioButton> rb1 = new RadioButton(L"Easy");
 *   rb1->setText(L"Easy");
 *   rb1->setGroup(group);
 *
 *   Ref<RadioButton> rb2 = new RadioButton(L"Normal");
 *   rb2->setText(L"Normal");
 *   rb2->setGroup(group);
 *
 *   group->setSelected(rb1);
 */
class T_DLLCLASS RadioButton : public Interactable
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit RadioButton(const std::wstring& name = L"RadioButton");

	/*!
	 * \brief Destructor
	 */
	virtual ~RadioButton();

	// === State ===

	/*!
	 * \brief Get selected state
	 */
	bool isSelected() const { return m_selected; }

	/*!
	 * \brief Set selected state (use group->setSelected() instead)
	 */
	void setSelected(bool selected);

	// === Group ===

	/*!
	 * \brief Get radio button group
	 */
	RadioButtonGroup* getGroup() const { return m_group; }

	/*!
	 * \brief Set radio button group
	 */
	void setGroup(RadioButtonGroup* group);

	// === Label ===

	/*!
	 * \brief Get label text
	 */
	const std::wstring& getText() const;

	/*!
	 * \brief Set label text
	 */
	void setText(const std::wstring& text);

	/*!
	 * \brief Get text widget
	 */
	Text* getTextWidget() const { return m_textWidget; }

	// === Visuals ===

	/*!
	 * \brief Get circle image widget
	 */
	Image* getCircleImage() const { return m_circleImage; }

	/*!
	 * \brief Get dot image widget
	 */
	Image* getDotImage() const { return m_dotImage; }

	// === Events ===

	std::function<void()> onSelected;

	// === Interaction ===

	virtual bool onMouseButtonDown(MouseButton button, const Vector2& position) override;

	virtual void render(RenderContext* rc, float parentOpacity) override;

private:
	bool m_selected = false;
	RadioButtonGroup* m_group = nullptr;

	// Child widgets
	Image* m_circleImage = nullptr;
	Image* m_dotImage = nullptr;
	Text* m_textWidget = nullptr;

	// Helper to create default widgets
	void createDefaultWidgets();

	// Helper to update dot visibility
	void updateDotVisibility();
};

/*!
 * \brief Radio button group for managing exclusive selection.
 *
 * RadioButtonGroup manages a group of radio buttons, ensuring only one
 * is selected at a time.
 */
class T_DLLCLASS RadioButtonGroup : public Object
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 */
	RadioButtonGroup();

	/*!
	 * \brief Destructor
	 */
	virtual ~RadioButtonGroup();

	// === Selection ===

	/*!
	 * \brief Get selected radio button
	 */
	RadioButton* getSelected() const { return m_selected; }

	/*!
	 * \brief Set selected radio button
	 */
	void setSelected(RadioButton* button);

	// === Group Management ===

	/*!
	 * \brief Add radio button to group
	 */
	void addButton(RadioButton* button);

	/*!
	 * \brief Remove radio button from group
	 */
	void removeButton(RadioButton* button);

	/*!
	 * \brief Get all buttons in group
	 */
	const AlignedVector<RadioButton*>& getButtons() const { return m_buttons; }

	// === Events ===

	std::function<void(RadioButton*)> onSelectionChanged;

private:
	RadioButton* m_selected = nullptr;
	AlignedVector<RadioButton*> m_buttons;
};

}
