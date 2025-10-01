/*
 * TRAKTOR
 * Copyright (c) 2022-2023 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <angelscript.h>
#include "Core/Log/Log.h"
#include "Core/Misc/TString.h"

namespace traktor
{

class OutputStream;

}

namespace traktor::script
{

void asPrint(asIScriptGeneric* gen);

void asSleep(asIScriptGeneric* gen);

}
