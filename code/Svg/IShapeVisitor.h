/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Object.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SVG_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::svg
{

class Shape;

/*! SVG document visitor interface.
 * \ingroup SVG
 */
class T_DLLCLASS IShapeVisitor : public Object
{
	T_RTTI_CLASS;

public:
	virtual bool enter(Shape* shape) = 0;

	virtual void leave(Shape* shape) = 0;
};

}
