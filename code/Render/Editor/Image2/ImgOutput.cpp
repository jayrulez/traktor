/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Render/Editor/Image2/ImgOutput.h"

namespace traktor::render
{
	namespace
	{

const ImmutableNode::InputPinDesc c_ImgOutput_i[] = { { L"Input", L"{C603AA17-9EFC-42F9-9E12-41D08FB7AC4E}", false }, { 0 } };

	}

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.render.ImgOutput", 0, ImgOutput, ImmutableNode)

ImgOutput::ImgOutput()
:	ImmutableNode(c_ImgOutput_i, nullptr)
{
}

void ImgOutput::serialize(ISerializer& s)
{
	Node::serialize(s);
}

}
