#pragma once

#include "Svg/Path.h"
#include "Svg/Shape.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SVG_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor
{
	namespace svg
	{

/*! SVG text shape.
 * \ingroup SVG
 */
class T_DLLCLASS TextShape : public Shape
{
	T_RTTI_CLASS;

public:
	explicit TextShape(const Vector2& position);

	const Vector2& getPosition() const;

private:
	Vector2 m_position;
};

	}
}