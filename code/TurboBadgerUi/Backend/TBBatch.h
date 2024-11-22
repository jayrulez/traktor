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

#include "TurboBadgerUi/Backend/TBVertex.h"

#include "Render/Types.h"

namespace traktor::render
{
    class ITexture;
}

namespace traktor::turbobadgerui
{
    struct TBBatch
    {
        Ref < render::ITexture > texture;
        AlignedVector<TBVertex> vertices;
        render::Rectangle clipRect;
    };
}