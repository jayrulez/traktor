/*
 * TRAKTOR
 * Copyright (c) 2022-2025 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Ref.h"
#include "Render/IRenderSystem.h"
#include "Render/Vulkan/Private/ApiHeader.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_RENDER_VULKAN_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::render
{

class Context;
class PipelineLayoutCache;
class ShaderModuleCache;

#if defined(_WIN32) || defined(__LINUX__) || defined(__RPI__)
class Window;
#endif

/*! Vulkan render system.
 * \ingroup Render
 *
 * Vulkan render system implementation.
 */
class T_DLLCLASS RenderSystemVk : public IRenderSystem
{
	T_RTTI_CLASS;

public:
	virtual bool create(const RenderSystemDesc& desc) override final;

	virtual void destroy() override final;

	virtual bool reset(const RenderSystemDesc& desc) override final;

	virtual void getInformation(RenderSystemInformation& outInfo) const override final;

	virtual bool supportRayTracing() const override final;

	virtual uint32_t getDisplayCount() const override final;

	virtual uint32_t getDisplayModeCount(uint32_t display) const override final;

	virtual DisplayMode getDisplayMode(uint32_t display, uint32_t index) const override final;

	virtual DisplayMode getCurrentDisplayMode(uint32_t display) const override final;

	virtual float getDisplayAspectRatio(uint32_t display) const override final;

	virtual Ref< IRenderView > createRenderView(const RenderViewDefaultDesc& desc) override final;

	virtual Ref< IRenderView > createRenderView(const RenderViewEmbeddedDesc& desc) override final;

	virtual Ref< Buffer > createBuffer(uint32_t usage, uint32_t bufferSize, bool dynamic) override final;

	virtual Ref< const IVertexLayout > createVertexLayout(const AlignedVector< VertexElement >& vertexElements) override final;

	virtual Ref< ITexture > createSimpleTexture(const SimpleTextureCreateDesc& desc, const wchar_t* const tag) override final;

	virtual Ref< ITexture > createCubeTexture(const CubeTextureCreateDesc& desc, const wchar_t* const tag) override final;

	virtual Ref< ITexture > createVolumeTexture(const VolumeTextureCreateDesc& desc, const wchar_t* const tag) override final;

	virtual Ref< IRenderTargetSet > createRenderTargetSet(const RenderTargetSetCreateDesc& desc, IRenderTargetSet* sharedDepthStencil, const wchar_t* const tag) override final;

	virtual Ref< IAccelerationStructure > createTopLevelAccelerationStructure(uint32_t numInstances) override final;

	virtual Ref< IAccelerationStructure > createAccelerationStructure(const Buffer* vertexBuffer, const IVertexLayout* vertexLayout, const Buffer* indexBuffer, IndexType indexType, const AlignedVector< Primitives >& primitives, bool dynamic) override final;

	virtual Ref< IProgram > createProgram(const ProgramResource* programResource, const wchar_t* const tag) override final;

	virtual void purge() override final;

	virtual void getStatistics(RenderSystemStatistics& outStatistics) const override final;

	virtual void* getInternalHandle() const override final;

private:
#if defined(_WIN32) || defined(__LINUX__) || defined(__RPI__)
	Ref< Window > m_window;
#endif
#if defined(__LINUX__) || defined(__RPI__)
	void* m_display = nullptr;
#endif
#if defined(__ANDROID__)
	int32_t m_screenWidth = 0;
	int32_t m_screenHeight = 0;
#endif
	VkInstance m_instance = 0;
	VkPhysicalDevice m_physicalDevice = 0;
	VkDevice m_logicalDevice = 0;
	VkDebugUtilsMessengerEXT m_debugMessenger = 0;
	RenderSystemStatistics m_statistics;
	Ref< Context > m_context;
	Ref< ShaderModuleCache > m_shaderModuleCache;
	Ref< PipelineLayoutCache > m_pipelineLayoutCache;
	VmaAllocator m_allocator = 0;
	int32_t m_maxAnisotropy = 0;
	float m_mipBias = 0.0f;
	bool m_rayTracing = false;
};

}
