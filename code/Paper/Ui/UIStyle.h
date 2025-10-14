/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <string>
#include "Core/Serialization/ISerializable.h"
#include "Core/Containers/SmallMap.h"
#include "Core/Guid.h"
#include "Core/Math/Color4f.h"
#include "Core/Math/Vector2.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_PAPER_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::paper
{

/*! UI Style.
 * \ingroup Paper
 *
 * Holds styling properties for UI elements.
 * Supports colors, dimensions, fonts, and custom properties.
 */
class T_DLLCLASS UIStyle : public ISerializable
{
	T_RTTI_CLASS;

public:
	// Color properties
	void setColor(const std::wstring& name, const Color4f& value);
	bool tryGetColor(const std::wstring& name, Color4f& outValue) const;
	const Color4f& getColor(const std::wstring& name, const Color4f& defaultValue) const;

	// Dimension properties (floats)
	void setDimension(const std::wstring& name, float value);
	bool tryGetDimension(const std::wstring& name, float& outValue) const;
	float getDimension(const std::wstring& name, float defaultValue) const;

	// Vector properties
	void setVector(const std::wstring& name, const Vector2& value);
	bool tryGetVector(const std::wstring& name, Vector2& outValue) const;
	const Vector2& getVector(const std::wstring& name, const Vector2& defaultValue) const;

	// Font properties
	void setFont(const std::wstring& name, const Guid& fontId);
	bool tryGetFont(const std::wstring& name, Guid& outFontId) const;
	const Guid& getFont(const std::wstring& name, const Guid& defaultFontId) const;

	virtual void serialize(ISerializer& s) override;

private:
	SmallMap< std::wstring, Color4f > m_colors;
	SmallMap< std::wstring, float > m_dimensions;
	SmallMap< std::wstring, Vector2 > m_vectors;
	SmallMap< std::wstring, Guid > m_fonts;
};

}
