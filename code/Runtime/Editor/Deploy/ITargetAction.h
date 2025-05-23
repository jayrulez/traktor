/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <string>
#include "Core/Object.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_RUNTIME_DEPLOY_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::runtime
{

/*! Abstract target action class.
 * \ingroup Runtime
 */
class T_DLLCLASS ITargetAction : public Object
{
	T_RTTI_CLASS;

public:
	struct IProgressListener : public IRefCount
	{
		virtual void notifyLog(int32_t level, const std::wstring& log) = 0;

		virtual void notifyTargetActionProgress(int32_t currentStep, int32_t maxStep) = 0;
	};

	virtual bool execute(IProgressListener* progressListener) = 0;
};

}
