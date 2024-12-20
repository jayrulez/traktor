/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <string>
#include "Core/Ref.h"
#include "Core/Object.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_ONLINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor
{
	namespace drawing
	{

class Image;

	}

	namespace online
	{

class T_DLLCLASS IUserProvider : public Object
{
	T_RTTI_CLASS;

public:
	virtual bool getName(uint64_t userHandle, std::wstring& outName) = 0;

	virtual Ref< drawing::Image > getImage(uint64_t userHandle) const = 0;

	virtual bool isFriend(uint64_t userHandle) = 0;

	virtual bool isMemberOfGroup(uint64_t userHandle, const std::wstring& groupName) const = 0;

	virtual bool joinGroup(uint64_t userHandle, const std::wstring& groupName) = 0;

	virtual bool invite(uint64_t userHandle) = 0;

	virtual bool setPresenceValue(uint64_t userHandle, const std::wstring& key, const std::wstring& value) = 0;

	virtual bool getPresenceValue(uint64_t userHandle, const std::wstring& key, std::wstring& outValue) = 0;

	virtual void setP2PEnable(uint64_t userHandle, bool enable) = 0;

	virtual bool isP2PAllowed(uint64_t userHandle) const = 0;

	virtual bool isP2PRelayed(uint64_t userHandle) const = 0;

	virtual bool sendP2PData(uint64_t userHandle, const void* data, size_t size, bool reliable) = 0;
};

	}
}

