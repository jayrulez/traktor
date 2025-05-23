/*
 * TRAKTOR
 * Copyright (c) 2022-2025 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Render/Frame/RenderGraph.h"

#include "Core/Containers/StaticSet.h"
#include "Core/Log/Log.h"
#include "Core/Misc/SafeDestroy.h"
#include "Core/Misc/String.h"
#include "Core/Timer/Profiler.h"
#include "Render/Context/RenderBlock.h"
#include "Render/Context/RenderContext.h"
#include "Render/Frame/RenderGraphBufferPool.h"
#include "Render/Frame/RenderGraphTargetSet.h"
#include "Render/Frame/RenderGraphTargetSetPool.h"
#include "Render/Frame/RenderGraphTexturePool.h"
#include "Render/IRenderSystem.h"
#include "Render/IRenderTargetSet.h"
#include "Render/IRenderView.h"

namespace traktor::render
{
namespace
{

void traverse(const RefArray< const RenderPass >& passes, int32_t depth, int32_t index, StaticVector< uint32_t, 512 >& chain, const std::function< void(int32_t, int32_t) >& fn)
{
	// Check if we're in a cyclic path.
	if (std::find(chain.begin(), chain.end(), index) != chain.end())
		return;

	// Traverse inputs first as we want to traverse passes depth-first.
	chain.push_back(index);
	for (const auto& input : passes[index]->getInputs())
	{
		for (int32_t i = 0; i < passes.size(); ++i)
			if (passes[i]->getOutput().resourceId == input.resourceId)
				traverse(passes, depth + 1, i, chain, fn);
	}
	chain.pop_back();

	// Call visitor for this pass.
	fn(depth, index);
}

}

T_IMPLEMENT_RTTI_CLASS(L"traktor.render.RenderGraph", RenderGraph, Object)

RenderGraph::RenderGraph(
	IRenderSystem* renderSystem,
	uint32_t multiSample,
	const fn_profiler_t& profiler)
	: m_targetSetPool(new RenderGraphTargetSetPool(renderSystem))
	, m_bufferPool(new RenderGraphBufferPool(renderSystem))
	, m_texturePool(new RenderGraphTexturePool(renderSystem))
	, m_counter(0)
	, m_multiSample(multiSample)
	, m_nextResourceId(1)
	, m_profiler(profiler)
{
}

RenderGraph::~RenderGraph()
{
	T_FATAL_ASSERT_M(m_targetSetPool == nullptr, L"Forgot to destroy RenderGraph instance.");
	T_FATAL_ASSERT_M(m_bufferPool == nullptr, L"Forgot to destroy RenderGraph instance.");
	T_FATAL_ASSERT_M(m_texturePool == nullptr, L"Forgot to destroy RenderGraph instance.");
}

void RenderGraph::destroy()
{
	m_targets.clear();
	m_buffers.clear();
	m_passes.clear();
	for (int32_t i = 0; i < sizeof_array(m_order); ++i)
		m_order[i].clear();
	safeDestroy(m_targetSetPool);
	safeDestroy(m_bufferPool);
	safeDestroy(m_texturePool);
}

RGTargetSet RenderGraph::addExplicitTargetSet(const wchar_t* const name, IRenderTargetSet* targetSet)
{
	const RGTargetSet resourceId(m_nextResourceId++);

	auto& tr = m_targets[resourceId];
	tr.name = name;
	tr.persistentHandle = 0;
	tr.doubleBuffered = false;
	tr.targetSet = new RenderGraphTargetSet(targetSet, targetSet);
	tr.sizeReferenceTargetSetId = RGTargetSet::Invalid;
	tr.inputRefCount = 0;
	tr.outputRefCount = 0;
	tr.external = true;

	return resourceId;
}

RGTargetSet RenderGraph::addTransientTargetSet(
	const wchar_t* const name,
	const RenderGraphTargetSetDesc& targetSetDesc,
	RGTargetSet sharedDepthStencilTargetSetId,
	RGTargetSet sizeReferenceTargetSetId)
{
	const RGTargetSet resourceId(m_nextResourceId++);

	auto& tr = m_targets[resourceId];
	tr.name = name;
	tr.persistentHandle = 0;
	tr.doubleBuffered = false;
	tr.targetSetDesc = targetSetDesc;
	tr.sharedDepthStencilTargetSetId = sharedDepthStencilTargetSetId;
	tr.sizeReferenceTargetSetId = sizeReferenceTargetSetId;
	tr.inputRefCount = 0;
	tr.outputRefCount = 0;
	tr.external = false;

	return resourceId;
}

RGTargetSet RenderGraph::addPersistentTargetSet(
	const wchar_t* const name,
	handle_t persistentHandle,
	bool doubleBuffered,
	const RenderGraphTargetSetDesc& targetSetDesc,
	RGTargetSet sharedDepthStencilTargetSetId,
	RGTargetSet sizeReferenceTargetSetId)
{
	const RGTargetSet resourceId(m_nextResourceId++);

	auto& tr = m_targets[resourceId];
	tr.name = name;
	tr.persistentHandle = persistentHandle;
	tr.doubleBuffered = doubleBuffered;
	tr.targetSetDesc = targetSetDesc;
	tr.sharedDepthStencilTargetSetId = sharedDepthStencilTargetSetId;
	tr.sizeReferenceTargetSetId = sizeReferenceTargetSetId;
	tr.inputRefCount = 0;
	tr.outputRefCount = 0;
	tr.external = false;

	return resourceId;
}

RGBuffer RenderGraph::addExplicitBuffer(const wchar_t* const name, Buffer* buffer)
{
	const RGBuffer resourceId(m_nextResourceId++);

	auto& br = m_buffers[resourceId];
	br.name = name;
	br.buffer = buffer;

	return resourceId;
}

RGBuffer RenderGraph::addTransientBuffer(const wchar_t* const name, const RenderGraphBufferDesc& bufferDesc)
{
	const RGBuffer resourceId(m_nextResourceId++);

	auto& br = m_buffers[resourceId];
	br.name = name;
	br.bufferDesc = bufferDesc;

	return resourceId;
}

RGBuffer RenderGraph::addPersistentBuffer(const wchar_t* const name, handle_t persistentHandle, const RenderGraphBufferDesc& bufferDesc)
{
	const RGBuffer resourceId(m_nextResourceId++);

	auto& br = m_buffers[resourceId];
	br.name = name;
	br.persistentHandle = persistentHandle;
	br.bufferDesc = bufferDesc;

	return resourceId;
}

RGTexture RenderGraph::addTransientTexture(const wchar_t* const name, const RenderGraphTextureDesc& textureDesc)
{
	const RGTexture resourceId(m_nextResourceId++);

	auto& tr = m_textures[resourceId];
	tr.name = name;
	tr.textureDesc = textureDesc;

	return resourceId;
}

RGTexture RenderGraph::addPersistentTexture(const wchar_t* const name, handle_t persistentHandle, const RenderGraphTextureDesc& textureDesc)
{
	const RGTexture resourceId(m_nextResourceId++);

	auto& tr = m_textures[resourceId];
	tr.name = name;
	tr.persistentHandle = persistentHandle;
	tr.textureDesc = textureDesc;

	return resourceId;
}

RGDependency RenderGraph::addDependency()
{
	return RGDependency(m_nextResourceId++);
}

IRenderTargetSet* RenderGraph::getTargetSet(RGTargetSet resource) const
{
	T_FATAL_ASSERT(m_buildingPasses);

	const auto it = m_targets.find(resource);
	if (it == m_targets.end() || it->second.targetSet == nullptr)
		return nullptr;

	return it->second.targetSet->getReadTargetSet();
}

Buffer* RenderGraph::getBuffer(RGBuffer resource) const
{
	T_FATAL_ASSERT(m_buildingPasses);
	const auto it = m_buffers.find(resource);
	return (it != m_buffers.end()) ? it->second.buffer : nullptr;
}

ITexture* RenderGraph::getTexture(RGTexture resource) const
{
	T_FATAL_ASSERT(m_buildingPasses);
	const auto it = m_textures.find(resource);
	return (it != m_textures.end()) ? it->second.texture : nullptr;
}

void RenderGraph::addPass(const RenderPass* pass)
{
	m_passes.push_back(pass);
}

bool RenderGraph::validate()
{
	// Find root passes, which are either of:
	// 1) Have no output resource.
	// 2) Writing to external targets.
	// 3) Writing to primary target.
	StaticVector< uint32_t, 32 > roots;
	for (uint32_t i = 0; i < (uint32_t)m_passes.size(); ++i)
	{
		const auto pass = m_passes[i];
		const auto& output = pass->getOutput();
		if (output.resourceId == ~0)
			roots.push_back(i);
		else if (output.resourceId != ~0 && output.resourceId != 0)
		{
			auto it = m_targets.find(RGTargetSet(output.resourceId));
			if (it->second.external)
				roots.push_back(i);
		}
		else if (output.resourceId == 0)
			roots.push_back(i);
	}

	// Determine maximum depths of each pass.
	StaticVector< int32_t, 512 > depths;
	depths.resize(m_passes.size(), -1);
	for (auto root : roots)
	{
		StaticVector< uint32_t, 512 > chain;
		traverse(m_passes, 0, root, chain, [&](int32_t depth, int32_t index) {
			T_ASSERT(depth < sizeof_array(m_order));
			depths[index] = std::max(depths[index], depth);
		});
	}

	// Gather passes in order for each depth.
	for (int32_t i = 0; i < sizeof_array(m_order); ++i)
		m_order[i].resize(0);
	for (uint32_t i = 0; i < (uint32_t)m_passes.size(); ++i)
		if (depths[i] >= 0)
			m_order[depths[i]].push_back(i);

	// Sort each depth based on output resource.
	for (int32_t i = 0; i < sizeof_array(m_order); ++i)
		std::stable_sort(m_order[i].begin(), m_order[i].end(), [&](uint32_t lh, uint32_t rh) {
			const auto lt = m_passes[lh]->getOutput().resourceId;
			const auto rt = m_passes[rh]->getOutput().resourceId;
			return lt > rt;
		});

	// Gather targets which are used as shared depth.
	for (auto& it : m_targets)
		if (
			it.second.sharedDepthStencilTargetSetId != RGTargetSet::Invalid &&
			it.second.sharedDepthStencilTargetSetId != RGTargetSet::Output)
			m_sharedDepthTargets.insert(it.second.sharedDepthStencilTargetSetId);

	return true;
}

bool RenderGraph::build(RenderContext* renderContext, int32_t width, int32_t height)
{
	T_FATAL_ASSERT(!renderContext->havePendingDraws());

	// Initialize input and output reference counts of all targets.
	for (int32_t i = 0; i < sizeof_array(m_order); ++i)
	{
		const auto& order = m_order[i];
		for (const auto index : order)
		{
			const auto pass = m_passes[index];
			const auto& output = pass->getOutput();
			if (output.resourceId != ~0)
			{
				auto it = m_targets.find(RGTargetSet(output.resourceId));
				if (it != m_targets.end())
					it->second.outputRefCount++;
			}
			for (const auto& input : pass->getInputs())
			{
				auto it = m_targets.find(RGTargetSet(input.resourceId));
				if (it != m_targets.end())
					it->second.inputRefCount++;
			}
		}
	}

	// Calculate size of all targets.
	for (auto it : m_targets)
		if (!realizeTargetDimensions(width, height, it.first))
			return false;

	// Acquire all targets which are used for sharing depth.
	for (auto id : m_sharedDepthTargets)
	{
		auto& target = m_targets[id];
		target.persistentHandle = ~0U;
		if (!acquire(target))
		{
			cleanup();
			return false;
		}
	}

	// Acquire all persistent targets in case they are read from
	// before being used as an output.
	for (auto& it : m_targets)
	{
		auto& target = it.second;
		if (
			target.targetSet == nullptr &&
			target.persistentHandle != 0 &&
			(target.inputRefCount != 0 || target.outputRefCount != 0))
		{
			if (!acquire(target))
			{
				cleanup();
				return false;
			}
		}
	}

	for (auto& it : m_buffers)
	{
		auto& sbuffer = it.second;
		if (sbuffer.buffer == nullptr)
		{
			sbuffer.buffer = m_bufferPool->acquire(sbuffer.bufferDesc, width, height, sbuffer.persistentHandle);
			if (!sbuffer.buffer)
				return false;
		}
	}

	for (auto& it : m_textures)
	{
		auto& texture = it.second;
		if (texture.texture == nullptr)
		{
			texture.texture = m_texturePool->acquire(texture.textureDesc, width, height, texture.persistentHandle);
			if (!texture.texture)
				return false;
		}
	}

#if !defined(__ANDROID__) && !defined(__IOS__)
	double referenceOffset = Profiler::getInstance().getTime();

	int32_t* queryHandles = nullptr;
	int32_t* referenceQueryHandle = nullptr;
	int32_t* passQueryHandles = nullptr;
	int32_t* profiling = nullptr;

	if (m_profiler)
	{
		// Allocate query handles from render context's heap since they get automatically
		// freed when the context is reset.
		queryHandles = (int32_t*)renderContext->alloc((uint32_t)(m_passes.size() + 1) * sizeof(int32_t), (uint32_t)alignOf< int32_t >());
		referenceQueryHandle = queryHandles;
		passQueryHandles = queryHandles + 1;

		renderContext->direct< ProfileBeginRenderBlock >(referenceQueryHandle);
	}

#	define T_PASS_PROFILE_BEGIN() \
		if (profiling) \
		{ \
			renderContext->direct< ProfileBeginRenderBlock >(profiling); \
		}
#	define T_PASS_PROFILE_END() \
		if (profiling) \
		{ \
			renderContext->direct< ProfileEndRenderBlock >(profiling); \
			profiling = nullptr; \
		}
#	define T_PASS_IS_PROFILING \
		(profiling != nullptr)

#else
#	define T_PASS_PROFILE_BEGIN()
#	define T_PASS_PROFILE_END()
#	define T_PASS_IS_PROFILING false
#endif

	// Render passes in dependency order.
	//
	// Since we don't want to load/store render passes, esp when using MSAA,
	// we track current output target and automatically merge render passes.
	//
	TargetResource* currentTarget = nullptr;
	RenderPass::Output currentOutput;

	for (int32_t i = sizeof_array(m_order) - 1; i >= 0; --i)
	{
		const auto& order = m_order[i];
		for (const auto index : order)
		{
			const auto pass = m_passes[index];
			const auto& inputs = pass->getInputs();
			const auto& output = pass->getOutput();

			// Begin render pass.
			if (pass->haveOutput())
			{
				if (output.resourceId != 0)
				{
					// Continue rendering to same target if possible; else start another pass.
					if (T_PASS_IS_PROFILING || currentOutput != output)
					{
						if (currentOutput.resourceId != ~0U)
						{
							T_PASS_PROFILE_BEGIN();
							renderContext->mergeComputeIntoRender();
							renderContext->mergeDrawIntoRender();
							renderContext->direct< EndPassRenderBlock >();
							T_PASS_PROFILE_END();

							if (currentTarget && currentTarget->doubleBuffered)
								currentTarget->targetSet->swap();
						}
						else if (T_PASS_IS_PROFILING)
						{
							// Profiling of a non-output pass.
							T_PASS_PROFILE_BEGIN();
							renderContext->mergeComputeIntoRender();
							renderContext->mergeDrawIntoRender();
							T_PASS_PROFILE_END();
						}

						// Begin pass if resource is a target.
						auto it = m_targets.find(RGTargetSet(output.resourceId));
						if (it != m_targets.end())
						{
							TargetResource& target = it->second;
							if (target.targetSet == nullptr)
							{
								T_ASSERT(!target.external);
								if (!acquire(target))
								{
									cleanup();
									return false;
								}
							}

							auto tb = renderContext->allocNamed< BeginPassRenderBlock >(pass->getName());
							tb->renderTargetSet = target.targetSet->getWriteTargetSet();
							tb->clear = output.clear;
							tb->load = output.load;
							tb->store = output.store;
							renderContext->draw(tb);

							currentTarget = &target;
							currentOutput = output;
						}
						else
						{
							currentTarget = nullptr;
							currentOutput = RenderPass::Output();
						}
					}
				}
				else // Output to framebuffer; implicit as target 0.
				{
					if (T_PASS_IS_PROFILING || currentOutput.resourceId != 0)
					{
						if (currentOutput.resourceId != ~0U)
						{
							T_PASS_PROFILE_BEGIN();
							renderContext->mergeComputeIntoRender();
							renderContext->mergeDrawIntoRender();
							renderContext->direct< EndPassRenderBlock >();
							T_PASS_PROFILE_END();

							if (currentTarget && currentTarget->doubleBuffered)
								currentTarget->targetSet->swap();
						}
						else if (T_PASS_IS_PROFILING)
						{
							// Profiling of a non-output pass.
							T_PASS_PROFILE_BEGIN();
							renderContext->mergeComputeIntoRender();
							renderContext->mergeDrawIntoRender();
							T_PASS_PROFILE_END();
						}
						
						auto tb = renderContext->allocNamed< BeginPassRenderBlock >(pass->getName());
						tb->clear = output.clear;
						tb->load = output.load;
						tb->store = output.store;
						renderContext->draw(tb);

						currentTarget = nullptr;
						currentOutput = output;
					}
				}
			}
			else if (currentOutput.resourceId != ~0U)
			{
				T_PASS_PROFILE_BEGIN();
				renderContext->mergeComputeIntoRender();
				renderContext->mergeDrawIntoRender();
				renderContext->direct< EndPassRenderBlock >();
				T_PASS_PROFILE_END();

				if (currentTarget && currentTarget->doubleBuffered)
					currentTarget->targetSet->swap();

				currentTarget = nullptr;
				currentOutput = RenderPass::Output();
			}
			else if (T_PASS_IS_PROFILING)
			{
				// Profiling of a non-output pass.
				T_PASS_PROFILE_BEGIN();
				renderContext->mergeComputeIntoRender();
				renderContext->mergeDrawIntoRender();
				T_PASS_PROFILE_END();
			}

			// Build this pass.
			T_PROFILER_BEGIN(L"RenderGraph build \"" + pass->getName() + L"\"");
			m_buildingPasses = true;

#if !defined(__ANDROID__) && !defined(__IOS__)
			if (m_profiler)
			{
				T_FATAL_ASSERT(profiling == nullptr);
				profiling = &passQueryHandles[index];
			}
#endif
			for (const auto& build : pass->getBuilds())
			{
				build(*this, renderContext);

				// Merge all pending priority draws (sorted by depth) after each build step.
				renderContext->mergePriorityIntoDraw(RenderPriority::All);
			}
			m_buildingPasses = false;
			T_PROFILER_END();

			// Decrement reference counts on input targets; release if last reference.
			for (const auto& input : inputs)
			{
				if (input.resourceId == 0)
					continue;

				auto it = m_targets.find(RGTargetSet(input.resourceId));
				if (it == m_targets.end())
					continue;

				auto& target = it->second;
				if (--target.inputRefCount <= 0)
				{
					if (!target.external && target.persistentHandle == 0)
						m_targetSetPool->release(target.targetSet);
				}
			}
		}
	}

	if (currentOutput.resourceId != ~0U)
	{
		T_PASS_PROFILE_BEGIN();
		renderContext->mergeComputeIntoRender();
		renderContext->mergeDrawIntoRender();
		renderContext->direct< EndPassRenderBlock >();
		T_PASS_PROFILE_END();

		if (currentTarget && currentTarget->doubleBuffered)
			currentTarget->targetSet->swap();
	}

#if !defined(__ANDROID__) && !defined(__IOS__)
	if (m_profiler)
	{
		renderContext->direct< ProfileEndRenderBlock >(referenceQueryHandle);

		// Report all queries last using reference query to calculate offset.
		int32_t ordinal = 0;
		for (int32_t i = sizeof_array(m_order) - 1; i >= 0; --i)
		{
			const auto& order = m_order[i];
			for (int32_t j = 0; j < (int32_t)order.size(); ++j)
			{
				const uint32_t index = order[j];
				const auto pass = m_passes[index];

				auto pr = renderContext->alloc< ProfileReportRenderBlock >();
				pr->queryHandle = &passQueryHandles[index];
				pr->referenceQueryHandle = referenceQueryHandle;
				pr->offset = referenceOffset;
				pr->sink = [=, name = pass->getName(), this](double start, double duration) {
					m_profiler(ordinal, i, name, start, duration);
				};
				renderContext->direct(pr);

				++ordinal;
			}
		}
	}
#endif

	T_FATAL_ASSERT(!renderContext->havePendingComputes());
	T_FATAL_ASSERT(!renderContext->havePendingDraws());

	// Ensure all persistent targets are returned to pool, since we're
	// manually acquiring all at the beginning.
	for (auto& it : m_targets)
	{
		auto& target = it.second;
		if (target.persistentHandle != 0)
			m_targetSetPool->release(target.targetSet);
		target.realized = { 0, 0 };
		target.targetSet = nullptr;
	}

	for (auto& it : m_buffers)
	{
		auto& sbuffer = it.second;
		if (sbuffer.buffer != nullptr)
			m_bufferPool->release(sbuffer.buffer);
	}

	for (auto& it : m_textures)
	{
		auto& texture = it.second;
		if (texture.texture != nullptr)
			m_texturePool->release(texture.texture);
	}

	// Cleanup pool data structures.
	m_targetSetPool->cleanup();
	m_texturePool->cleanup();

	// Remove all data; keeps memory allocated for arrays
	// since it's very likely this will be identically
	// re-populated next frame.
	cleanup();

	m_counter++;
	return true;
}

bool RenderGraph::realizeTargetDimensions(int32_t width, int32_t height, RGTargetSet targetId)
{
	TargetResource& target = m_targets[targetId];
	if (target.realized.width != 0 && target.realized.height != 0)
		return true;

	const RenderGraphTargetSetDesc& targetSetDesc = target.targetSetDesc;
	if (targetSetDesc.width != 0 && targetSetDesc.height != 0)
	{
		width = targetSetDesc.width;
		height = targetSetDesc.height;
	}

	if (target.sizeReferenceTargetSetId != RGTargetSet::Output)
	{
		if (!realizeTargetDimensions(width, height, target.sizeReferenceTargetSetId))
			return false;

		const TargetResource& sizeReferenceTarget = m_targets[target.sizeReferenceTargetSetId];
		width = sizeReferenceTarget.realized.width;
		height = sizeReferenceTarget.realized.height;
	}

	if (
		target.sharedDepthStencilTargetSetId != RGTargetSet::Output &&
		target.sharedDepthStencilTargetSetId != RGTargetSet::Invalid)
	{
		if (!realizeTargetDimensions(width, height, target.sharedDepthStencilTargetSetId))
			return false;

		const TargetResource& sharedDepthStencilTarget = m_targets[target.sharedDepthStencilTargetSetId];
		width = sharedDepthStencilTarget.realized.width;
		height = sharedDepthStencilTarget.realized.height;
	}

	if (targetSetDesc.referenceWidthDenom > 0)
		width = (width * targetSetDesc.referenceWidthMul + targetSetDesc.referenceWidthDenom - 1) / targetSetDesc.referenceWidthDenom;
	if (targetSetDesc.referenceHeightDenom > 0)
		height = (height * targetSetDesc.referenceHeightMul + targetSetDesc.referenceHeightDenom - 1) / targetSetDesc.referenceHeightDenom;

	if (targetSetDesc.maxWidth > 0)
		width = min< int32_t >(width, targetSetDesc.maxWidth);
	if (targetSetDesc.maxHeight > 0)
		height = min< int32_t >(height, targetSetDesc.maxHeight);

	target.realized = { width, height };
	return true;
}

bool RenderGraph::acquire(TargetResource& inoutTarget)
{
	Ref< IRenderTargetSet > sharedDepthTargetSet;
	if (
		inoutTarget.sharedDepthStencilTargetSetId != RGTargetSet::Output &&
		inoutTarget.sharedDepthStencilTargetSetId != RGTargetSet::Invalid)
	{
		const auto& t = m_targets[inoutTarget.sharedDepthStencilTargetSetId];
		sharedDepthTargetSet = t.targetSet->getWriteTargetSet();
	}

	const bool sharedPrimaryDepthStencilTargetSet = (inoutTarget.sharedDepthStencilTargetSetId == RGTargetSet::Output);

	inoutTarget.targetSet = m_targetSetPool->acquire(
		inoutTarget.name,
		inoutTarget.targetSetDesc,
		sharedDepthTargetSet,
		sharedPrimaryDepthStencilTargetSet,
		inoutTarget.realized.width,
		inoutTarget.realized.height,
		m_multiSample,
		inoutTarget.doubleBuffered,
		inoutTarget.persistentHandle);

	if (!inoutTarget.targetSet)
		return false;

	return true;
}

void RenderGraph::cleanup()
{
	m_passes.resize(0);
	m_targets.reset();
	m_buffers.reset();
	for (int32_t i = 0; i < sizeof_array(m_order); ++i)
		m_order[i].resize(0);
	m_sharedDepthTargets.clear();
	m_nextResourceId = 1;
}

}
