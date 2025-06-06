/*
 * TRAKTOR
 * Copyright (c) 2022-2025 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/System/OS.h"

#include "Core/Io/Path.h"
#include "Core/Log/Log.h"
#include "Core/Math/MathUtils.h"
#include "Core/Misc/String.h"
#include "Core/Misc/StringSplit.h"
#include "Core/Misc/TString.h"
#include "Core/Singleton/SingletonManager.h"
#include "Core/System/Environment.h"

#include <cstdio>
#include <cstring>
#include <pwd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace traktor
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.OS", OS, Object)

OS& OS::getInstance()
{
	static OS* s_instance = nullptr;
	if (!s_instance)
	{
		s_instance = new OS();
		s_instance->addRef(nullptr);
		SingletonManager::getInstance().add(s_instance);
	}
	return *s_instance;
}

std::wstring OS::getName() const
{
	return L"Android";
}

std::wstring OS::getIdentifier() const
{
	return L"android";
}

uint32_t OS::getCPUCoreCount() const
{
	char possible[1024] = { '\0' };
	FILE* fp = fopen("/sys/devices/system/cpu/possible", "r");
	if (fp)
	{
		fgets(possible, sizeof(possible), fp);
		fclose(fp);

		char* p = strchr(possible, '-');
		if (p)
		{
			uint32_t count = parseString< uint32_t >(p + 1);
			return clamp< uint32_t >(count, 1, 16);
		}
	}
	return 1;
}

Path OS::getExecutable() const
{
	return L"";
}

std::wstring OS::getCommandLine() const
{
	char cmdLine[1024] = { '\0' };
	FILE* fp = fopen("/proc/self/cmdline", "r");
	if (fp)
	{
		fgets(cmdLine, sizeof(cmdLine), fp);
		fclose(fp);
	}
	return mbstows(cmdLine);
}

std::wstring OS::getComputerName() const
{
	char name[1024];

	if (gethostname(name, sizeof_array(name)) != -1)
		return mbstows(name);

	return L"Unavailable";
}

std::wstring OS::getCurrentUser() const
{
	passwd* pwd = getpwuid(geteuid());
	if (!pwd)
		return L"Unavailable";

	const char* who = pwd->pw_name;
	if (!who)
		return L"Unavailable";

	return mbstows(who);
}

std::wstring OS::getUserHomePath() const
{
	std::wstring home;
	if (getEnvironment(L"HOME", home))
		return home;
	else
		return L"~";
}

std::wstring OS::getUserApplicationDataPath() const
{
	std::wstring path;
	if (getEnvironment(L"INTERNAL_DATA_PATH", path))
		return L"C:" + path;
	else
		return L"";
}

std::wstring OS::getWritableFolderPath() const
{
	std::wstring path;
	if (getEnvironment(L"INTERNAL_DATA_PATH", path))
		return L"C:" + path;
	else
		return L"";
}

bool OS::openFile(const std::wstring& file) const
{
	return false;
}

bool OS::editFile(const std::wstring& file) const
{
	return false;
}

bool OS::exploreFile(const std::wstring& file) const
{
	return false;
}

bool OS::setEnvironment(const std::wstring& name, const std::wstring& value) const
{
	return false;
}

Ref< Environment > OS::getEnvironment() const
{
	Ref< Environment > env = new Environment();
	for (char** e = environ; *e; ++e)
	{
		char* sep = strchr(*e, '=');
		if (sep)
		{
			char* val = sep + 1;
			env->set(
				mbstows(std::string(*e, sep)),
				mbstows(val));
		}
	}
	return env;
}

bool OS::getEnvironment(const std::wstring& name, std::wstring& outValue) const
{
	const char* value = getenv(wstombs(name).c_str());
	if (value)
	{
		outValue = mbstows(value);
		return true;
	}
	else
		return false;
}

Ref< IProcess > OS::execute(
	const std::wstring& commandLine,
	const Path& workingDirectory,
	const Environment* env,
	uint32_t flags) const
{
	return nullptr;
}

Ref< ISharedMemory > OS::createSharedMemory(const std::wstring& name, uint32_t size) const
{
	return nullptr;
}

bool OS::setOwnProcessPriorityBias(int32_t priorityBias)
{
	return false;
}

bool OS::whereIs(const std::wstring& executable, Path& outPath) const
{
	std::wstring paths;

	// Get system "PATH" environment variable.
	if (!getEnvironment(L"PATH", paths))
		return false;

	// Try to locate binary in any of the paths specified in "PATH".
	for (auto path : StringSplit< std::wstring >(paths, L";:,"))
	{
		Ref< File > file = FileSystem::getInstance().get(path + L"/" + executable);
		if (file)
		{
			outPath = file->getPath();
			return true;
		}
	}

	return false;
}

bool OS::getAssociatedExecutable(const std::wstring& extension, Path& outPath) const
{
	return false;
}

OS::OS()
{
}

OS::~OS()
{
}

void OS::destroy()
{
	T_SAFE_RELEASE(this);
}

}
