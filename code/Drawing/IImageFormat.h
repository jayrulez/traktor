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
#include "Core/Ref.h"
#include "Core/Io/Path.h"
#include "Core/Io/IStream.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_DRAWING_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::drawing
{

class Image;

/*! Image file format base class.
 * \ingroup Drawing
 */
class T_DLLCLASS IImageFormat : public Object
{
	T_RTTI_CLASS;

public:
	static Ref< IImageFormat > determineFormat(const std::wstring& extension);

	static Ref< IImageFormat > determineFormat(const Path& fileName);

	virtual Ref< Image > read(IStream* stream) = 0;

	virtual bool write(IStream* stream, const Image* image) = 0;
};

}
