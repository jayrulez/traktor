/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/RefArray.h"
#include "Core/Object.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SPARK_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor
{

class Any;
class IRuntimeDelegate;

}

namespace traktor::spark
{

/*! Event subject.
 * \ingroup Spark
 */
class T_DLLCLASS Event : public Object
{
	T_RTTI_CLASS;

public:
	int32_t add(IRuntimeDelegate* rd);

	void remove(int32_t index);

	void removeAll();

	bool empty() const;

	void issue();

	void issue(int32_t argc, const Any* argv);

private:
	RefArray< IRuntimeDelegate > m_rds;
};

}
