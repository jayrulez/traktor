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

/*!
 * \brief Slider widget for selecting a value in a range.
 *
 * Slider displays a track with a draggable handle for selecting values.
 *
 * Usage:
 *   Ref<Slider> slider = new Slider(L"Volume");
 *   slider->setMinValue(0.0f);
 *   slider->setMaxValue(100.0f);
 *   slider->setValue(50.0f);
 *   slider->onValueChanged = [](float value) {
 *       setVolume(value);
 *   };
 */
class T_DLLCLASS Slider : public Interactable
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Slider orientation
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
	explicit Slider(const std::wstring& name = L"Slider");

	/*!
	 * \brief Destructor
	 */
	virtual ~Slider();

	// === Value ===

	/*!
	 * \brief Get current value
	 */
	float getValue() const { return m_value; }

	/*!
	 * \brief Set current value
	 */
	void setValue(float value);

	/*!
	 * \brief Get minimum value
	 */
	float getMinValue() const { return m_minValue; }

	/*!
	 * \brief Set minimum value
	 */
	void setMinValue(float minValue);

	/*!
	 * \brief Get maximum value
	 */
	float getMaxValue() const { return m_maxValue; }

	/*!
	 * \brief Set maximum value
	 */
	void setMaxValue(float maxValue);

	/*!
	 * \brief Get normalized value (0-1)
	 */
	float getNormalizedValue() const;

	/*!
	 * \brief Set normalized value (0-1)
	 */
	void setNormalizedValue(float normalized);

	// === Step ===

	/*!
	 * \brief Get step size (0 = no stepping)
	 */
	float getStepSize() const { return m_stepSize; }

	/*!
	 * \brief Set step size
	 */
	void setStepSize(float stepSize) { m_stepSize = stepSize; }

	// === Orientation ===

	/*!
	 * \brief Get orientation
	 */
	Orientation getOrientation() const { return m_orientation; }

	/*!
	 * \brief Set orientation
	 */
	void setOrientation(Orientation orientation);

	// === Visuals ===

	/*!
	 * \brief Get track image
	 */
	Image* getTrackImage() const { return m_trackImage; }

	/*!
	 * \brief Get fill image (filled portion of track)
	 */
	Image* getFillImage() const { return m_fillImage; }

	/*!
	 * \brief Get handle image (draggable thumb)
	 */
	Image* getHandleImage() const { return m_handleImage; }

	// === Events ===

	std::function<void(float)> onValueChanged;

	// === Interaction ===

	virtual bool onMouseButtonDown(MouseButton button, const Vector2& position) override;
	virtual bool onMouseButtonUp(MouseButton button, const Vector2& position) override;
	virtual bool onMouseMove(const Vector2& position) override;

	virtual void update(float deltaTime) override;
	virtual void render(RenderContext* rc, float parentOpacity) override;

private:
	float m_value = 0.0f;
	float m_minValue = 0.0f;
	float m_maxValue = 1.0f;
	float m_stepSize = 0.0f; // 0 = no stepping
	Orientation m_orientation = Orientation::Horizontal;

	// Dragging state
	bool m_draggingHandle = false;

	// Child widgets
	Image* m_trackImage = nullptr;
	Image* m_fillImage = nullptr;
	Image* m_handleImage = nullptr;

	// Helper methods
	void createDefaultWidgets();
	void updateHandlePosition();
	void updateFillSize();
	void setValueFromPosition(const Vector2& position);
	float clampValue(float value) const;
	float applyStep(float value) const;
};

}
