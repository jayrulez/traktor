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

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_PAPER_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::paper
{

class Draw2D;

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

	/*! Measure the desired size of the element.
	 * \param availableSize Available space for this element.
	 * \return Desired size of the element.
	 */
	virtual Vector2 measure(const Vector2& availableSize);

	/*! Arrange the element within the given bounds.
	 * \param finalRect Final position and size for this element.
	 */
	virtual void arrange(const Vector2& position, const Vector2& size);

	/*! Render the element.
	 * \param renderer 2D renderer to use for drawing.
	 */
	virtual void render(Draw2D* renderer);

	/*! Get the desired size after measure.
	 */
	const Vector2& getDesiredSize() const { return m_desiredSize; }

	/*! Get the actual size after arrange.
	 */
	const Vector2& getActualSize() const { return m_actualSize; }

	/*! Get the actual position after arrange.
	 */
	const Vector2& getActualPosition() const { return m_actualPosition; }

	virtual void serialize(ISerializer& s) override;

protected:
	Vector2 m_desiredSize = Vector2::zero();
	Vector2 m_actualSize = Vector2::zero();
	Vector2 m_actualPosition = Vector2::zero();
};

}
