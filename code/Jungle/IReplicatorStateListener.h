/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Object.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_JUNGLE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::jungle
{

class Replicator;
class ReplicatorProxy;

class T_DLLCLASS IReplicatorStateListener : public Object
{
	T_RTTI_CLASS;

public:
	enum
	{
		ReConnected = 1,
		ReDisconnected = 2,
		ReState = 3,
		ReStatus = 4
	};

	virtual void notify(
		Replicator* replicator,
		float eventTime,
		uint32_t eventId,
		ReplicatorProxy* proxy,
		const Object* eventObject
	) = 0;
};

}
