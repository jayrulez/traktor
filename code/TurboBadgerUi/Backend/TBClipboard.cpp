/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/Types.h"

#include "tb_system.h"

namespace tb
{

    // == TBClipboard =====================================

    TBStr clipboard; ///< Obviosly not a full implementation since it ignores the OS :)

    void TBClipboard::Empty()
    {
        clipboard.Clear();
    }

    bool TBClipboard::HasText()
    {
        return !clipboard.IsEmpty();
    }

    bool TBClipboard::SetText(const char* text)
    {
        return clipboard.Set(text);
    }

    bool TBClipboard::GetText(TBStr& text)
    {
        return text.Set(clipboard);
    }

} // namespace tb