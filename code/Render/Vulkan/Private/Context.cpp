/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Core/Io/FileSystem.h"
#include "Core/Io/IStream.h"
#include "Core/Io/StringOutputStream.h"
#include "Core/Log/Log.h"
#include "Core/System/OS.h"
#include "Core/Thread/Acquire.h"
#include "Core/Thread/Atomic.h"
#include "Render/Vulkan/Private/ApiLoader.h"
#include "Render/Vulkan/Private/CommandBuffer.h"
#include "Render/Vulkan/Private/Context.h"
#include "Render/Vulkan/Private/Queue.h"
#include "Render/Vulkan/Private/UniformBufferPool.h"
#include "Render/Vulkan/Private/Utilities.h"

namespace traktor::render
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.render.Context", Context, Object)

Context::Context(
	VkPhysicalDevice physicalDevice,
	VkDevice logicalDevice,
	VmaAllocator allocator,
	uint32_t graphicsQueueIndex
)
:	m_physicalDevice(physicalDevice)
,	m_logicalDevice(logicalDevice)
,	m_allocator(allocator)
,	m_graphicsQueueIndex(graphicsQueueIndex)
,	m_pipelineCache(0)
,	m_descriptorPool(0)
,	m_views(0)
,	m_bindlessTexturesDescriptorLayout(0)
,	m_bindlessTexturesDescriptorSet(0)
,	m_bindlessImagesDescriptorLayout(0)
,	m_bindlessImagesDescriptorSet(0)
,	m_sampledResourceIndexAllocator(0, MaxBindlessResources - 1)
{
}

Context::~Context()
{
	// Destroy uniform buffer pools.
	for (int32_t i = 0; i < sizeof_array(m_uniformBufferPools); ++i)
	{
		m_uniformBufferPools[i]->destroy();
		m_uniformBufferPools[i] = nullptr;
	}

	// Destroy descriptor pool.
	if (m_descriptorPool != 0)
	{
		vkDestroyDescriptorPool(m_logicalDevice, m_descriptorPool, nullptr);
		m_descriptorPool = 0;
	}
}

