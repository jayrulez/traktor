/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/Backend/TBFile.h"

#include "Core/Misc/TString.h"
#include "Core/Io/FileSystem.h"

namespace tb
{
	// static
	TBFile* tb::TBFile::Open(const char* filename, TBFileMode mode)
	{
		if (mode != TBFileMode::MODE_READ)
			return nullptr;

		traktor::turbobadgerui::TBUIFile* file = new ::traktor::turbobadgerui::TBUIFile(traktor::mbstows(filename), traktor::File::FmRead);
		if (!file->IsOpen())
		{
			file = nullptr;
		}

		return file;
	}
} // namespace tb

namespace traktor::turbobadgerui
{
	TBUIFile::TBUIFile(const std::wstring& filePath, File::Mode accessMode)
		: m_stream(nullptr)
	{
		m_stream = FileSystem::getInstance().open(filePath, accessMode);
	}

	TBUIFile::~TBUIFile()
	{
		if (m_stream)
			m_stream->close();
	}

	long TBUIFile::Size()
	{
		return m_stream->available();
	}

	size_t TBUIFile::Read(void* buf, size_t elemSize, size_t count)
	{
		return m_stream->read(buf, count);
	}
}