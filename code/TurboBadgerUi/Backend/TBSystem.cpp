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

#include "Core/Date/DateTime.h"
#include "Core/Log/Log.h"
#include "Core/Misc/TString.h"

#ifdef TB_RUNTIME_DEBUG_INFO

void TBDebugOut(const char* str)
{
    traktor::log::debug << traktor::mbstows(str) << traktor::Endl;
}

#endif // TB_RUNTIME_DEBUG_INFO

namespace tb
{
    /** Get the system time in milliseconds since some undefined epoch. */
    double TBSystem::GetTimeMS()
    {
        traktor::DateTime now = traktor::DateTime::now();
        return (double)now.getSecondsSinceEpoch() * 1000;
    }

    /** Called when the need to call TBMessageHandler::ProcessMessages has changed due to changes in the
      message queue. fire_time is the new time is needs to be called.
      It may be 0 which means that ProcessMessages should be called asap (but NOT from this call!)
      It may also be TB_NOT_SOON which means that ProcessMessages doesn't need to be called. */
    void TBSystem::RescheduleTimer(double fire_time)
    {
    }

    /** Get how many milliseconds it should take after a touch down event should generate a long click
      event. */
    int TBSystem::GetLongClickDelayMS()
    {
        return 500;
    }

    /** Get how many pixels of dragging should start panning scrollable widgets. */
    int TBSystem::GetPanThreshold()
    {
        return 40;
    }

    /** Get how many pixels a typical line is: The length that should be scrolled when turning a mouse
      wheel one notch. */
    int TBSystem::GetPixelsPerLine()
    {
        return 40;
    }

    /** Get Dots Per Inch for the main screen. */
    int TBSystem::GetDPI()
    {
        return 96;
    }
} // namespace tb