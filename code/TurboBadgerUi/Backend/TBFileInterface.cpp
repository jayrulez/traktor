/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/Backend/TBFileInterface.h"

#include "Core/Misc/TString.h"
#include "Core/Io/FileSystem.h"

namespace
{
	size_t allocateFileId()
	{
		static size_t s_fileId = 0;
		return ++s_fileId;
	}
}

namespace traktor::turbobadgerui
{
	tb::TBFileHandle TBFileInterface::Open(const char* filename, TBFileMode mode)
	{
		if (mode != TBFileMode::MODE_READ)
			return 0;

		Ref< IStream > stream = FileSystem::getInstance().open(Path(mbstows(filename)), File::FmRead);
		if (!stream)
			return 0;

		size_t fileId = allocateFileId();
		m_openFiles.insert(fileId, stream);
		return static_cast<tb::TBFileHandle>(fileId);
	}

	void TBFileInterface::Close(tb::TBFileHandle file)
	{
		size_t fileId = static_cast<size_t>(file);

		if (m_openFiles.find(fileId) != m_openFiles.end())
		{
			m_openFiles[fileId]->close();
			m_openFiles.remove(fileId);
		}
	}

	long TBFileInterface::Size(tb::TBFileHandle file)
	{
		size_t fileId = static_cast<size_t>(file);

		if (m_openFiles.find(fileId) != m_openFiles.end())
		{
			return m_openFiles[fileId]->available();
		}

		return 0;
	}

	size_t TBFileInterface::Read(tb::TBFileHandle file, void* buf, size_t elemSize, size_t count)
	{
		size_t fileId = static_cast<size_t>(file);

		if (m_openFiles.find(fileId) != m_openFiles.end())
		{
			return m_openFiles[fileId]->read(buf, count);
		}

		return 0;
	}
}