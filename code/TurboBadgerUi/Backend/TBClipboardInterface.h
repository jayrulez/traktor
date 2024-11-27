/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "TurboBadgerUi/Types.h"

#include "platform/tb_clipboard_interface.h"

namespace traktor::turbobadgerui
{
    class TBClipboardInterface : public tb::TBClipboardInterface
    {
    public:
		void Empty()  override;
		bool HasText()  override;
		bool SetText(const char* text)  override;
        bool GetText(tb::TBStr& text)  override;

    private:
        tb::TBStr m_clipboard;
    };
}