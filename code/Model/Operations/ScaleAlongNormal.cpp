/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Model/Model.h"
#include "Model/Operations/ScaleAlongNormal.h"

namespace traktor
{
	namespace model
	{

T_IMPLEMENT_RTTI_CLASS(L"traktor.model.ScaleAlongNormal", ScaleAlongNormal, IModelOperation)

ScaleAlongNormal::ScaleAlongNormal(float distance)
:	m_distance(distance)
{
}

bool ScaleAlongNormal::apply(Model& model) const
{
	AlignedVector< Vector4 > positions = model.getPositions();

	for (uint32_t i = 0; i < model.getVertexCount(); ++i)
	{
		const Vertex& vertex = model.getVertex(i);
		if (vertex.getPosition() == c_InvalidIndex || vertex.getNormal() == c_InvalidIndex)
			continue;

		const Vector4& normal = model.getNormal(vertex.getNormal());
		positions[vertex.getPosition()] += normal * Scalar(m_distance);
	}

	model.setPositions(positions);
	return true;
}

	}
}
