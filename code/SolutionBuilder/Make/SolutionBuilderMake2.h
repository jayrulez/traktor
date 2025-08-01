/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Ref.h"
#include "SolutionBuilder/SolutionBuilder.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SOLUTIONBUILDER_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::sb
{

class ScriptProcessor;

/*! Generate solution files for makefiles. */
class T_DLLCLASS SolutionBuilderMake2 : public SolutionBuilder
{
	T_RTTI_CLASS;

public:
	virtual ~SolutionBuilderMake2();

	virtual bool create(const CommandLine& cmdLine) override final;

	virtual bool generate(const Solution* solution, const Path& solutionPathName) override final;

	virtual void showOptions() const override final;

private:
	std::wstring m_solutionTemplate;
	std::wstring m_projectTemplate;
	Ref< ScriptProcessor > m_scriptProcessor;
};

}