bool Context::create()
{
	AlignedVector< uint8_t > buffer;

	// Create queues.
	m_graphicsQueue = Queue::create(this, m_graphicsQueueIndex);

	// Create pipeline cache.
	VkPipelineCacheCreateInfo pcci = {};
	pcci.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	pcci.flags = 0;
	pcci.initialDataSize = 0;
	pcci.pInitialData = nullptr;

	StringOutputStream ss;
#if defined(__IOS__)
	ss << OS::getInstance().getUserHomePath() << L"/Library/Caches/Traktor/Vulkan/Pipeline.cache";
#else
	ss << OS::getInstance().getWritableFolderPath() << L"/Traktor/Vulkan/Pipeline.cache";
#endif

	Ref< IStream > file = FileSystem::getInstance().open(ss.str(), File::FmRead);
	if (file)
	{
	 	const uint32_t size = (uint32_t)file->available();
		buffer.resize(size);
	 	file->read(buffer.ptr(), size);
	 	file->close();

		pcci.initialDataSize = size;
		pcci.pInitialData = buffer.c_ptr();

		log::debug << L"Pipeline cache \"" << ss.str() << L"\" loaded successfully." << Endl;
	}
	else
		log::debug << L"No pipeline cache found; creating new cache." << Endl;

	vkCreatePipelineCache(
		m_logicalDevice,
		&pcci,
		nullptr,
		&m_pipelineCache
	);

	// Create descriptor set pool.
	VkDescriptorPoolSize dps[5];
	dps[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	dps[0].descriptorCount = 80000;
	dps[1].type = VK_DESCRIPTOR_TYPE_SAMPLER;
	dps[1].descriptorCount = 80000;
	dps[2].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	dps[2].descriptorCount = MaxBindlessResources;
	dps[3].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
	dps[3].descriptorCount = 8000;
	dps[4].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	dps[4].descriptorCount = MaxBindlessResources;

	VkDescriptorPoolCreateInfo dpci = {};
	dpci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	dpci.pNext = nullptr;
	dpci.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT | VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;
	dpci.maxSets = 32000;
	dpci.poolSizeCount = sizeof_array(dps);
	dpci.pPoolSizes = dps;

	vkCreateDescriptorPool(m_logicalDevice, &dpci, nullptr, &m_descriptorPool);

	// Create uniform buffer pools.
	m_uniformBufferPools[0] = new UniformBufferPool(this,   1000, L"Once");
	m_uniformBufferPools[1] = new UniformBufferPool(this,  10000, L"Frame");
	m_uniformBufferPools[2] = new UniformBufferPool(this, 100000, L"Draw");

	// Bindless textures.
	{
		const VkDescriptorBindingFlags bindlessFlags = 
			VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT |
			VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT |
			VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;/* |
			VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT_EXT;*/

		VkDescriptorSetLayoutBinding binding;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		binding.descriptorCount = MaxBindlessResources;
		binding.binding = BindlessTexturesBinding;
		binding.stageFlags = VK_SHADER_STAGE_ALL;
		binding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo layoutInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &binding;
		layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;

		VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT };
		extendedInfo.bindingCount = 1;
		extendedInfo.pBindingFlags = &bindlessFlags;

		layoutInfo.pNext = &extendedInfo;

		if (vkCreateDescriptorSetLayout(m_logicalDevice, &layoutInfo, nullptr, &m_bindlessTexturesDescriptorLayout) != VK_SUCCESS)
		{
			log::error << L"Failed to create Vulkan; failed to create bindless textures descriptor layout." << Endl;
			return false;
		}

		// Create descriptor set.
		VkDescriptorSetAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		alloc_info.descriptorPool = m_descriptorPool;
		alloc_info.descriptorSetCount = 1;
		alloc_info.pSetLayouts = &m_bindlessTexturesDescriptorLayout;

		VkDescriptorSetVariableDescriptorCountAllocateInfoEXT count_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT };
		const uint32_t maxBinding = MaxBindlessResources - 1;
		count_info.descriptorSetCount = 1;
		count_info.pDescriptorCounts = &maxBinding;	// This number is the max allocatable count.
		alloc_info.pNext = &count_info;

		VkResult result;
		if ((result = vkAllocateDescriptorSets(m_logicalDevice, &alloc_info, &m_bindlessTexturesDescriptorSet)) != VK_SUCCESS)
		{
			log::error << L"Failed to create Vulkan; failed to create bindless textures descriptor set. " << getHumanResult(result) << Endl;
			return false;

		}
	}

	// Bindless images.
	{
		const VkDescriptorBindingFlags bindlessFlags = 
			VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT |
			VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT |
			VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;/* |
			VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT_EXT;*/

		VkDescriptorSetLayoutBinding binding;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		binding.descriptorCount = MaxBindlessResources;
		binding.binding = BindlessImagesBinding;
		binding.stageFlags = VK_SHADER_STAGE_ALL;
		binding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo layoutInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &binding;
		layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;

		VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT };
		extendedInfo.bindingCount = 1;
		extendedInfo.pBindingFlags = &bindlessFlags;

		layoutInfo.pNext = &extendedInfo;

		if (vkCreateDescriptorSetLayout(m_logicalDevice, &layoutInfo, nullptr, &m_bindlessImagesDescriptorLayout) != VK_SUCCESS)
		{
			log::error << L"Failed to create Vulkan; failed to create bindless images descriptor layout." << Endl;
			return false;
		}

		// Create descriptor set.
		VkDescriptorSetAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		alloc_info.descriptorPool = m_descriptorPool;
		alloc_info.descriptorSetCount = 1;
		alloc_info.pSetLayouts = &m_bindlessImagesDescriptorLayout;

		VkDescriptorSetVariableDescriptorCountAllocateInfoEXT count_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT };
		const uint32_t maxBinding = MaxBindlessResources - 1;
		count_info.descriptorSetCount = 1;
		count_info.pDescriptorCounts = &maxBinding;	// This number is the max allocatable count.
		alloc_info.pNext = &count_info;

		VkResult result;
		if ((result = vkAllocateDescriptorSets(m_logicalDevice, &alloc_info, &m_bindlessImagesDescriptorSet)) != VK_SUCCESS)
		{
			log::error << L"Failed to create Vulkan; failed to create bindless images descriptor set. " << getHumanResult(result) << Endl;
			return false;

		}
	}

	return true;
}

void Context::incrementViews()
{
	Atomic::increment(m_views);
}

