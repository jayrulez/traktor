/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Paper/Editor/UIPagePreviewControl.h"
#include "Ui/StyleBitmap.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.paper.UIPagePreviewControl", UIPagePreviewControl, ui::Widget)

bool UIPagePreviewControl::create(ui::Widget* parent)
{
	if (!ui::Widget::create(parent, ui::WsNone))
		return false;

	return true;
}

void UIPagePreviewControl::destroy()
{
	ui::Widget::destroy();
}

}
