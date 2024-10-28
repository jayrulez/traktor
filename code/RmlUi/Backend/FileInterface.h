/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Config.h"
#include "Core/Ref.h"
#include "Core/Object.h"
#include "Core/Containers/SmallMap.h"
#include "Core/Io/IStream.h"
#include "RmlUi/Core/FileInterface.h"

 // import/export mechanism.
#undef T_DLLCLASS
#if defined(T_RMLUI_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::rmlui
{
	class T_DLLCLASS FileInterface : public Rml::FileInterface
	{
	public:

		virtual Rml::FileHandle Open(const Rml::String& path) override;

		virtual void Close(Rml::FileHandle file) override;

		virtual size_t Read(void* buffer, size_t size, Rml::FileHandle file) override;

		virtual bool Seek(Rml::FileHandle file, long offset, int origin) override;

		virtual size_t Tell(Rml::FileHandle file) override;

	private:
		SmallMap<size_t, Ref<IStream>> m_openFiles;
	};

}