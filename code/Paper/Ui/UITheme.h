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
#include "Core/Ref.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_PAPER_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::paper
{

class UIStyle;

/*! UI Theme.
 * \ingroup Paper
 *
 * A collection of named styles for different UI elements and states.
 * Supports style lookup by element type and optional state (e.g., "Button:Hover").
 */
class T_DLLCLASS UITheme : public ISerializable
{
	T_RTTI_CLASS;

public:
	/*! Set a style for a given name.
	 * \param name Style name (e.g., "Button", "TextBlock", "Button:Hover")
	 * \param style The style to associate with this name
	 */
	void setStyle(const std::wstring& name, UIStyle* style);

	/*! Get a style by name.
	 * \param name Style name
	 * \return Style, or nullptr if not found
	 */
	UIStyle* getStyle(const std::wstring& name) const;

	/*! Get all style names.
	 */
	const SmallMap< std::wstring, Ref< UIStyle > >& getStyles() const { return m_styles; }

	virtual void serialize(ISerializer& s) override;

private:
	SmallMap< std::wstring, Ref< UIStyle > > m_styles;
};

}
