/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

namespace traktor
{
	namespace input
	{

bool isInputAllowed();

bool getMouseRange(NSSize& outRange);

bool getMousePosition(NSPoint& outMousePositionGlobal, NSPoint& outMousePositionLocal);

bool getMouseCenterPosition(NSPoint& outCenterPosition);

	}
}

