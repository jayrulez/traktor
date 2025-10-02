/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Script/Editor/IScriptOutline.h"

namespace traktor::script
{

/*! AngelScript script outline parser.
 * \ingroup Script
 */
class ScriptOutlineAngelScript : public IScriptOutline
{
	T_RTTI_CLASS;

public:
	virtual Ref< Node > parse(const std::wstring& text) const override final;

private:
	struct Token
	{
		enum Type
		{
			EndOfFile,
			Word,
			Symbol,
			String,
			Number
		};

		Type type;
		std::wstring text;
		int32_t line;
	};

	mutable const wchar_t* m_text;
	mutable uint32_t m_length;
	mutable uint32_t m_position;
	mutable int32_t m_line;

	Token nextToken() const;

	wchar_t readChar() const;

	wchar_t peekChar() const;

	void skipWhitespaceAndComments() const;

	Ref< Node > parseBlock() const;

	bool isKeyword(const std::wstring& word) const;
};

}
