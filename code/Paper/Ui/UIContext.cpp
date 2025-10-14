/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Paper/Ui/UIContext.h"
#include "Paper/Draw2D.h"
#include "Paper/FontManager.h"
#include "Paper/IFontRenderer.h"

namespace traktor::paper
{

UIContext::UIContext(Draw2D* renderer, FontManager* fontManager, IFontRenderer* fontRenderer)
	: m_renderer(renderer)
	, m_fontManager(fontManager)
	, m_fontRenderer(fontRenderer)
{
}

}
