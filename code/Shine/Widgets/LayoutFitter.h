/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Shine/Layout.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

/*!
 * \brief LayoutFitter widget that automatically resizes to fit its content.
 *
 * LayoutFitter adjusts its size to match the size of its children, useful for
 * creating dynamic layouts that grow/shrink based on content.
 *
 * Usage:
 *   Ref<LayoutFitter> fitter = new LayoutFitter(L"ContentFitter");
 *   fitter->setFitWidth(true);
 *   fitter->setFitHeight(true);
 *   fitter->addChild(new Text(L"Dynamic content..."));
 *   // Fitter automatically resizes to fit text
 */
class T_DLLCLASS LayoutFitter : public Layout
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Fit mode
	 */
	enum class FitMode
	{
		Unconstrained,     // Don't constrain size
		MinSize,           // Fit to minimum size of children
		PreferredSize      // Fit to preferred size of children
	};

	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit LayoutFitter(const std::wstring& name = L"LayoutFitter");

	/*!
	 * \brief Destructor
	 */
	virtual ~LayoutFitter();

	// === Fit Mode ===

	/*!
	 * \brief Get horizontal fit mode
	 */
	FitMode getHorizontalFit() const { return m_horizontalFit; }

	/*!
	 * \brief Set horizontal fit mode
	 */
	void setHorizontalFit(FitMode mode);

	/*!
	 * \brief Get vertical fit mode
	 */
	FitMode getVerticalFit() const { return m_verticalFit; }

	/*!
	 * \brief Set vertical fit mode
	 */
	void setVerticalFit(FitMode mode);

	// === Convenience ===

	/*!
	 * \brief Set whether to fit width
	 */
	void setFitWidth(bool fit);

	/*!
	 * \brief Set whether to fit height
	 */
	void setFitHeight(bool fit);

	// === Layout ===

	virtual void updateLayout() override;

	virtual Vector2 calculateMinSize() const override;

	virtual Vector2 calculatePreferredSize() const override;

private:
	FitMode m_horizontalFit = FitMode::PreferredSize;
	FitMode m_verticalFit = FitMode::PreferredSize;

	// Helper to calculate fitted size
	Vector2 calculateFittedSize() const;
};

}
