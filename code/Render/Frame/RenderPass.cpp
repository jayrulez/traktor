/*
 * TRAKTOR
 * Copyright (c) 2022-2025 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Render/Frame/RenderPass.h"

#include "Core/Class/BoxedAllocator.h"

namespace traktor::render
{
namespace
{

BoxedAllocator< RenderPass, 256 > s_allocRenderPass;

}

T_IMPLEMENT_RTTI_CLASS(L"traktor.render.RenderPass", RenderPass, Object)

RenderPass::RenderPass(const std::wstring& name)
	: m_name(name)
{
}

void RenderPass::setName(const std::wstring& name)
{
	m_name = name;
}

void RenderPass::addInput(handle_t resourceId)
{
	auto& input = m_inputs.push_back();
	input.resourceId = resourceId;
}

void RenderPass::addInput(RGTargetSet targetSet)
{
	// Just ignore invalid target set; convenient when setting up passes to be able to add null inputs.
	if (targetSet == RGTargetSet::Invalid || targetSet == RGTargetSet::Output)
		return;

	auto& input = m_inputs.push_back();
	input.resourceId = targetSet.get();
}

void RenderPass::addInput(RGBuffer buffer)
{
	// Just ignore invalid buffer; convenient when setting up passes to be able to add null inputs.
	if (buffer == RGBuffer::Invalid)
		return;

	auto& input = m_inputs.push_back();
	input.resourceId = buffer.get();
}

void RenderPass::addInput(RGTexture texture)
{
	// Just ignore invalid texture; convenient when setting up passes to be able to add null inputs.
	if (texture == RGTexture::Invalid)
		return;

	auto& input = m_inputs.push_back();
	input.resourceId = texture.get();
}

void RenderPass::addInput(RGDependency dependency)
{
	// Just ignore invalid dependency; convenient when setting up passes to be able to add null inputs.
	if (dependency == RGDependency::Invalid)
		return;

	auto& input = m_inputs.push_back();
	input.resourceId = dependency.get();
}

void RenderPass::addWeakInput(RGTexture texture)
{
	// #todo Currently we can get away with not doing anything but
	// we need to revisit this to ensure resource life time.
}

void RenderPass::setOutput(RGTargetSet targetSet, uint32_t load, uint32_t store)
{
	m_output.resourceId = targetSet.get();
	m_output.clear.mask = 0;
	m_output.load = load;
	m_output.store = store;
}

void RenderPass::setOutput(RGTargetSet targetSet, const Clear& clear, uint32_t load, uint32_t store)
{
	m_output.resourceId = targetSet.get();
	m_output.clear = clear;
	m_output.load = load;
	m_output.store = store;
}

void RenderPass::setOutput(RGBuffer buffer)
{
	m_output.resourceId = buffer.get();
	m_output.clear.mask = 0;
	m_output.load = 0;
	m_output.store = 0;
}

void RenderPass::setOutput(RGTexture texture)
{
	m_output.resourceId = texture.get();
	m_output.clear.mask = 0;
	m_output.load = 0;
	m_output.store = 0;
}

void RenderPass::setOutput(RGDependency dependency)
{
	m_output.resourceId = dependency.get();
	m_output.clear.mask = 0;
	m_output.load = 0;
	m_output.store = 0;
}

void RenderPass::addBuild(const fn_build_t& build)
{
	m_builds.push_back(build);
}

void* RenderPass::operator new(size_t size)
{
	return s_allocRenderPass.alloc();
}

void RenderPass::operator delete(void* ptr)
{
	s_allocRenderPass.free(ptr);
}

}
