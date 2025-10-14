/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Serialization/ISerializable.h"
#include "Core/Ref.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_PAPER_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::paper
{

class UIElement;

/*! UI Page definition.
 * \ingroup Paper
 *
 * A UIPage is a container that holds the root UIElement of a UI hierarchy.
 * It can be serialized and loaded from assets.
 */
class T_DLLCLASS UIPage : public ISerializable
{
	T_RTTI_CLASS;

public:
	UIPage() = default;

	void setRoot(UIElement* root) { m_root = root; }

	UIElement* getRoot() const { return m_root; }

	virtual void serialize(ISerializer& s) override final;

private:
	Ref< UIElement > m_root;
};

}
