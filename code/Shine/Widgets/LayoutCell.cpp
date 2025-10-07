/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/LayoutCell.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.LayoutCell", LayoutCell, UIElement)

LayoutCell::LayoutCell(const std::wstring& name)
:	UIElement(name)
{
}

LayoutCell::~LayoutCell()
{
}

}
