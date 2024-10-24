/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Io/IStream.h"
#include "Core/Log/Log.h"
#include "RmlUi/RmlDocument.h"
#include "RmlUi/RmlDocumentFactory.h"

namespace traktor::rmlui
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.rmlui.RmlDocumentFactory", RmlDocumentFactory, Object)

RmlDocumentFactory::RmlDocumentFactory()
{
}

Ref< RmlDocument > RmlDocumentFactory::createRmlDocument(const traktor::Path& assetPath, const db::Instance* sourceInstance, IStream* sourceStream) const
{
	// Create new document.
	Ref< RmlDocument > rmlDocument = new RmlDocument();

	return rmlDocument;
}

}
