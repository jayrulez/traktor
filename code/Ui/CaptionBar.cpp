/*
 * TRAKTOR
 * Copyright (c) 2023-2025 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Ui/CaptionBar.h"

#include "Ui/Application.h"
#include "Ui/Form.h"
#include "Ui/MiniButton.h"
#include "Ui/Static.h"
#include "Ui/StyleBitmap.h"
#include "Ui/StyleSheet.h"

namespace traktor::ui
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.ui.CaptionBar", CaptionBar, ToolBar)

bool CaptionBar::create(Widget* parent, uint32_t style)
{
	if (!ToolBar::create(parent, style))
		return false;

#if !defined(__LINUX__)
	m_buttonMinimize = new MiniButton();
	m_buttonMinimize->create(this, new ui::StyleBitmap(L"UI.CaptionMinimize"), MiniButton::WsNoBorder | MiniButton::WsNoBackground);
	m_buttonMinimize->addEventHandler< ButtonClickEvent >(this, &CaptionBar::eventButtonClick);

	m_buttonMaximizeOrRestore = new MiniButton();
	m_buttonMaximizeOrRestore->create(this, new ui::StyleBitmap(L"UI.CaptionMaximize"), MiniButton::WsNoBorder | MiniButton::WsNoBackground);
	m_buttonMaximizeOrRestore->addEventHandler< ButtonClickEvent >(this, &CaptionBar::eventButtonClick);

	m_buttonClose = new MiniButton();
	m_buttonClose->create(this, new ui::StyleBitmap(L"UI.CaptionClose"), MiniButton::WsNoBorder | MiniButton::WsNoBackground);
	m_buttonClose->addEventHandler< ButtonClickEvent >(this, &CaptionBar::eventButtonClick);

	m_label = new Static();
	m_label->create(this, L"");

	addEventHandler< MouseButtonDownEvent >(this, &CaptionBar::eventMouseButtonDown);
	addEventHandler< MouseButtonUpEvent >(this, &CaptionBar::eventMouseButtonUp);
	addEventHandler< MouseDoubleClickEvent >(this, &CaptionBar::eventMouseDoubleClick);
	addEventHandler< MouseMoveEvent >(this, &CaptionBar::eventMouseMove);
	addEventHandler< SizeEvent >(this, &CaptionBar::eventSize);
#endif

	return true;
}

void CaptionBar::setText(const std::wstring& text)
{
	if (m_label)
		m_label->setText(text);
	ToolBar::setText(text);
	update();
}

void CaptionBar::update(const Rect* rc_, bool immediate)
{
	const Rect rc = getInnerRect();
	const int32_t pad = pixel(10_ut);

	int32_t r = rc.right - pad;
	int32_t h = rc.getHeight();

	if (m_buttonClose)
	{
		const Size sz = m_buttonClose->getPreferredSize(Size(0, 0));
		m_buttonClose->setRect({ Point(r - sz.cx, (h - sz.cy) / 2), Size(sz.cx, sz.cy) });
		r -= sz.cx + pad;
	}

	if (m_buttonMaximizeOrRestore)
	{
		const Form* parentForm = dynamic_type_cast< const Form* >(getAncestor());
		const bool maximized = (parentForm != nullptr) ? parentForm->isMaximized() : false;
		const Size sz = m_buttonMaximizeOrRestore->getPreferredSize(Size(0, 0));
		m_buttonMaximizeOrRestore->setRect({ Point(r - sz.cx, (h - sz.cy) / 2), Size(sz.cx, sz.cy) });
		m_buttonMaximizeOrRestore->setImage(new ui::StyleBitmap(maximized ? L"UI.CaptionRestore" : L"UI.CaptionMaximize"));
		r -= sz.cx + pad;
	}

	if (m_buttonMinimize)
	{
		const Size sz = m_buttonMinimize->getPreferredSize(Size(0, 0));
		m_buttonMinimize->setRect({ Point(r - sz.cx, (h - sz.cy) / 2), Size(sz.cx, sz.cy) });
		r -= sz.cx + pad;
	}

	if (m_label)
	{
		const Size sz = m_label->getPreferredSize(Size(0, 0));
		m_label->setRect({ Point(r - sz.cx, (h - sz.cy) / 2), Size(sz.cx, sz.cy) });
		r -= sz.cx + pad;
	}

	ToolBar::update(rc_, immediate);
}

Size CaptionBar::getPreferredSize(const Size& hint) const
{
	Size preferedSize = ToolBar::getPreferredSize(hint);
	if (getParent())
		preferedSize.cx = getParent()->getInnerRect().getWidth();
	return preferedSize;
}

void CaptionBar::eventButtonClick(ButtonClickEvent* event)
{
	Form* parentForm = dynamic_type_cast< Form* >(getParent());
	if (!parentForm)
		return;

	if (event->getSender() == m_buttonMinimize)
		parentForm->minimize();
	else if (event->getSender() == m_buttonMaximizeOrRestore)
	{
		if (!parentForm->isMaximized())
			parentForm->maximize();
		else
			parentForm->restore();
	}
	else if (event->getSender() == m_buttonClose)
	{
		CloseEvent closeEvent(parentForm);
		parentForm->raiseEvent(&closeEvent);
		if (!closeEvent.consumed() && !closeEvent.cancelled())
			parentForm->destroy();
	}
}

void CaptionBar::eventMouseButtonDown(MouseButtonDownEvent* event)
{
	// Check if user press on a toolbar item first; then don't move form,
	ToolBarItem* item = getItem(event->getPosition());
	if (item != nullptr)
		return;

	Form* parentForm = dynamic_type_cast< Form* >(getAncestor());
	if (!parentForm)
		return;

	if (parentForm->isMaximized())
		return;

	m_mousePosition = getParent()->clientToScreen(event->getPosition());
	m_parentRect = getParent()->getRect();
	m_haveCapture = true;
	setCapture();

	event->consume();
}

void CaptionBar::eventMouseButtonUp(MouseButtonUpEvent* event)
{
	if (m_haveCapture)
	{
		m_haveCapture = false;
		releaseCapture();
		event->consume();
	}
}

void CaptionBar::eventMouseDoubleClick(MouseDoubleClickEvent* event)
{
	Form* parentForm = dynamic_type_cast< Form* >(getAncestor());
	if (!parentForm)
		return;

	if (!parentForm->isMaximized())
		parentForm->maximize();
	else
		parentForm->restore();

	event->consume();
}

void CaptionBar::eventMouseMove(MouseMoveEvent* event)
{
	if (!m_haveCapture)
		return;

	Form* parentForm = dynamic_type_cast< Form* >(getAncestor());
	if (!parentForm)
		return;

	const Point position = parentForm->clientToScreen(event->getPosition());
	const Rect rc = m_parentRect.offset(position - m_mousePosition);
	parentForm->setRect(rc);

	event->consume();
}

void CaptionBar::eventSize(SizeEvent* event)
{
	update();
}

}
