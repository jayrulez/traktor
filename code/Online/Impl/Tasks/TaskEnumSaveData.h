/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <map>
#include <set>
#include "Online/Impl/ITask.h"

namespace traktor::online
{

class ISaveDataProvider;

class TaskEnumSaveData : public ITask
{
	T_RTTI_CLASS;

public:
	typedef void (Object::*sink_method_t)(const std::set< std::wstring >&);

	explicit TaskEnumSaveData(
		ISaveDataProvider* provider,
		Object* sinkObject,
		sink_method_t sinkMethod
	);

	virtual void execute(TaskQueue* taskQueue) override final;

private:
	Ref< ISaveDataProvider > m_provider;
	Ref< Object > m_sinkObject;
	sink_method_t m_sinkMethod;
};

}
