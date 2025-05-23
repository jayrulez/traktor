/*
 * TRAKTOR
 * Copyright (c) 2022-2025 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Math/Aabb3.h"
#include "Core/Object.h"
#include "World/WorldTypes.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_WORLD_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::world
{

class Entity;
class World;

/*! Entity component.
 * \ingroup World
 */
class T_DLLCLASS IEntityComponent : public Object
{
	T_RTTI_CLASS;

public:
	/*! Destroy the component. */
	virtual void destroy() = 0;

	/*! Set owner entity of component.
	 * \param owner New component owner.
	 */
	virtual void setOwner(Entity* owner) = 0;

	/*! Called when entity's world is set.
	 * \param world New world.
	 */
	virtual void setWorld(World* world) {}

	/*! Called when entity's state is modified.
	 * \param state Entity state.
	 * \param mask Mask of which states is being modified.
	 * \param includeChildren If modification should continue with children, any component which holds other entities should recurse with modified state.
	 */
	virtual void setState(const EntityState& state, const EntityState& mask, bool includeChildren) {}

	/*! Called when entity's transform is modified.
	 * \param transform New transform.
	 */
	virtual void setTransform(const Transform& transform) = 0;

	/*! Calculate bounding box of this component.
	 * \return Bounding box.
	 */
	virtual Aabb3 getBoundingBox() const = 0;

	/*! Update component
	 * \param update Update information.
	 */
	virtual void update(const UpdateParams& update) = 0;
};

}
