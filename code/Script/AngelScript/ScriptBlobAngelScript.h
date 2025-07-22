/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Misc/AutoPtr.h"
#include "Script/IScriptBlob.h"
#include <string>

namespace traktor::script
{

/*! Serialized blob of AngelScript code.
 * \ingroup Script
 */
class ScriptBlobAngelScript : public IScriptBlob
{
	T_RTTI_CLASS;

public:
	virtual void serialize(ISerializer& s) override final;

private:
	friend class ScriptCompilerAngelScript;
	friend class ScriptContextAngelScript;
	friend class ScriptManagerAngelScript;

	std::string m_fileName;
	std::string m_script;
};

}