#pragma once

#include "Core/Containers/SmallMap.h"
#include "Core/Io/ChunkMemory.h"
#include "Core/Thread/Semaphore.h"
#include "Editor/IPipelineCache.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_EDITOR_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor
{
	namespace editor
	{

class T_DLLCLASS MemoryPipelineCache : public IPipelineCache
{
	T_RTTI_CLASS;

public:
	virtual bool create(const PropertyGroup* settings) override final;

	virtual void destroy() override final;

	virtual Ref< IStream > get(const Guid& guid, const PipelineDependencyHash& hash) override final;

	virtual Ref< IStream > put(const Guid& guid, const PipelineDependencyHash& hash) override final;

	virtual bool commit(const Guid& guid, const PipelineDependencyHash& hash) override final;

	virtual Ref< IStream > get(const Key& key) override final;

	virtual Ref< IStream > put(const Key& key) override final;
	
	virtual void getInformation(OutputStream& os) override final;

private:
	Semaphore m_lock;
	SmallMap< Guid, Ref< ChunkMemory > > m_pending;
	SmallMap< Guid, Ref< ChunkMemory > > m_committed;
};

	}
}
