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

class T_DLLCLASS UIPageAsset : public editor::Asset
{
	T_RTTI_CLASS;

public:
	void setWidth(int32_t width) { m_width = width; }

	int32_t getWidth() const { return m_width; }

	void setHeight(int32_t height) { m_height = height; }

	int32_t getHeight() const { return m_height; }

	virtual void serialize(ISerializer& s) override final;

private:
	int32_t m_width = 1920;
	int32_t m_height = 1080;
};

}
