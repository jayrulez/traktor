#pragma once

#include "Physics/JointDesc.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_PHYSICS_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor
{
	namespace physics
	{

/*! Degree-of-freedom joint description.
 * \ingroup Physics
 */
class T_DLLCLASS DofJointDesc : public JointDesc
{
	T_RTTI_CLASS;

public:
	struct Limit
	{
		bool x, y, z;
	};

	const Limit& getTranslate() const { return m_translate; }

	const Limit& getRotate() const { return m_rotate; }

	virtual void serialize(ISerializer& s) override final;

private:
	Limit m_translate = { false, false, false };
	Limit m_rotate = { false, false, false };
};

	}
}
