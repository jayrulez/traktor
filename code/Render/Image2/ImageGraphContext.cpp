/*
 * TRAKTOR
 * Copyright (c) 2022-2025 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Render/Image2/ImageGraphContext.h"

#include "Render/Frame/RenderGraph.h"
#include "Render/IRenderTargetSet.h"

namespace traktor::render
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.render.ImageGraphContext", ImageGraphContext, Object)

void ImageGraphContext::associateExplicitTexture(img_handle_t textureId, ITexture* texture)
{
	m_textureTargetSet[textureId] = { .texture = texture };
}

void ImageGraphContext::associateTexture(img_handle_t textureId, handle_t rgTextureId)
{
	m_textureTargetSet[textureId] = { .textureId = rgTextureId };
}

void ImageGraphContext::associateTextureTargetSet(img_handle_t textureId, handle_t targetSetId, int32_t colorIndex)
{
	m_textureTargetSet[textureId] = { .targetSetId = targetSetId,
		.colorIndex = colorIndex };
}

void ImageGraphContext::associateTextureTargetSetDepth(img_handle_t textureId, handle_t targetSetId)
{
	m_textureTargetSet[textureId] = { .targetSetId = targetSetId,
		.colorIndex = -1 };
}

handle_t ImageGraphContext::findTextureTargetSetId(img_handle_t textureId) const
{
	const auto it = m_textureTargetSet.find(textureId);
	if (it != m_textureTargetSet.end())
		return it->second.targetSetId;
	else
		return 0;
}

ITexture* ImageGraphContext::findTexture(const RenderGraph& renderGraph, img_handle_t textureId) const
{
	const auto it = m_textureTargetSet.find(textureId);
	if (it == m_textureTargetSet.end())
		return nullptr;

	if (it->second.texture)
		return it->second.texture;

	if (it->second.targetSetId != 0)
	{
		auto targetSet = renderGraph.getTargetSet(it->second.targetSetId);
		if (!targetSet)
			return nullptr;

		if (it->second.colorIndex >= 0)
			return targetSet->getColorTexture(it->second.colorIndex);
		else
			return targetSet->getDepthTexture();
	}

	if (it->second.textureId != 0)
		return renderGraph.getTexture(it->second.textureId);

	return nullptr;
}

void ImageGraphContext::associateSBuffer(img_handle_t sbufferId, handle_t frameSbufferId)
{
	m_sbufferHandles[sbufferId] = frameSbufferId;
}

handle_t ImageGraphContext::findSBufferId(img_handle_t sbufferId) const
{
	auto it = m_sbufferHandles.find(sbufferId);
	if (it != m_sbufferHandles.end())
		return it->second;
	else
		return 0;
}

Buffer* ImageGraphContext::findSBuffer(const RenderGraph& renderGraph, img_handle_t sbufferId) const
{
	auto it = m_sbufferHandles.find(sbufferId);
	if (it == m_sbufferHandles.end())
		return nullptr;

	return renderGraph.getBuffer(it->second);
}

void ImageGraphContext::setTechniqueFlag(const handle_t techniqueId, bool flag)
{
	m_techniqueFlags[techniqueId] = flag;
}

const StaticMap< handle_t, bool, 16 >& ImageGraphContext::getTechniqueFlags() const
{
	return m_techniqueFlags;
}

void ImageGraphContext::applyTechniqueFlags(const Shader* shader, Shader::Permutation& inoutPermutation) const
{
	for (auto it : m_techniqueFlags)
		shader->setCombination(it.first, it.second, inoutPermutation);
}
}
