/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Runtime/Editor/HostEnumerator.h"
#include "Runtime/Editor/TargetInstance.h"
#include "Runtime/Editor/Ui/DropListCell.h"
#include "Ui/Application.h"
#include "Ui/Canvas.h"
#include "Ui/Command.h"
#include "Ui/Menu.h"
#include "Ui/MenuItem.h"
#include "Ui/StyleSheet.h"
#include "Ui/Auto/AutoWidget.h"

namespace traktor::runtime
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.runtime.DropListCell", DropListCell, ui::AutoWidgetCell)

DropListCell::DropListCell(HostEnumerator* hostEnumerator, TargetInstance* instance)
:	m_hostEnumerator(hostEnumerator)
,	m_instance(instance)
{
}

void DropListCell::mouseDown(ui::MouseButtonDownEvent* event, const ui::Point& position)
{
	std::wstring platformName = m_instance->getPlatformName();

	ui::Menu menu;
	int32_t count = m_hostEnumerator->count();
	for (int32_t i = 0; i < count; ++i)
	{
		if (m_hostEnumerator->supportPlatform(i, platformName))
		{
			const std::wstring& description = m_hostEnumerator->getDescription(i);
			menu.add(new ui::MenuItem(ui::Command(i), description));
		}
	}

	ui::Rect rcInner = getClientRect();
	const ui::MenuItem* selectedItem = menu.showModal(getWidget< ui::AutoWidget >(), rcInner.getBottomLeft(), rcInner.getWidth(), 8);
	if (selectedItem)
		m_instance->setDeployHostId(selectedItem->getCommand().getId());
}

void DropListCell::paint(ui::Canvas& canvas, const ui::Rect& rect)
{
	const ui::StyleSheet* ss = getStyleSheet();
	const ui::Rect& rcInner = rect;
	const ui::Point at = rcInner.getTopLeft();
	const ui::Size size = rcInner.getSize();
	const int32_t sep = pixel(14_ut);
	bool hover = false; //isEnable() && hasCapture();

	const ui::Rect rcText(
		at.x + pixel(4_ut),
		at.y + 2,
		at.x + size.cx - sep - 2,
		at.y + size.cy - 2
	);
	const ui::Rect rcButton(
		at.x + size.cx - sep,
		at.y + 1,
		at.x + size.cx - 1,
		at.y + size.cy - 1
	);

	canvas.setBackground(ss->getColor(this, hover ? L"background-color-hover" : L"background-color"));
	canvas.fillRect(rcInner);

	canvas.setBackground(ss->getColor(this, L"background-color-button"));
	canvas.fillRect(rcButton);

	if (hover)
	{
		canvas.setForeground(ss->getColor(this, L"color-hover"));
		canvas.drawRect(rcInner);
		canvas.drawLine(rcButton.left - 1, rcButton.top, rcButton.left - 1, rcButton.bottom);
	}

	const ui::Point center = rcButton.getCenter();
	const ui::Point pnts[] =
	{
		ui::Point(center.x - pixel(3_ut), center.y - pixel(1_ut)),
		ui::Point(center.x + pixel(2_ut), center.y - pixel(1_ut)),
		ui::Point(center.x - pixel(1_ut), center.y + pixel(2_ut))
	};

	canvas.setBackground(ss->getColor(this, L"color-arrow"));
	canvas.fillPolygon(pnts, 3);

	const int32_t id = m_instance->getDeployHostId();
	if (id >= 0)
	{
		const std::wstring& description = m_hostEnumerator->getDescription(id);
		canvas.setForeground(ss->getColor(this, /*isEnable() ?*/ L"color" /*: L"color-disabled"*/));
		canvas.setClipRect(rcText);
		canvas.drawText(rcText, description, ui::AnLeft, ui::AnCenter);
		canvas.resetClipRect();
	}
}

}