void Context::decrementViews()
{
	Atomic::decrement(m_views);
}

void Context::addDeferredCleanup(const cleanup_fn_t& fn)
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_cleanupLock);

	// In case there are no render views which can perform cleanup
	// after each frame, we do this immediately.
	if (m_views > 0)
		m_cleanupFns.push_back(fn);
	else
	{
		for (int32_t i = 0; i < sizeof_array(m_uniformBufferPools); ++i)
		{
			if (m_uniformBufferPools[i])
				m_uniformBufferPools[i]->flush();
		}

		fn(this);
	}
}

void Context::addCleanupListener(ICleanupListener* cleanupListener)
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_cleanupLock);
	m_cleanupListeners.push_back(cleanupListener);
}

void Context::removeCleanupListener(ICleanupListener* cleanupListener)
{
	T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_cleanupLock);
	auto it = std::find(m_cleanupListeners.begin(), m_cleanupListeners.end(), cleanupListener);
	m_cleanupListeners.erase(it);
}

void Context::performCleanup()
{
	if (m_cleanupFns.empty())
		return;

	{
		T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_graphicsQueue->m_lock);
		T_ANONYMOUS_VAR(Acquire< Semaphore >)(m_cleanupLock);

		// Wait until GPU is idle to ensure resources are not used, or pending, in some queue before destroying them.
		vkDeviceWaitIdle(m_logicalDevice);

		while (!m_cleanupFns.empty())
		{
			// Take over vector in case more resources are added for cleanup from callbacks.
			AlignedVector< cleanup_fn_t > cleanupFns;
			cleanupFns.swap(m_cleanupFns);

			// Invoke cleanups.
			for (const auto& cleanupFn : cleanupFns)
				cleanupFn(this);
		}
		
		// Reset descriptor pool since we need to ensure programs clear their cached descriptor sets.
		//vkResetDescriptorPool(m_logicalDevice, m_descriptorPool, 0);
		//m_descriptorPoolRevision++;

		for (auto cleanupListener : m_cleanupListeners)
			cleanupListener->postCleanup();
	}
}

void Context::recycle()
{
	for (int32_t i = 0; i < sizeof_array(m_uniformBufferPools); ++i)
		m_uniformBufferPools[i]->recycle();
}

bool Context::savePipelineCache()
{
	size_t size = 0;
	vkGetPipelineCacheData(m_logicalDevice, m_pipelineCache, &size, nullptr);
	if (!size)
		return true;

	AlignedVector< uint8_t > buffer(size, 0);
	vkGetPipelineCacheData(m_logicalDevice, m_pipelineCache, &size, buffer.ptr());

	StringOutputStream ss;
#if defined(__IOS__)
	ss << OS::getInstance().getUserHomePath() << L"/Library/Caches/Traktor/Vulkan/Pipeline.cache";
#else
	ss << OS::getInstance().getWritableFolderPath() << L"/Traktor/Vulkan/Pipeline.cache";
#endif

	FileSystem::getInstance().makeAllDirectories(Path(ss.str()).getPathOnly());

	Ref< IStream > file = FileSystem::getInstance().open(ss.str(), File::FmWrite);
	if (!file)
	{
		log::error << L"Unable to save pipeline cache; failed to create file \"" << ss.str() << L"\"." << Endl;
		return false;
	}

	file->write(buffer.c_ptr(), size);
	file->close();
	
	log::debug << L"Pipeline cache \"" << ss.str() << L"\" saved successfully." << Endl;
	return true;
}

uint32_t Context::allocateSampledResourceIndex()
{
	return m_sampledResourceIndexAllocator.alloc();
}

void Context::freeSampledResourceIndex(uint32_t& resourceIndex)
{
	T_FATAL_ASSERT(resourceIndex != ~0U);
	m_sampledResourceIndexAllocator.free(resourceIndex);
	resourceIndex = ~0U;
}

uint32_t Context::allocateStorageResourceIndex(uint32_t span)
{
	return m_storageResourceIndexAllocator.allocSequential(span);
}

void Context::freeStorageResourceIndex(uint32_t& resourceIndex, uint32_t span)
{
	T_FATAL_ASSERT(resourceIndex != ~0U);
	m_storageResourceIndexAllocator.freeSequential(resourceIndex, span);
	resourceIndex = ~0U;
}

}
