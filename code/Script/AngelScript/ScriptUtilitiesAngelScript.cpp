/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <angelscript.h>
#include "Core/Io/OutputStream.h"
#include "Core/Memory/IAllocator.h"
#include "Core/Memory/MemoryConfig.h"
#include "Core/Thread/Thread.h"
#include "Core/Thread/ThreadManager.h"
#include "Script/AngelScript/ScriptUtilitiesAngelScript.h"

namespace traktor::script
{

void asPrint(asIScriptGeneric* gen)
{
	int32_t argCount = gen->GetArgCount();
	for (int32_t i = 0; i < argCount; ++i)
	{
		if (i > 0)
			log::info << L"\t";

		// Get argument as string
		const char* str = *(const char**)gen->GetAddressOfArg(i);
		if (str)
			log::info << mbstows(str);
	}
	log::info << Endl;
}

void asSleep(asIScriptGeneric* gen)
{
	int32_t ms = gen->GetArgDWord(0);
	if (ms >= 0)
		ThreadManager::getInstance().getCurrentThread()->sleep(ms);
}

}
