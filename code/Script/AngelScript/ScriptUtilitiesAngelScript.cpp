/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Script/AngelScript/ScriptUtilitiesAngelScript.h"

#include "angelscript.h"
#include "Core/Io/OutputStream.h"
#include "Core/Memory/IAllocator.h"
#include "Core/Thread/Thread.h"
#include "Core/Thread/ThreadManager.h"

namespace traktor::script
{

void asPrint(const std::string& str)
{
	log::info << mbstows(str) << Endl;
}

void asSleep(int32_t ms)
{
	if (ms >= 0)
		ThreadManager::getInstance().getCurrentThread()->sleep(ms);
}

size_t asGetAllocatedMemory()
{
	// This would need to be implemented to track AngelScript memory usage
	// For now, return 0
	return 0;
}

void registerStandardFunctions(asIScriptEngine* engine)
{
	// Register print function - convert string to wide string for Traktor's log
	engine->RegisterGlobalFunction("void print(const string &in)",
		asFUNCTION(asPrint),
		asCALL_CDECL);

	// Register sleep function
	engine->RegisterGlobalFunction("void sleep(int)",
		asFUNCTION(asSleep),
		asCALL_CDECL);

	// Register memory function
	engine->RegisterGlobalFunction("uint allocatedMemory()",
		asFUNCTION(asGetAllocatedMemory),
		asCALL_CDECL);
}

std::string asTypeToString(int typeId, asIScriptEngine* engine)
{
	const char* typeName = engine->GetTypeDeclaration(typeId);
	return typeName ? std::string(typeName) : "unknown";
}

bool asTypeIsPrimitive(int typeId)
{
	return (typeId >= asTYPEID_BOOL && typeId <= asTYPEID_DOUBLE) ||
		(typeId & asTYPEID_MASK_OBJECT) == 0;
}

bool asTypeIsObjectHandle(int typeId)
{
	return (typeId & asTYPEID_OBJHANDLE) != 0;
}

}