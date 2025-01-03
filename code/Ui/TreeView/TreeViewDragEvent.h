/*
 * TRAKTOR
 * Copyright (c) 2022-2023 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Ui/Events/DragEvent.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_UI_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::ui
{

class TreeViewItem;

/*! Tree view drag event.
 * \ingroup UI
 */
class T_DLLCLASS TreeViewDragEvent : public DragEvent
{
	T_RTTI_CLASS;

public:
	explicit TreeViewDragEvent(EventSubject* sender, TreeViewItem* dragItem, Moment moment, const Point& position = Point(0, 0));

	TreeViewItem* getItem() const;

private:
	Ref< TreeViewItem > m_dragItem;
};

}
