/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Editor/Asset.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_PAPER_EDITOR_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::paper
{

class T_DLLCLASS BitmapFontAsset : public editor::Asset
{
	T_RTTI_CLASS;

public:
	void setSize(int32_t size) { m_size = size; }

	int32_t getSize() const { return m_size; }

	void setTextureSize(int32_t textureSize) { m_textureSize = textureSize; }

	int32_t getTextureSize() const { return m_textureSize; }

	virtual void serialize(ISerializer& s) override final;

private:
	int32_t m_size = 16;
	int32_t m_textureSize = 512;
};

}
