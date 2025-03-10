/*
 * TRAKTOR
 * Copyright (c) 2022-2023 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "I18N/Format.h"
#include "I18N/Text.h"
#include "Sound/Resound/RepeatGrainData.h"
#include "Sound/Editor/Resound/RepeatGrainFacade.h"
#include "Ui/StyleBitmap.h"

namespace traktor::sound
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.sound.RepeatGrainFacade", RepeatGrainFacade, IGrainFacade)

RepeatGrainFacade::RepeatGrainFacade()
{
	m_image = new ui::StyleBitmap(L"Sound.RepeatGrain");
}

ui::Widget* RepeatGrainFacade::createView(IGrainData* grain, ui::Widget* parent)
{
	return nullptr;
}

ui::StyleBitmap* RepeatGrainFacade::getImage(const IGrainData* grain) const
{
	return m_image;
}

std::wstring RepeatGrainFacade::getText(const IGrainData* grain) const
{
	const RepeatGrainData* repeatGrain = static_cast< const RepeatGrainData* >(grain);
	if (repeatGrain->getCount() != 0)
		return i18n::Format(L"RESOUND_REPEAT_GRAIN_TEXT", int32_t(repeatGrain->getCount()));
	else
		return i18n::Text(L"RESOUND_REPEAT_GRAIN_INFINITE_TEXT");
}

bool RepeatGrainFacade::getProperties(const IGrainData* grain, std::set< std::wstring >& outProperties) const
{
	return false;
}

bool RepeatGrainFacade::canHaveChildren() const
{
	return false;
}

bool RepeatGrainFacade::addChild(IGrainData* parentGrain, IGrainData* childGrain)
{
	return false;
}

bool RepeatGrainFacade::removeChild(IGrainData* parentGrain, IGrainData* childGrain)
{
	return false;
}

bool RepeatGrainFacade::getChildren(IGrainData* grain, RefArray< IGrainData >& outChildren)
{
	RepeatGrainData* repeatGrain = mandatory_non_null_type_cast< RepeatGrainData* >(grain);
	outChildren.push_back(repeatGrain->getGrain());
	return true;
}

}
