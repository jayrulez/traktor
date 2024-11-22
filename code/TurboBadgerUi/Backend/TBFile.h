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

#include "tb_system.h"

namespace traktor::turbobadgerui
{
    class TBUIFile : public tb::TBFile
    {
    public:
        TBUIFile(const std::wstring& filepath, File::Mode accessMode);

        ~TBUIFile();

        long Size() override;
        size_t Read(void* buf, size_t elemSize, size_t count) override;

        inline bool IsOpen() const
        {
            return m_stream != nullptr;
        }

    private:
        Ref<IStream> m_stream = nullptr;
    };
}