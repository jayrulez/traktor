/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Serialization/ISerializer.h"
#include "Core/Serialization/MemberSmallMap.h"
#include "Paper/Ui/UIStyle.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.UIStyle", 0, UIStyle, ISerializable)

void UIStyle::setColor(const std::wstring& name, const Color4f& value)
{
	m_colors[name] = value;
}

bool UIStyle::tryGetColor(const std::wstring& name, Color4f& outValue) const
{
	auto it = m_colors.find(name);
	if (it != m_colors.end())
	{
		outValue = it->second;
		return true;
	}
	return false;
}

const Color4f& UIStyle::getColor(const std::wstring& name, const Color4f& defaultValue) const
{
	auto it = m_colors.find(name);
	return (it != m_colors.end()) ? it->second : defaultValue;
}

void UIStyle::setDimension(const std::wstring& name, float value)
{
	m_dimensions[name] = value;
}

bool UIStyle::tryGetDimension(const std::wstring& name, float& outValue) const
{
	auto it = m_dimensions.find(name);
	if (it != m_dimensions.end())
	{
		outValue = it->second;
		return true;
	}
	return false;
}

float UIStyle::getDimension(const std::wstring& name, float defaultValue) const
{
	auto it = m_dimensions.find(name);
	return (it != m_dimensions.end()) ? it->second : defaultValue;
}

void UIStyle::setVector(const std::wstring& name, const Vector2& value)
{
	m_vectors[name] = value;
}

bool UIStyle::tryGetVector(const std::wstring& name, Vector2& outValue) const
{
	auto it = m_vectors.find(name);
	if (it != m_vectors.end())
	{
		outValue = it->second;
		return true;
	}
	return false;
}

const Vector2& UIStyle::getVector(const std::wstring& name, const Vector2& defaultValue) const
{
	auto it = m_vectors.find(name);
	return (it != m_vectors.end()) ? it->second : defaultValue;
}

void UIStyle::setFont(const std::wstring& name, const Guid& fontId)
{
	m_fonts[name] = fontId;
}

bool UIStyle::tryGetFont(const std::wstring& name, Guid& outFontId) const
{
	auto it = m_fonts.find(name);
	if (it != m_fonts.end())
	{
		outFontId = it->second;
		return true;
	}
	return false;
}

const Guid& UIStyle::getFont(const std::wstring& name, const Guid& defaultFontId) const
{
	auto it = m_fonts.find(name);
	return (it != m_fonts.end()) ? it->second : defaultFontId;
}

void UIStyle::serialize(ISerializer& s)
{
	s >> MemberSmallMap< std::wstring, Color4f >(L"colors", m_colors);
	s >> MemberSmallMap< std::wstring, float >(L"dimensions", m_dimensions);
	s >> MemberSmallMap< std::wstring, Vector2 >(L"vectors", m_vectors);
	s >> MemberSmallMap< std::wstring, Guid >(L"fonts", m_fonts);
}

}
