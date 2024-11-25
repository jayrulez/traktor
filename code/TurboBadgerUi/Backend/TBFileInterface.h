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

#include "tb_file_interface.h"

namespace traktor::turbobadgerui
{
    class TBFileInterface : public tb::TBFileInterface
    {
    public:
        tb::TBFileHandle Open(const char* filename, TBFileMode mode) override;
        void Close(tb::TBFileHandle file) override;

        long Size(tb::TBFileHandle file) override;
        size_t Read(tb::TBFileHandle file, void* buf, size_t elemSize, size_t count) override;

        inline bool IsOpen(tb::TBFileHandle file) const
        {
            return m_openFiles.find(static_cast<size_t>(file)) != m_openFiles.end();
        }

    private:
        SmallMap<size_t, Ref<IStream>> m_openFiles;
    };
}