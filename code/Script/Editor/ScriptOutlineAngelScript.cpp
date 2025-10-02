/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <cctype>
#include "Core/Log/Log.h"
#include "Script/Editor/ScriptOutlineAngelScript.h"

namespace traktor::script
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.script.ScriptOutlineAngelScript", 0, ScriptOutlineAngelScript, IScriptOutline)

Ref< IScriptOutline::Node > ScriptOutlineAngelScript::parse(const std::wstring& text) const
{
	m_text = text.c_str();
	m_length = (uint32_t)text.length();
	m_position = 0;
	m_line = 1;

	return parseBlock();
}

ScriptOutlineAngelScript::Token ScriptOutlineAngelScript::nextToken() const
{
	skipWhitespaceAndComments();

	Token token;
	token.line = m_line;

	wchar_t ch = peekChar();
	if (ch == L'\0')
	{
		token.type = Token::EndOfFile;
		return token;
	}

	// String literals
	if (ch == L'"' || ch == L'\'')
	{
		wchar_t quote = readChar();
		token.type = Token::String;
		token.text.clear();

		while (peekChar() != L'\0' && peekChar() != quote)
		{
			if (peekChar() == L'\\')
			{
				readChar();
				if (peekChar() != L'\0')
					token.text += readChar();
			}
			else
			{
				token.text += readChar();
			}
		}

		if (peekChar() == quote)
			readChar();

		return token;
	}

	// Numbers
	if (std::iswdigit(ch))
	{
		token.type = Token::Number;
		token.text.clear();

		while (std::iswdigit(peekChar()) || peekChar() == L'.' || peekChar() == L'f')
			token.text += readChar();

		return token;
	}

	// Words (identifiers and keywords)
	if (std::iswalpha(ch) || ch == L'_')
	{
		token.type = Token::Word;
		token.text.clear();

		while (std::iswalnum(peekChar()) || peekChar() == L'_')
			token.text += readChar();

		return token;
	}

	// Symbols
	token.type = Token::Symbol;
	token.text = readChar();

	// Multi-character operators
	wchar_t next = peekChar();
	if ((token.text == L"=" && next == L'=') ||
		(token.text == L"!" && next == L'=') ||
		(token.text == L"<" && next == L'=') ||
		(token.text == L">" && next == L'=') ||
		(token.text == L"&" && next == L'&') ||
		(token.text == L"|" && next == L'|') ||
		(token.text == L"+" && next == L'+') ||
		(token.text == L"-" && next == L'-') ||
		(token.text == L":" && next == L':'))
	{
		token.text += readChar();
	}

	return token;
}

wchar_t ScriptOutlineAngelScript::readChar() const
{
	if (m_position >= m_length)
		return L'\0';

	wchar_t ch = m_text[m_position++];
	if (ch == L'\n')
		m_line++;

	return ch;
}

wchar_t ScriptOutlineAngelScript::peekChar() const
{
	if (m_position >= m_length)
		return L'\0';

	return m_text[m_position];
}

void ScriptOutlineAngelScript::skipWhitespaceAndComments() const
{
	while (m_position < m_length)
	{
		wchar_t ch = peekChar();

		// Whitespace
		if (std::iswspace(ch))
		{
			readChar();
			continue;
		}

		// Single-line comments
		if (ch == L'/' && m_position + 1 < m_length && m_text[m_position + 1] == L'/')
		{
			readChar(); // /
			readChar(); // /
			while (peekChar() != L'\0' && peekChar() != L'\n')
				readChar();
			continue;
		}

		// Multi-line comments
		if (ch == L'/' && m_position + 1 < m_length && m_text[m_position + 1] == L'*')
		{
			readChar(); // /
			readChar(); // *
			while (m_position + 1 < m_length)
			{
				if (peekChar() == L'*' && m_text[m_position + 1] == L'/')
				{
					readChar(); // *
					readChar(); // /
					break;
				}
				readChar();
			}
			continue;
		}

		break;
	}
}

