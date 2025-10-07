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
class Button;

/*!
 * \brief ScrollBar widget for scrolling content.
 *
 * ScrollBar provides a track, thumb, and optional increment/decrement buttons.
 *
 * Usage:
 *   Ref<ScrollBar> scrollbar = new ScrollBar(L"Scrollbar");
 *   scrollbar->setContentSize(1000);  // Total content size
 *   scrollbar->setViewportSize(200);  // Visible viewport size
 *   scrollbar->onValueChanged = [](float value) {
 *       scrollContent(value);
 *   };
 */
class T_DLLCLASS ScrollBar : public Interactable
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief ScrollBar orientation
	 */
	enum class Orientation
	{
		Horizontal,
		Vertical
	};

	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit ScrollBar(const std::wstring& name = L"ScrollBar");

	/*!
	 * \brief Destructor
	 */
	virtual ~ScrollBar();

	// === Value ===

	/*!
	 * \brief Get current scroll value (0 = start, 1 = end)
	 */
	float getValue() const { return m_value; }

	/*!
	 * \brief Set scroll value
	 */
	void setValue(float value);

	// === Content and Viewport ===

	/*!
	 * \brief Get total content size
	 */
	float getContentSize() const { return m_contentSize; }

	/*!
	 * \brief Set content size
	 */
	void setContentSize(float size);

	/*!
	 * \brief Get viewport size (visible portion)
	 */
	float getViewportSize() const { return m_viewportSize; }

	/*!
	 * \brief Set viewport size
	 */
	void setViewportSize(float size);

	/*!
	 * \brief Get thumb size (calculated from content/viewport ratio)
	 */
	float getThumbSize() const;

	// === Orientation ===

	/*!
	 * \brief Get orientation
	 */
	Orientation getOrientation() const { return m_orientation; }

	/*!
	 * \brief Set orientation
	 */
	void setOrientation(Orientation orientation);

	// === Buttons ===

	/*!
	 * \brief Get whether to show increment/decrement buttons
	 */
	bool getShowButtons() const { return m_showButtons; }

	/*!
	 * \brief Set whether to show buttons
	 */
	void setShowButtons(bool show);

	// === Visuals ===

	/*!
	 * \brief Get track image
	 */
	Image* getTrackImage() const { return m_trackImage; }

	/*!
	 * \brief Get thumb image (draggable)
	 */
	Image* getThumbImage() const { return m_thumbImage; }

	/*!
	 * \brief Get decrement button
	 */
	Button* getDecrementButton() const { return m_decrementButton; }

	/*!
	 * \brief Get increment button
	 */
	Button* getIncrementButton() const { return m_incrementButton; }

	// === Events ===

	std::function<void(float)> onValueChanged;

	// === Interaction ===

	virtual bool onMouseButtonDown(MouseButton button, const Vector2& position) override;
	virtual bool onMouseButtonUp(MouseButton button, const Vector2& position) override;
	virtual bool onMouseMove(const Vector2& position) override;
	virtual bool onMouseWheel(float delta) override;

	virtual void update(float deltaTime) override;
	virtual void render(RenderContext* rc, float parentOpacity) override;

private:
	float m_value = 0.0f;
	float m_contentSize = 1000.0f;
	float m_viewportSize = 200.0f;
	Orientation m_orientation = Orientation::Vertical;
	bool m_showButtons = true;

	// Dragging state
	bool m_draggingThumb = false;
	Vector2 m_dragStartPosition;
	float m_dragStartValue;

	// Child widgets
	Image* m_trackImage = nullptr;
	Image* m_thumbImage = nullptr;
	Button* m_decrementButton = nullptr;
	Button* m_incrementButton = nullptr;

	// Helper methods
	void createDefaultWidgets();
	void updateThumbPosition();
	void updateLayout();
	void setValueFromPosition(const Vector2& position);
	void scroll(float delta);
};

}
