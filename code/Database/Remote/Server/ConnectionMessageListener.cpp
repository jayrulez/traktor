/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Database/Remote/Server/ConnectionMessageListener.h"
#include "Database/Remote/Server/Connection.h"
#include "Database/Remote/Messages/CnmReleaseObject.h"
#include "Database/Remote/Messages/MsgStatus.h"

namespace traktor
{
	namespace db
	{

T_IMPLEMENT_RTTI_CLASS(L"traktor.db.ConnectionMessageListener", ConnectionMessageListener, IMessageListener)

ConnectionMessageListener::ConnectionMessageListener(Connection* connection)
:	m_connection(connection)
{
	registerMessage< CnmReleaseObject >(&ConnectionMessageListener::messageReleaseObject);
}

bool ConnectionMessageListener::messageReleaseObject(const CnmReleaseObject* message)
{
	m_connection->releaseObject(message->getHandle());
	m_connection->sendReply(MsgStatus(StSuccess));
	return true;
}

	}
}
