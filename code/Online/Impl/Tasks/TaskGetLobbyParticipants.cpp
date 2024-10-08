/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Online/UserArrayResult.h"
#include "Online/Impl/User.h"
#include "Online/Impl/UserCache.h"
#include "Online/Impl/Tasks/TaskGetLobbyParticipants.h"
#include "Online/Provider/IMatchMakingProvider.h"

namespace traktor::online
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.online.TaskGetLobbyParticipants", TaskGetLobbyParticipants, ITask)

TaskGetLobbyParticipants::TaskGetLobbyParticipants(
	IMatchMakingProvider* matchMakingProvider,
	UserCache* userCache,
	uint64_t lobbyHandle,
	UserArrayResult* result
)
:	m_matchMakingProvider(matchMakingProvider)
,	m_userCache(userCache)
,	m_lobbyHandle(lobbyHandle)
,	m_result(result)
{
}

void TaskGetLobbyParticipants::execute(TaskQueue* taskQueue)
{
	T_ASSERT(m_matchMakingProvider);
	T_ASSERT(m_userCache);
	T_ASSERT(m_result);

	std::vector< uint64_t > userHandles;
	if (m_matchMakingProvider->getLobbyParticipants(m_lobbyHandle, userHandles))
	{
		RefArray< IUser > users;
		users.reserve(userHandles.size());
		for (std::vector< uint64_t >::iterator i = userHandles.begin(); i != userHandles.end(); ++i)
			users.push_back(m_userCache->get(*i));
		m_result->succeed(users);
	}
	else
		m_result->fail();
}

}
