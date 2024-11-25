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

#include "Core/Ref.h"
#include "Core/Io/IStream.h"
#include "Core/Io/File.h"

#include "tb_system_interface.h"

namespace traktor::turbobadgerui
{
    class TBSystemInterface : public tb::TBSystemInterface
    {
    public:
        void DebugOut(const char* str) override;

		double GetTimeMS() override;
		void RescheduleTimer(double fire_time) override;
		int GetLongClickDelayMS() override;
		int GetPanThreshold() override;
        int GetPixelsPerLine() override;
        int GetDPI() override;
    };
}