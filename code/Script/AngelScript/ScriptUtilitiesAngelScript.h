/*
 * TRAKTOR
 * Copyright (c) 2022-2023 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "angelscript.h"
#include "Core/Log/Log.h"
#include "Core/Misc/TString.h"

namespace traktor
{
class OutputStream;
}

namespace traktor::script
{

/*! Print function for AngelScript scripts */
void asPrint(const std::string& str);

/*! Sleep function for AngelScript scripts */
void asSleep(int32_t ms);

/*! Get allocated memory for AngelScript scripts */
size_t asGetAllocatedMemory();

/*! Register standard functions with AngelScript engine */
void registerStandardFunctions(asIScriptEngine* engine);

/*! Convert AngelScript type to string */
std::string asTypeToString(int typeId, asIScriptEngine* engine);

/*! Check if AngelScript type is primitive */
bool asTypeIsPrimitive(int typeId);

/*! Check if AngelScript type is object handle */
bool asTypeIsObjectHandle(int typeId);

}