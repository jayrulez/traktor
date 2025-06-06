/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Render/Context/RenderContext.h"

#include "Core/Memory/Alloc.h"

#include <algorithm>

#if defined(_DEBUG)
#	include "Core/Log/Log.h"
#	include "Core/Misc/TString.h"
#endif

namespace traktor::render
{
namespace
{

const float c_distanceQuantizeRangeInv = 1.0f / 10.0f;

T_FORCE_INLINE bool SortOpaquePredicate(const DrawableRenderBlock* renderBlock1, const DrawableRenderBlock* renderBlock2)
{
// Don't sort front-to-back on iOS as it's a TDBR architecture thus
// we focus on minimizing state changes on the CPU instead.
#if !defined(__IOS__)
	const float d1 = std::floor(renderBlock1->distance * c_distanceQuantizeRangeInv);
	const float d2 = std::floor(renderBlock2->distance * c_distanceQuantizeRangeInv);
	if (d1 < d2)
		return true;
	else if (d1 > d2)
		return false;
#endif

	return renderBlock1->program < renderBlock2->program;
}

T_FORCE_INLINE bool SortAlphaBlendPredicate(const DrawableRenderBlock* renderBlock1, const DrawableRenderBlock* renderBlock2)
{
	return renderBlock1->distance > renderBlock2->distance;
}

}

T_IMPLEMENT_RTTI_CLASS(L"traktor.render.RenderContext", RenderContext, Object)

RenderContext::RenderContext(uint32_t heapSize)
	: m_heapEnd(nullptr)
	, m_heapPtr(nullptr)
{
	m_heap.reset(static_cast< uint8_t* >(Alloc::acquireAlign(heapSize, 16, T_FILE_LINE)));
	T_FATAL_ASSERT_M(m_heap.ptr(), L"Out of memory (Render context)");
	m_heapEnd = m_heap.ptr() + heapSize;
	m_heapPtr = m_heap.ptr();
}

RenderContext::~RenderContext()
{
	flush();
	m_heap.release();
}

void* RenderContext::alloc(uint32_t blockSize)
{
	if (m_heapPtr + blockSize >= m_heapEnd)
		T_FATAL_ERROR;

	void* ptr = m_heapPtr;
	m_heapPtr += blockSize;

	return ptr;
}

void* RenderContext::alloc(uint32_t blockSize, uint32_t align)
{
	T_ASSERT(align > 0);
	m_heapPtr = alignUp(m_heapPtr, align);
	return alloc(blockSize);
}

void RenderContext::compute(RenderBlock* renderBlock)
{
	m_computeQueue.push_back(renderBlock);
}

void RenderContext::draw(RenderBlock* renderBlock)
{
	m_drawQueue.push_back(renderBlock);
}

void RenderContext::draw(uint32_t type, DrawableRenderBlock* renderBlock)
{
	if (type == RenderPriority::Setup)
		m_priorityQueue[0].push_back(renderBlock);
	else if (type == RenderPriority::Opaque)
		m_priorityQueue[1].push_back(renderBlock);
	else if (type == RenderPriority::PostOpaque)
		m_priorityQueue[2].push_back(renderBlock);
	else if (type == RenderPriority::AlphaBlend)
		m_priorityQueue[3].push_back(renderBlock);
	else if (type == RenderPriority::PostAlphaBlend)
		m_priorityQueue[4].push_back(renderBlock);
	else if (type == RenderPriority::Overlay)
		m_priorityQueue[5].push_back(renderBlock);
}

void RenderContext::direct(RenderBlock* renderBlock)
{
	m_renderQueue.push_back(renderBlock);
}

void RenderContext::mergePriorityIntoDraw(uint32_t priorities)
{
	// Merge setup blocks unsorted.
	if (priorities & RenderPriority::Setup)
	{
		m_drawQueue.insert(m_drawQueue.end(), m_priorityQueue[0].begin(), m_priorityQueue[0].end());
		m_priorityQueue[0].resize(0);
	}

	// Merge opaque blocks, sorted by shader.
	if (priorities & RenderPriority::Opaque)
	{
		std::sort(m_priorityQueue[1].begin(), m_priorityQueue[1].end(), SortOpaquePredicate);
		m_drawQueue.insert(m_drawQueue.end(), m_priorityQueue[1].begin(), m_priorityQueue[1].end());
		m_priorityQueue[1].resize(0);
	}

	// Merge post opaque blocks, sorted by shader.
	if (priorities & RenderPriority::PostOpaque)
	{
		std::sort(m_priorityQueue[2].begin(), m_priorityQueue[2].end(), SortOpaquePredicate);
		m_drawQueue.insert(m_drawQueue.end(), m_priorityQueue[2].begin(), m_priorityQueue[2].end());
		m_priorityQueue[2].resize(0);
	}

	// Merge alpha blend blocks back to front.
	if (priorities & RenderPriority::AlphaBlend)
	{
		std::sort(m_priorityQueue[3].begin(), m_priorityQueue[3].end(), SortAlphaBlendPredicate);
		m_drawQueue.insert(m_drawQueue.end(), m_priorityQueue[3].begin(), m_priorityQueue[3].end());
		m_priorityQueue[3].resize(0);
	}

	// Merge post alpha blend blocks back to front.
	if (priorities & RenderPriority::PostAlphaBlend)
	{
		std::sort(m_priorityQueue[4].begin(), m_priorityQueue[4].end(), SortAlphaBlendPredicate);
		m_drawQueue.insert(m_drawQueue.end(), m_priorityQueue[4].begin(), m_priorityQueue[4].end());
		m_priorityQueue[4].resize(0);
	}

	// Merge overlay blocks unsorted.
	if (priorities & RenderPriority::Overlay)
	{
		m_drawQueue.insert(m_drawQueue.end(), m_priorityQueue[5].begin(), m_priorityQueue[5].end());
		m_priorityQueue[5].resize(0);
	}
}

void RenderContext::mergeComputeIntoRender()
{
	// Merge compute blocks into render queue.
	m_renderQueue.insert(m_renderQueue.end(), m_computeQueue.begin(), m_computeQueue.end());
	m_computeQueue.resize(0);
}

void RenderContext::mergeDrawIntoRender()
{
	// Merge draw blocks into render queue.
	m_renderQueue.insert(m_renderQueue.end(), m_drawQueue.begin(), m_drawQueue.end());
	m_drawQueue.resize(0);
}

void RenderContext::render(IRenderView* renderView) const
{
	// Execute all blocks in render queue.
	const size_t rsize = m_renderQueue.size();
	for (size_t i = 0; i < rsize; ++i)
		m_renderQueue[i]->render(renderView);
}

void RenderContext::flush()
{
	// Reset queues and heap.
	for (int32_t i = 0; i < sizeof_array(m_priorityQueue); ++i)
	{
		T_ASSERT(m_priorityQueue[i].empty());

		// As blocks are allocated from a fixed pool we need to manually call destructors.
		for (auto renderBlock : m_priorityQueue[i])
			renderBlock->~DrawableRenderBlock();

		m_priorityQueue[i].resize(0);
	}

	// As blocks are allocated from a fixed pool we need to manually call destructors.
	for (auto renderBlock : m_computeQueue)
		renderBlock->~RenderBlock();
	for (auto renderBlock : m_drawQueue)
		renderBlock->~RenderBlock();
	for (auto renderBlock : m_renderQueue)
		renderBlock->~RenderBlock();

	m_computeQueue.resize(0);
	m_drawQueue.resize(0);
	m_renderQueue.resize(0);

	m_heapPtr = m_heap.ptr();
}

bool RenderContext::havePendingComputes() const
{
	return !m_computeQueue.empty();
}

bool RenderContext::havePendingDraws() const
{
	for (int32_t i = 0; i < sizeof_array(m_priorityQueue); ++i)
		if (!m_priorityQueue[i].empty())
			return true;
	return false;
}

}
