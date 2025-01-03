/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Spark/Character.h"
#include "Spark/Types.h"

namespace traktor::spark
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.spark.Character", Character, ISerializable)

Character::Character()
:	m_tag(allocateCacheTag())
{
}

void Character::serialize(ISerializer& s)
{
}

}
