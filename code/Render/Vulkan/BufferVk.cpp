#include "Render/Vulkan/BufferVk.h"

namespace traktor
{
	namespace render
	{
	
T_IMPLEMENT_RTTI_CLASS(L"traktor.render.BufferVk", BufferVk, Buffer)

BufferVk::BufferVk(Context* context, uint32_t bufferSize, uint32_t& instances)
:	Buffer(bufferSize)
,	m_context(context)
,	m_instances(instances)
{
	Atomic::increment((int32_t&)m_instances);
}

BufferVk::~BufferVk()
{
	Atomic::decrement((int32_t&)m_instances);
}

	}
}