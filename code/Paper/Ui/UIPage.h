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
#include "Core/RefArray.h"
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
class UIElement;
class UITheme;

/*! UI Page definition.
 * \ingroup Paper
 *
 * A UIPage is a container that holds the root UIElement of a UI hierarchy.
 * It can be serialized and loaded from assets.
 */
class T_DLLCLASS UIPage : public ISerializable
{
	T_RTTI_CLASS;

public:
	UIPage() = default;

	void setRoot(UIElement* root) { m_root = root; }

	UIElement* getRoot() const { return m_root; }

	void setWidth(int32_t width) { m_width = width; }

	int32_t getWidth() const { return m_width; }

	void setHeight(int32_t height) { m_height = height; }

	int32_t getHeight() const { return m_height; }

	void setTheme(UITheme* theme) { m_theme = theme; }

	UITheme* getTheme() const { return m_theme; }

	/*! Update UI layout (measure and arrange).
	 * Should be called before rendering if the UI or window size has changed.
	 */
	void updateLayout(UIContext* context);

	/*! Render the UI page.
	 * \param context UI context with renderer, font manager, etc.
	 * \param debugVisualization If true, renders debug overlays showing element bounds.
	 */
	void render(UIContext* context, bool debugVisualization = false);

	/*! Handle mouse movement.
	 * \param position Mouse position in UI coordinates.
	 */
	void handleMouseMove(const Vector2& position);

	/*! Handle mouse button down.
	 * \param position Mouse position in UI coordinates.
	 * \param button Which mouse button was pressed.
	 */
	void handleMouseDown(const Vector2& position, MouseButton button);

	/*! Handle mouse button up.
	 * \param position Mouse position in UI coordinates.
	 * \param button Which mouse button was released.
	 */
	void handleMouseUp(const Vector2& position, MouseButton button);

	virtual void serialize(ISerializer& s) override final;

private:
	Ref< UIElement > m_root;
	Ref< UITheme > m_theme;
	UIElement* m_hoveredElement = nullptr;
	RefArray< UIElement > m_hoveredAncestors;
	int32_t m_width = 1920;
	int32_t m_height = 1080;
};

}
