/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Serialization/ISerializable.h"
#include "Core/Ref.h"
#include "Core/Math/Vector2.h"
#include "Paper/Ui/UITypes.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_PAPER_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::paper
{

class UIContext;
class UIStyle;

/*! Base class for all UI elements.
 * \ingroup Paper
 *
 * UIElement is the base class for the Paper UI framework, inspired by WPF.
 * It provides the foundation for layout, input, and rendering.
 */
class T_DLLCLASS UIElement : public ISerializable
{
	T_RTTI_CLASS;

public:
	virtual ~UIElement() = default;

	/*! Apply a style to this element.
	 * \param style Style to apply, or nullptr to use default values
	 *
	 * Override this method in derived classes to apply style properties
	 * specific to that element type. Always call base class implementation.
	 */
	virtual void applyStyle(const UIStyle* style);

	/*! Measure the desired size of the element.
	 * \param availableSize Available space for this element.
	 * \param context UI context containing layout services.
	 * \return Desired size of the element.
	 */
	virtual Vector2 measure(const Vector2& availableSize, UIContext* context);

	/*! Arrange the element within the given bounds.
	 * \param finalRect Final position and size for this element.
	 */
	virtual void arrange(const Vector2& position, const Vector2& size);

	/*! Render the element.
	 * \param context UI context containing rendering services.
	 */
	virtual void render(UIContext* context);

	/*! Render debug visualization for layout.
	 * \param context UI context containing rendering services.
	 */
	virtual void renderDebug(UIContext* context);

	/*! Get the desired size after measure.
	 */
	const Vector2& getDesiredSize() const { return m_desiredSize; }

	/*! Get the actual size after arrange.
	 */
	const Vector2& getActualSize() const { return m_actualSize; }

	/*! Get the actual position after arrange.
	 */
	const Vector2& getActualPosition() const { return m_actualPosition; }

	/*! Hit test to find element at given position.
	 * \param position Position in UI coordinates.
	 * \return The deepest element at this position, or nullptr if none.
	 */
	virtual UIElement* hitTest(const Vector2& position);

	/*! Check if a point is within this element's bounds.
	 * \param position Position in UI coordinates.
	 * \return True if position is within bounds.
	 */
	virtual bool containsPoint(const Vector2& position) const;

	/*! Mouse moved over this element.
	 * \param event Mouse event data.
	 */
	virtual void onMouseMove(MouseEvent& event);

	/*! Mouse button pressed on this element.
	 * \param event Mouse event data.
	 */
	virtual void onMouseDown(MouseEvent& event);

	/*! Mouse button released on this element.
	 * \param event Mouse event data.
	 */
	virtual void onMouseUp(MouseEvent& event);

	/*! Mouse entered this element's bounds.
	 * \param event Mouse event data.
	 */
	virtual void onMouseEnter(MouseEvent& event);

	/*! Mouse left this element's bounds.
	 * \param event Mouse event data.
	 */
	virtual void onMouseLeave(MouseEvent& event);

	/*! Check if mouse is currently over this element.
	 */
	bool isMouseOver() const { return m_isMouseOver; }

	/*! Get the parent element.
	 */
	UIElement* getParent() const { return m_parent; }

	/*! Set the parent element (called by container during arrange).
	 */
	void setParent(UIElement* parent) { m_parent = parent; }

	virtual void serialize(ISerializer& s) override;

protected:
	Vector2 m_desiredSize = Vector2::zero();
	Vector2 m_actualSize = Vector2::zero();
	Vector2 m_actualPosition = Vector2::zero();
	UIElement* m_parent = nullptr;
	bool m_isMouseOver = false;
};

}
