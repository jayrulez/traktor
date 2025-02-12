/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Ui/Auto/AutoWidgetCell.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_UI_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::ui
{

class IBitmap;
class StyleBitmap;

class T_DLLCLASS PreviewItem : public AutoWidgetCell
{
	T_RTTI_CLASS;

public:
	PreviewItem();

	explicit PreviewItem(const std::wstring& text);

	void setText(const std::wstring& text);

	const std::wstring& getText() const;

	void setSubText(const std::wstring& subText);

	const std::wstring& getSubText() const;

	void setImage(ui::IBitmap* image);

	ui::IBitmap* getImage() const;

	void setSelected(bool selected);

	bool isSelected() const;

	const Rect& getTextRect() const { return m_textRect; }

	virtual void interval() override final;

	virtual void mouseDown(MouseButtonDownEvent* event, const Point& position) override final;

	virtual void mouseUp(MouseButtonUpEvent* event, const Point& position) override final;

	virtual void mouseDoubleClick(MouseDoubleClickEvent* event, const Point& position) override final;

	virtual void paint(Canvas& canvas, const Rect& rect) override final;

private:
	std::wstring m_text;
	std::wstring m_subText;
	Ref< ui::StyleBitmap > m_imageBackground;
	Ref< ui::IBitmap > m_bitmapImage;
	Rect m_textRect;
	bool m_selected = false;
	bool m_editable = true;
	int32_t m_editMode = 0;
};

}
