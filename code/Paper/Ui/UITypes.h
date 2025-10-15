/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

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

/*! Mouse button enumeration.
 */
enum class MouseButton
{
	Left = 0,
	Right = 1,
	Middle = 2
};

/*! Mouse event data.
 */
struct MouseEvent
{
	Vector2 position;        // Mouse position in UI coordinates
	MouseButton button;      // Which button caused the event
	bool handled = false;    // Set to true to stop event propagation
	bool capture = false;    // Set to true to request mouse capture
};

/*! Mouse wheel event data.
 */
struct MouseWheelEvent
{
	Vector2 position;        // Mouse position in UI coordinates
	int32_t delta;           // Wheel delta (positive = scroll up, negative = scroll down)
	bool handled = false;    // Set to true to stop event propagation
};

/*! Keyboard event data.
 */
struct KeyEvent
{
	wchar_t character = 0;   // Character input (0 if not a character key)
	int virtualKey = 0;      // Virtual key code (from ui::VirtualKey)
	bool shift = false;      // Shift key held
	bool control = false;    // Control key held
	bool alt = false;        // Alt key held
	bool handled = false;    // Set to true to stop event propagation
};

}
