/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

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

class Draw2D;
class FontManager;
class IFontRenderer;

/*! UI Context.
 * \ingroup Paper
 *
 * Bundles services needed by UI elements for layout and rendering.
 */
class T_DLLCLASS UIContext
{
public:
	UIContext(Draw2D* renderer, FontManager* fontManager, IFontRenderer* fontRenderer);

	Draw2D* getRenderer() const { return m_renderer; }

	FontManager* getFontManager() const { return m_fontManager; }

	IFontRenderer* getFontRenderer() const { return m_fontRenderer; }

private:
	Ref< Draw2D > m_renderer;
	Ref< FontManager > m_fontManager;
	Ref< IFontRenderer > m_fontRenderer;
};

}
