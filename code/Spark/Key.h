/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/RefArray.h"
#include "Spark/Event.h"

namespace traktor::spark
{

/*! Key class.
 * \ingroup Spark
 */
class Key : public Object
{
	T_RTTI_CLASS;

public:
	enum AsKeyEnum
	{
		AkBackspace = 8,
		AkCapsLock = 20,
		AkControl = 17,
		AkDeleteKey = 46,
		AkDown = 40,
		AkEnd = 35,
		AkEnter = 13,
		AkEscape = 27,
		AkHome = 36,
		AkInsert = 45,
		AkLeft = 37,
		AkPgDn = 34,
		AkPgUp = 33,
		AkRight = 39,
		AkShift = 16,
		AkSpace = 32,
		AkTab = 9,
		AkUp = 38
	};

	Key();

	int32_t getAscii() const;

	int32_t getCode() const;

	bool isDown(int32_t keyCode) const;

	void eventKeyDown(int32_t keyCode);

	void eventKeyUp(int32_t keyCode);

	/*! \group Events */
	//@{

	Event* getEventKeyDown() { return &m_eventKeyDown; }

	Event* getEventKeyUp() { return &m_eventKeyUp; }

	//@}

private:
	bool m_keyState[256];
	int32_t m_lastKeyCode;
	Event m_eventKeyDown;
	Event m_eventKeyUp;
};

}
