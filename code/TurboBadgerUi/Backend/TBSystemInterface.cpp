#include "TurboBadgerUi/Backend/TBSystemInterface.h"

#include "Core/Log/Log.h"
#include "Core/Misc/TString.h"

namespace traktor::turbobadgerui
{
	void TBSystemInterface::DebugOut(const char* str)
	{
		traktor::log::debug << traktor::mbstows(str) << traktor::Endl;
	}

    double TBSystemInterface::GetTimeMS()
    {
        traktor::DateTime now = traktor::DateTime::now();
        return (double)now.getSecondsSinceEpoch() * 1000;
    }

    void TBSystemInterface::RescheduleTimer(double fire_time)
    {
    }

    int TBSystemInterface::GetLongClickDelayMS()
    {
        return 500;
    }

    int TBSystemInterface::GetPanThreshold()
    {
        return 40;
    }

    int TBSystemInterface::GetPixelsPerLine()
    {
        return 40;
    }

    int TBSystemInterface::GetDPI()
    {
        return 96;
    }
}
