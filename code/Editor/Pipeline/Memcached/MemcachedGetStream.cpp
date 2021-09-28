#include <algorithm>
#include <cstring>
#include <sstream>
#include "Core/Log/Log.h"
#include "Core/Misc/Split.h"
#include "Core/Misc/String.h"
#include "Core/Misc/TString.h"
#include "Core/Thread/Acquire.h"
#include "Editor/Pipeline/Memcached/MemcachedGetStream.h"
#include "Editor/Pipeline/Memcached/MemcachedPipelineCache.h"
#include "Editor/Pipeline/Memcached/MemcachedProto.h"

namespace traktor
{
	namespace editor
	{

T_IMPLEMENT_RTTI_CLASS(L"traktor.editor.MemcachedGetStream", MemcachedGetStream, IStream)

MemcachedGetStream::MemcachedGetStream(MemcachedPipelineCache* cache, MemcachedProto* proto, const std::string& key)
:	m_cache(cache)
,	m_proto(proto)
,	m_key(key)
,	m_in(0)
,	m_index(0)
{
}

bool MemcachedGetStream::requestEndBlock()
{
	if (!m_proto)
		return false;

	std::stringstream ss;
	std::string command;
	std::string reply;

	ss << "get " << m_key << ":END";

	command = ss.str();
	T_DEBUG(mbstows(command));

	if (!m_proto->sendCommand(command))
	{
		log::error << L"Unable to request cache block; unable to send command." << Endl;
		m_proto = nullptr;
		return false;
	}

	bool gotEndBlock = false;
	for (;;)
	{
		if (!m_proto->readReply(reply))
		{
			log::error << L"Unable to request cache block; unable to receive reply." << Endl;
			m_proto = nullptr;
			return false;
		}

		std::vector< std::string > args;
		Split< std::string >::any(reply, " ", args);

		if (args.empty())
		{
			log::error << L"Unable to request cache block; empty reply." << Endl;
			m_proto = nullptr;
			return false;
		}

		if (args[0] == "VALUE")
		{
			if (args.size() < 4)
			{
				log::error << L"Unable to request cache block; malformed reply." << Endl;
				m_proto = nullptr;
				return false;
			}

			uint32_t bytes = parseString< uint32_t >(args[3]);
			if (bytes != 1)
			{
				log::error << L"Unable to request cache block; invalid size, end block must be one byte." << Endl;
				m_proto = nullptr;
				return false;
			}

			uint8_t endData[] = { 0x00, 0x00, 0x00 };
			if (!m_proto->readData(endData, 1))
			{
				log::error << L"Unable to request cache block; unable to receive data." << Endl;
				m_proto = nullptr;
				return false;
			}

			if (endData[0] != 0x22)
				return false;

			gotEndBlock = true;
		}
		else
		{
			if (args[0] != "END")
				log::error << L"Unable to request cache block; server error " << mbstows(args[0]) << L"." << Endl;
			break;
		}
	}

	return gotEndBlock;
}

bool MemcachedGetStream::requestNextBlock()
{
	if (!m_proto)
		return false;

	std::stringstream ss;
	std::string command;
	std::string reply;

	ss << "get " << m_key << ":" << m_index;

	command = ss.str();
	T_DEBUG(mbstows(command));

	if (!m_proto->sendCommand(command))
	{
		log::error << L"Unable to request cache block; unable to send command." << Endl;
		m_proto = nullptr;
		return false;
	}

	m_in = 0;
	m_out = 0;

	for (;;)
	{
		if (!m_proto->readReply(reply))
		{
			log::error << L"Unable to request cache block; unable to receive reply." << Endl;
			m_proto = nullptr;
			return false;
		}

		std::vector< std::string > args;
		Split< std::string >::any(reply, " ", args);

		if (args.empty())
		{
			log::error << L"Unable to request cache block; empty reply." << Endl;
			m_proto = nullptr;
			return false;
		}

		if (args[0] == "VALUE")
		{
			if (args.size() < 4)
			{
				log::error << L"Unable to request cache block; malformed reply." << Endl;
				m_proto = nullptr;
				return false;
			}

			uint32_t bytes = parseString< uint32_t >(args[3]);
			T_DEBUG(L"\tbytes = " << bytes);

			uint32_t avail = MaxBlockSize - m_in;
			if (bytes > avail)
			{
				log::error << L"Unable to request cache block; data block too big." << Endl;
				m_proto = nullptr;
				return false;
			}

			if (!m_proto->readData(&m_block[m_in], bytes))
			{
				log::error << L"Unable to request cache block; unable to receive data." << Endl;
				m_proto = nullptr;
				return false;
			}

			m_in += bytes;
		}
		else
		{
			if (args[0] != "END")
				log::error << L"Unable to request cache block; server error " << mbstows(args[0]) << Endl;
			break;
		}
	}

	if (!m_in)
		return false;

	m_index++;
	return true;
}

void MemcachedGetStream::close()
{
	if (m_proto)
	{
		T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_cache->m_lock);
		m_cache->m_protos.push_back(m_proto);
		m_proto = nullptr;
	}
}

bool MemcachedGetStream::canRead() const
{
	return true;
}

bool MemcachedGetStream::canWrite() const
{
	return false;
}

bool MemcachedGetStream::canSeek() const
{
	return false;
}

int64_t MemcachedGetStream::tell() const
{
	return 0;
}

int64_t MemcachedGetStream::available() const
{
	return 0;
}

int64_t MemcachedGetStream::seek(SeekOriginType origin, int64_t offset)
{
	return 0;
}

int64_t MemcachedGetStream::read(void* block, int64_t nbytes)
{
	uint8_t* writePtr = static_cast< uint8_t* >(block);
	int64_t navail = nbytes;

	while (navail > 0)
	{
		if (m_in)
		{
			int32_t nget = std::min< int32_t >(navail, m_in);

			std::memcpy(writePtr, &m_block[m_out], nget);

			m_in -= nget;
			m_out += nget;

			navail -= nget;
			writePtr += nget;
		}
		else
		{
			if (!requestNextBlock())
				break;
		}
	}

	return nbytes - navail;
}

int64_t MemcachedGetStream::write(const void* block, int64_t nbytes)
{
	return 0;
}

void MemcachedGetStream::flush()
{
}

	}
}