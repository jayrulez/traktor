/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <algorithm>
#include "Core/Serialization/ISerializer.h"
#include "Core/Serialization/MemberAabb.h"
#include "Core/Serialization/MemberAlignedVector.h"
#include "Core/Serialization/MemberComposite.h"
#include "Spark/MorphShape.h"
#include "Spark/MorphShapeInstance.h"
#include "Spark/Swf/SwfTypes.h"

namespace traktor::spark
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.spark.MorphShape", 0, MorphShape, Character)

MorphShape::MorphShape()
{
	m_shapeBounds.mn.x = m_shapeBounds.mx.x =
	m_shapeBounds.mn.y = m_shapeBounds.mx.y = 0.0f;
}

bool MorphShape::create(const Aabb2& shapeBounds, const SwfShape* startShape, const SwfShape* endShape, const SwfStyles* startStyles, const SwfStyles* endStyles)
{
	uint16_t fillStyle0 = 0;
	uint16_t fillStyle1 = 0;
	uint16_t lineStyle = 0;
	uint32_t fillStyleBase = 0;
	uint32_t lineStyleBase = 0;

	m_fillStyles.resize(startStyles->numFillStyles);
	for (uint32_t i = 0; i < startStyles->numFillStyles; ++i)
		m_fillStyles[i].create(startStyles->fillStyles[i]);

	m_lineStyles.resize(startStyles->numLineStyles);
	for (uint32_t i = 0; i < startStyles->numLineStyles; ++i)
		m_lineStyles[i].create(startStyles->lineStyles[i]);

	Path path;
	for (SwfShapeRecord* shapeRecord = startShape->shapeRecords; shapeRecord; shapeRecord = shapeRecord->next)
	{
		if (shapeRecord->edgeFlag && shapeRecord->edge.straightFlag)
		{
			const SwfStraightEdgeRecord& s = shapeRecord->edge.straightEdge;
			if (s.generalLineFlag)
				path.lineTo(s.deltaX, s.deltaY, Path::CmRelative);
			else
			{
				if (!s.vertLineFlag)
					path.lineTo(s.deltaX, 0, Path::CmRelative);
				else
					path.lineTo(0, s.deltaY, Path::CmRelative);
			}
		}
		else if (shapeRecord->edgeFlag && !shapeRecord->edge.straightFlag)
		{
			const SwfCurvedEdgeRecord& c = shapeRecord->edge.curvedEdge;
			path.quadraticTo(
				c.controlDeltaX,
				c.controlDeltaY,
				c.controlDeltaX + c.anchorDeltaX,
				c.controlDeltaY + c.anchorDeltaY,
				Path::CmRelative
			);
		}
		else if (!shapeRecord->edgeFlag)
		{
			// Whenever a style records appear we close the current sub path.
			path.end(
				fillStyle0 ? fillStyle0 + fillStyleBase : 0,
				fillStyle1 ? fillStyle1 + fillStyleBase : 0,
				lineStyle ? lineStyle + lineStyleBase : 0
			);

			const SwfStyleRecord& s = shapeRecord->style;
			if (s.stateMoveTo)
			{
				path.moveTo(
					s.moveDeltaX,
					s.moveDeltaY,
					Path::CmAbsolute
				);
			}

			if (s.stateNewStyles)
			{
				fillStyleBase = uint32_t(m_fillStyles.size());
				lineStyleBase = uint32_t(m_lineStyles.size());

				m_fillStyles.resize(fillStyleBase + s.newStyles->numFillStyles);
				m_lineStyles.resize(lineStyleBase + s.newStyles->numLineStyles);

				for (int j = 0; j < s.newStyles->numFillStyles; ++j)
				{
					if (!m_fillStyles[fillStyleBase + j].create(s.newStyles->fillStyles[j]))
						return false;
				}
				for (int j = 0; j < s.newStyles->numLineStyles; ++j)
				{
					if (!m_lineStyles[lineStyleBase + j].create(s.newStyles->lineStyles[j]))
						return false;
				}

				fillStyle0 =
				fillStyle1 =
				lineStyle = 0;

				m_paths.push_back(path);
				path.reset();
			}

			if (s.stateFillStyle0)
				fillStyle0 = s.fillStyle0;
			if (s.stateFillStyle1)
				fillStyle1 = s.fillStyle1;
			if (s.stateLineStyle)
				lineStyle = s.lineStyle;
		}
	}

	path.end(
		fillStyle0 ? fillStyle0 + fillStyleBase : 0,
		fillStyle1 ? fillStyle1 + fillStyleBase : 0,
		lineStyle ? lineStyle + lineStyleBase : 0
	);
	m_paths.push_back(path);

	m_shapeBounds = shapeBounds;

	return true;
}

Ref< CharacterInstance > MorphShape::createInstance(
	Context* context,
	Dictionary* dictionary,
	CharacterInstance* parent,
	const std::string& name,
	const Matrix33& transform
) const
{
	return new MorphShapeInstance(context, dictionary, parent, this);
}

void MorphShape::serialize(ISerializer& s)
{
	Character::serialize(s);

	s >> MemberAabb2(L"shapeBounds", m_shapeBounds);
	s >> MemberAlignedVector< Path, MemberComposite< Path > >(L"paths", m_paths);
	s >> MemberAlignedVector< FillStyle, MemberComposite< FillStyle > >(L"fillStyles", m_fillStyles);
	s >> MemberAlignedVector< LineStyle, MemberComposite< LineStyle > >(L"lineStyles", m_lineStyles);
}

}
