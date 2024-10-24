/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Core/Rtti/ITypedObject.h"
#include "Core/Io/IStream.h"
#include "Core/Io/FileSystem.h"
#include "Core/Misc/TString.h"
#include "RmlUi/Backend/FileInterface.h"

namespace traktor::rmlui
{
	namespace
	{
		size_t allocateFileId()
		{
			static size_t s_fileId = 0;
			return ++s_fileId;
		}
	}

	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.rmlui.FileInterface", 0, FileInterface, Object)

		Rml::FileHandle FileInterface::Open(const Rml::String& path)
	{
		Ref< IStream > stream = FileSystem::getInstance().open(Path(mbstows(path)), File::FmRead);
		size_t fileId = allocateFileId();
		m_openFiles.insert(fileId, stream);
		return static_cast<Rml::FileHandle>(fileId);
	}

	void FileInterface::Close(Rml::FileHandle file)
	{
		size_t fileId = static_cast<size_t>(file);

		if (m_openFiles.find(fileId) != m_openFiles.end())
		{
			m_openFiles[fileId]->close();
			m_openFiles.remove(fileId);
		}
	}

	size_t FileInterface::Read(void* buffer, size_t size, Rml::FileHandle file)
	{
		size_t fileId = static_cast<size_t>(file);

		if (m_openFiles.find(fileId) != m_openFiles.end())
		{
			return m_openFiles[fileId]->read(buffer, size);
		}

		return 0;
	}

	bool FileInterface::Seek(Rml::FileHandle file, long offset, int origin)
	{
		size_t fileId = static_cast<size_t>(file);

		if (m_openFiles.find(fileId) != m_openFiles.end())
		{
			if (!m_openFiles[fileId]->canSeek())
			{
				return false;
			}

			if (origin == SEEK_SET)
			{
				m_openFiles[fileId]->seek(IStream::SeekOriginType::SeekSet, offset);
				return true;
			}
			else if (origin == SEEK_CUR)
			{
				m_openFiles[fileId]->seek(IStream::SeekOriginType::SeekCurrent, offset);
				return true;
			}
			else if (origin == SEEK_END)
			{
				m_openFiles[fileId]->seek(IStream::SeekOriginType::SeekEnd, offset);
				return true;
			}
			else
			{
				return false;
			}
		}

		return false;
	}

	size_t FileInterface::Tell(Rml::FileHandle file)
	{
		size_t fileId = static_cast<size_t>(file);

		if (m_openFiles.find(fileId) != m_openFiles.end())
		{
			return m_openFiles[fileId]->tell();
		}

		return -1;
	}
}