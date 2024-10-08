/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <functional>
#include <string>
#include "Core/Ref.h"
#include "Core/Object.h"
#include "Core/Containers/SmallMap.h"
#include "Core/Io/Path.h"
#include "Core/Math/Matrix33.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SVG_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::xml
{

class Document;
class Element;

}

namespace traktor::svg
{

class Shape;
class Style;
class Gradient;

/*! SVG parser.
 * \ingroup SVG
 */
class T_DLLCLASS Parser : public Object
{
	T_RTTI_CLASS;

public:
	typedef std::function< Ref< const Style >(const std::wstring_view&) > class_style_fn;

	explicit Parser(const class_style_fn& classStyleFn = class_style_fn());

	Ref< Shape > parse(xml::Document* doc);

	Ref< Shape > parse(const traktor::Path& fileName);

private:
	class_style_fn m_classStyleFn;
	SmallMap< std::wstring, Ref< Gradient > > m_gradients;
	SmallMap< std::wstring, Ref< Shape > > m_shapeDefs;

	Ref< Shape > traverse(xml::Element* elm);

	Ref< Shape > parseDocument(xml::Element* elm);

	Ref< Shape > parseGroup(xml::Element* elm);

	Ref< Shape > parseCircle(xml::Element* elm);

	Ref< Shape > parseEllipse(xml::Element* elm);

	Ref< Shape > parseRect(const xml::Element* elm);

	Ref< Shape > parsePolygon(xml::Element* elm);

	Ref< Shape > parsePolyLine(xml::Element* elm);

	Ref< Shape > parsePath(xml::Element* elm);

	Ref< Shape > parseText(xml::Element* elm);

	Ref< Shape > parseImage(xml::Element* elm);

	void parseDefs(xml::Element* elm);

	Ref< Gradient > parseGradientDef(const xml::Element* defs, const xml::Element* elm) const;

	void parseStyle(const xml::Element* elm, Style* style);

	Matrix33 parseTransform(const xml::Element* elm, const std::wstring& attrName) const;

	float parseAttr(const xml::Element* elm, const std::wstring& attrName, float defValue = 0.0f) const;
};

}
