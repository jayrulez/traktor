/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Render/Vrfy/VertexLayoutVrfy.h"

namespace traktor::render
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.render.VertexLayoutVrfy", VertexLayoutVrfy, IVertexLayout)

VertexLayoutVrfy::VertexLayoutVrfy(const IVertexLayout* wrappedVertexLayout, uint32_t vertexSize)
:	m_wrappedVertexLayout(wrappedVertexLayout)
,	m_vertexSize(vertexSize)
{
}

}