Ref< IScriptOutline::Node > ScriptOutlineAngelScript::parseBlock() const
{
	Ref< Node > head;
	Ref< Node > tail;

	while (true)
	{
		Token token = nextToken();

		if (token.type == Token::EndOfFile)
			break;

		// Function definitions
		if (token.type == Token::Word && !isKeyword(token.text))
		{
			int32_t functionLine = token.line;
			std::wstring returnType = token.text;

			// Check for array type (type[])
			Token peek = nextToken();
			if (peek.type == Token::Symbol && peek.text == L"[")
			{
				Token closeBracket = nextToken();
				if (closeBracket.type == Token::Symbol && closeBracket.text == L"]")
				{
					returnType += L"[]";
					peek = nextToken();
				}
			}

			// Check for handle type (type@)
			if (peek.type == Token::Symbol && peek.text == L"@")
			{
				returnType += L"@";
				peek = nextToken();
			}

			// Function name
			if (peek.type == Token::Word)
			{
				std::wstring functionName = peek.text;

				Token openParen = nextToken();
				if (openParen.type == Token::Symbol && openParen.text == L"(")
				{
					// Skip parameters
					int parenDepth = 1;
					while (parenDepth > 0)
					{
						Token paramToken = nextToken();
						if (paramToken.type == Token::EndOfFile)
							break;
						if (paramToken.type == Token::Symbol && paramToken.text == L"(")
							parenDepth++;
						else if (paramToken.type == Token::Symbol && paramToken.text == L")")
							parenDepth--;
					}

					// Skip optional const, override, final, etc.
					while (true)
					{
						Token modifier = nextToken();
						if (modifier.type != Token::Word ||
							(modifier.text != L"const" && modifier.text != L"override" && modifier.text != L"final"))
						{
							// Put token back by adjusting position
							m_position -= (uint32_t)modifier.text.length();
							break;
						}
					}

					// Check for function body
					Token bodyStart = nextToken();
					if (bodyStart.type == Token::Symbol && bodyStart.text == L"{")
					{
						// Parse function body recursively
						Ref< Node > body = parseBlock();

						// Create function node
						Ref< FunctionNode > funcNode = new FunctionNode(
							functionLine,
							functionName,
							false,  // AngelScript doesn't have "local" functions like Lua
							body
						);

						if (!head)
						{
							head = tail = funcNode;
						}
						else
						{
							tail->setNext(funcNode);
							tail = funcNode;
						}

						continue;
					}
					else if (bodyStart.type == Token::Symbol && bodyStart.text == L";")
					{
						// Forward declaration or interface method - skip
						continue;
					}
				}
			}
		}

		// Class definitions
		if (token.type == Token::Word && token.text == L"class")
		{
			Token className = nextToken();
			if (className.type == Token::Word)
			{
				// Skip base class specification
				Token peek = nextToken();
				while (peek.type == Token::Symbol && peek.text == L":")
				{
					peek = nextToken(); // Skip base class name
					if (peek.type == Token::Word)
						peek = nextToken();
				}

				// Parse class body
				if (peek.type == Token::Symbol && peek.text == L"{")
				{
					Ref< Node > classBody = parseBlock();
					// We could create a ClassNode here if needed, but for now just merge the body
					if (classBody)
					{
						if (!head)
						{
							head = tail = classBody;
						}
						else
						{
							tail->setNextTail(classBody);
							tail = classBody;
							while (tail->getNext())
								tail = tail->getNext();
						}
					}
				}
			}
			continue;
		}

		// Interface definitions
		if (token.type == Token::Word && token.text == L"interface")
		{
			Token interfaceName = nextToken();
			if (interfaceName.type == Token::Word)
			{
				Token bodyStart = nextToken();
				if (bodyStart.type == Token::Symbol && bodyStart.text == L"{")
				{
					Ref< Node > interfaceBody = parseBlock();
					if (interfaceBody)
					{
						if (!head)
						{
							head = tail = interfaceBody;
						}
						else
						{
							tail->setNextTail(interfaceBody);
							tail = interfaceBody;
							while (tail->getNext())
								tail = tail->getNext();
						}
					}
				}
			}
			continue;
		}

		// Namespace
		if (token.type == Token::Word && token.text == L"namespace")
		{
			// Skip namespace name
			Token namespaceName = nextToken();
			Token bodyStart = nextToken();
			if (bodyStart.type == Token::Symbol && bodyStart.text == L"{")
			{
				Ref< Node > namespaceBody = parseBlock();
				if (namespaceBody)
				{
					if (!head)
					{
						head = tail = namespaceBody;
					}
					else
					{
						tail->setNextTail(namespaceBody);
						tail = namespaceBody;
						while (tail->getNext())
							tail = tail->getNext();
					}
				}
			}
			continue;
		}

		// End of block
		if (token.type == Token::Symbol && token.text == L"}")
		{
			break;
		}

		// Skip other constructs like if, while, for, etc.
		if (token.type == Token::Word && (
			token.text == L"if" ||
			token.text == L"while" ||
			token.text == L"for" ||
			token.text == L"switch"))
		{
			// Skip condition
			Token openParen = nextToken();
			if (openParen.type == Token::Symbol && openParen.text == L"(")
			{
				int parenDepth = 1;
				while (parenDepth > 0)
				{
					Token t = nextToken();
					if (t.type == Token::EndOfFile)
						break;
					if (t.type == Token::Symbol && t.text == L"(")
						parenDepth++;
					else if (t.type == Token::Symbol && t.text == L")")
						parenDepth--;
				}
			}

			// Parse body if it exists
			Token bodyStart = nextToken();
			if (bodyStart.type == Token::Symbol && bodyStart.text == L"{")
			{
				Ref< Node > body = parseBlock();
				if (body)
				{
					if (!head)
					{
						head = tail = body;
					}
					else
					{
						tail->setNextTail(body);
						tail = body;
						while (tail->getNext())
							tail = tail->getNext();
					}
				}
			}
			continue;
		}

		// Function calls (for reference tracking)
		if (token.type == Token::Word && !isKeyword(token.text))
		{
			Token peek = nextToken();
			if (peek.type == Token::Symbol && peek.text == L"(")
			{
				// This is a function call
				Ref< FunctionReferenceNode > refNode = new FunctionReferenceNode(token.line, token.text);

				if (!head)
				{
					head = tail = refNode;
				}
				else
				{
					tail->setNext(refNode);
					tail = refNode;
				}

				// Skip arguments
				int parenDepth = 1;
				while (parenDepth > 0)
				{
					Token t = nextToken();
					if (t.type == Token::EndOfFile)
						break;
					if (t.type == Token::Symbol && t.text == L"(")
						parenDepth++;
					else if (t.type == Token::Symbol && t.text == L")")
						parenDepth--;
				}
			}
		}
	}

	return head;
}

bool ScriptOutlineAngelScript::isKeyword(const std::wstring& word) const
{
	static const wchar_t* keywords[] = {
		L"void", L"bool", L"int", L"int8", L"int16", L"int32", L"int64",
		L"uint", L"uint8", L"uint16", L"uint32", L"uint64",
		L"float", L"double", L"string",
		L"class", L"interface", L"namespace", L"enum",
		L"if", L"else", L"while", L"for", L"do", L"switch", L"case", L"default",
		L"break", L"continue", L"return",
		L"const", L"private", L"protected", L"public",
		L"true", L"false", L"null",
		L"auto", L"cast", L"funcdef", L"typedef",
		nullptr
	};

	for (int i = 0; keywords[i]; ++i)
	{
		if (word == keywords[i])
			return true;
	}

	return false;
}

}
