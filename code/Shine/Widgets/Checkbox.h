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

/*!
 * \brief Checkbox widget for boolean toggle.
 *
 * Checkbox displays a box that can be checked/unchecked, with optional label.
 *
 * Usage:
 *   Ref<Checkbox> checkbox = new Checkbox(L"EnableSound");
 *   checkbox->setText(L"Enable Sound");
 *   checkbox->setChecked(true);
 *   checkbox->onCheckedChanged = [](bool checked) {
 *       setSoundEnabled(checked);
 *   };
 */
class T_DLLCLASS Checkbox : public Interactable
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit Checkbox(const std::wstring& name = L"Checkbox");

	/*!
	 * \brief Destructor
	 */
	virtual ~Checkbox();

	// === State ===

	/*!
	 * \brief Get checked state
	 */
	bool isChecked() const { return m_checked; }

	/*!
	 * \brief Set checked state
	 */
	void setChecked(bool checked);

	/*!
	 * \brief Toggle checked state
	 */
	void toggle() { setChecked(!m_checked); }

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
	 * \brief Get box image widget
	 */
	Image* getBoxImage() const { return m_boxImage; }

	/*!
	 * \brief Get checkmark image widget
	 */
	Image* getCheckmarkImage() const { return m_checkmarkImage; }

	// === Events ===

	std::function<void(bool)> onCheckedChanged;

	// === Interaction ===

	virtual bool onMouseButtonDown(MouseButton button, const Vector2& position) override;

	virtual void render(RenderContext* rc, float parentOpacity) override;

private:
	bool m_checked = false;

	// Child widgets
	Image* m_boxImage = nullptr;
	Image* m_checkmarkImage = nullptr;
	Text* m_textWidget = nullptr;

	// Helper to create default widgets
	void createDefaultWidgets();

	// Helper to update checkmark visibility
	void updateCheckmarkVisibility();
};

}
