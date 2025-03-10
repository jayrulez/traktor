/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Containers/AlignedVector.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_CORE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::test
{

class CaseIdAllocator;

}

namespace traktor
{

/*! ID allocator.
 * \ingroup Core
 */
class T_DLLCLASS IdAllocator
{
public:
	IdAllocator();

	explicit IdAllocator(uint32_t minId, uint32_t maxId);

	uint32_t alloc();

	uint32_t allocSequential(uint32_t span);

	void free(uint32_t id);

	void freeSequential(uint32_t id, uint32_t span);

private:
	friend test::CaseIdAllocator;

	struct Interval
	{
		uint32_t left;
		uint32_t right;

		Interval() = default;

		Interval(uint32_t left_, uint32_t right_);

		bool operator < (const Interval& rh) const;

		bool operator == (const Interval& rh) const;
	};

	AlignedVector< Interval > m_free;
};

}
