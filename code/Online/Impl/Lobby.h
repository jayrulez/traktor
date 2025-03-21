/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Online/ILobby.h"

namespace traktor
{
	namespace online
	{

class IMatchMakingProvider;
class TaskQueue;
class UserCache;

class Lobby : public ILobby
{
	T_RTTI_CLASS;

public:
	virtual Ref< Result > setMetaValue(const std::wstring& key, const std::wstring& value) override final;

	virtual bool getMetaValue(const std::wstring& key, std::wstring& outValue) const override final;

	virtual Ref< Result > setParticipantMetaValue(const std::wstring& key, const std::wstring& value) override final;

	virtual bool getParticipantMetaValue(const IUser* user, const std::wstring& key, std::wstring& outValue) const override final;

	virtual Ref< Result > join() override final;

	virtual bool leave() override final;

	virtual bool getParticipants(RefArray< IUser >& outUsers) override final;

	virtual uint32_t getParticipantCount() const override final;

	virtual uint32_t getMaxParticipantCount() const override final;

	virtual uint32_t getFriendsCount() const override final;

	virtual bool invite(const IUser* user) override final;

	virtual bool setOwner(const IUser* user) override final;

	virtual const IUser* getOwner() const override final;

private:
	friend class MatchMaking;
	friend class TaskCreateLobby;
	friend class TaskFindMatchingLobbies;

	Ref< IMatchMakingProvider > m_matchMakingProvider;
	Ref< UserCache > m_userCache;
	Ref< TaskQueue > m_taskQueue;
	uint64_t m_handle;

	Lobby(IMatchMakingProvider* matchMakingProvider, UserCache* userCache, TaskQueue* taskQueue, uint64_t handle);
};

	}
}

