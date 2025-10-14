/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <cstring>
#include <ft2build.h>
#include <set>
#include FT_FREETYPE_H

#include "Core/Io/FileSystem.h"
#include "Core/Log/Log.h"
#include "Core/Misc/String.h"
#include "Core/Misc/TString.h"
#include "Core/Serialization/DeepHash.h"
#include "Core/Settings/PropertyString.h"
#include "Database/Instance.h"
#include "Drawing/Image.h"
#include "Drawing/PixelFormat.h"
#include "Editor/IPipelineBuilder.h"
#include "Editor/IPipelineDepends.h"
#include "Editor/IPipelineSettings.h"
#include "Paper/BitmapFont/BitmapFont.h"
#include "Paper/Editor/BitmapFontAsset.h"
#include "Paper/Editor/BitmapFontPipeline.h"
#include "Paper/Packer.h"
#include "Render/Editor/Texture/TextureOutput.h"

namespace traktor::paper
{
namespace
{
const Guid c_idPaper2DShader(L"{A704DB1C-60E6-9D44-AD1D-F7822568242D}"); // System/Paper/Shaders/Paper2D
}

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.BitmapFontPipeline", 0, BitmapFontPipeline, editor::IPipeline)

bool BitmapFontPipeline::create(const editor::IPipelineSettings* settings, db::Database* database)
{
	m_assetPath = settings->getPropertyExcludeHash< std::wstring >(L"Pipeline.AssetPath", L"");
	return true;
}

void BitmapFontPipeline::destroy()
{
}

TypeInfoSet BitmapFontPipeline::getAssetTypes() const
{
	return makeTypeInfoSet< BitmapFontAsset >();
}

bool BitmapFontPipeline::shouldCache() const
{
	return true;
}

uint32_t BitmapFontPipeline::hashAsset(const ISerializable* sourceAsset) const
{
	return DeepHash(sourceAsset).get();
}

bool BitmapFontPipeline::buildDependencies(
	editor::IPipelineDepends* pipelineDepends,
	const db::Instance* sourceInstance,
	const ISerializable* sourceAsset,
	const std::wstring& outputPath,
	const Guid& outputGuid) const
{
	const BitmapFontAsset* fontAsset = mandatory_non_null_type_cast< const BitmapFontAsset* >(sourceAsset);
	pipelineDepends->addDependency(Path(m_assetPath), fontAsset->getFileName().getOriginal());
	pipelineDepends->addDependency< render::TextureOutput >();
	pipelineDepends->addDependency(c_idPaper2DShader, editor::PdfBuild | editor::PdfResource);
	return true;
}

bool BitmapFontPipeline::buildOutput(
	editor::IPipelineBuilder* pipelineBuilder,
	const editor::PipelineDependencySet* dependencySet,
	const editor::PipelineDependency* dependency,
	const db::Instance* sourceInstance,
	const ISerializable* sourceAsset,
	const std::wstring& outputPath,
	const Guid& outputGuid,
	const Object* buildParams,
	uint32_t reason) const
{
	const BitmapFontAsset* fontAsset = mandatory_non_null_type_cast< const BitmapFontAsset* >(sourceAsset);

	FT_Library library;
	FT_Face face;
	FT_Error error;

	const Path filePath = FileSystem::getInstance().getAbsolutePath(Path(m_assetPath) + fontAsset->getFileName());

	error = FT_Init_FreeType(&library);
	if (error)
	{
		log::error << L"Unable to initialize FreeType library." << Endl;
		return false;
	}

	error = FT_New_Face(
		library,
		wstombs(filePath.getPathNameOS()).c_str(),
		0,
		&face);
	if (error)
	{
		log::error << L"Unable to load font \"" << filePath.getPathName() << L"\"." << Endl;
		FT_Done_FreeType(library);
		return false;
	}

	error = FT_Set_Pixel_Sizes(face, 0, fontAsset->getSize());
	if (error)
	{
		log::error << L"Unable to set font size." << Endl;
		FT_Done_Face(face);
		FT_Done_FreeType(library);
		return false;
	}

	// Generate glyphs for common characters (Latin-1)
	std::set< uint32_t > characters;
	for (uint32_t ch = 32; ch < 256; ++ch)
		characters.insert(ch);

	const int32_t textureSize = fontAsset->getTextureSize();
	const float lineHeight = (float)fontAsset->getSize();

	// Pack glyphs into texture
	Packer packer(textureSize, textureSize);
	Ref< drawing::Image > atlasImage = new drawing::Image(
		drawing::PixelFormat::getR8G8B8A8(),
		textureSize,
		textureSize);
	atlasImage->clear(Color4f(0.0f, 0.0f, 0.0f, 0.0f));

	Ref< BitmapFont > bitmapFont = new BitmapFont();
	bitmapFont->setLineHeight(lineHeight);

	// Track maximum baseline offset from glyph metrics
	int32_t baselineOffset = 0;

	for (uint32_t ch : characters)
	{
		FT_UInt glyphIndex = FT_Get_Char_Index(face, ch);
		if (glyphIndex == 0)
			continue;

		error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_RENDER);
		if (error)
			continue;

		FT_GlyphSlot slot = face->glyph;
		FT_Bitmap& bitmap = slot->bitmap;

		// Track maximum baseline offset using horiBearingY (same as bitmap_top)
		const int32_t glyphBaselineOffset = slot->metrics.horiBearingY >> 6;
		if (glyphBaselineOffset > baselineOffset)
			baselineOffset = glyphBaselineOffset;

		if (bitmap.width == 0 || bitmap.rows == 0)
		{
			// Add empty glyph for whitespace characters
			BitmapFont::Glyph glyph;
			glyph.character = ch;
			glyph.bounds = Aabb2(Vector2::zero(), Vector2::zero());
			glyph.uvMin = Vector2::zero();
			glyph.uvMax = Vector2::zero();
			glyph.advance = (float)(slot->advance.x >> 6);
			bitmapFont->addGlyph(glyph);
			continue;
		}

		// Pack glyph into atlas
		Packer::Rectangle rect;
		if (!packer.insert(bitmap.width + 2, bitmap.rows + 2, rect))
		{
			log::warning << L"Unable to pack glyph for character " << (int)ch << L" into atlas." << Endl;
			continue;
		}

		// Copy glyph bitmap to atlas with 1-pixel padding
		// Store WHITE RGB with varying alpha - shader will modulate color
		for (uint32_t y = 0; y < bitmap.rows; ++y)
		{
			for (uint32_t x = 0; x < bitmap.width; ++x)
			{
				uint8_t alpha = bitmap.buffer[y * bitmap.width + x];
				float alphaF = alpha / 255.0f;
				// White RGB everywhere, alpha varies by glyph coverage
				// Shader: texture.rgb * vertex_color * texture.a = white * color * alpha
				Color4f color(1.0f, 1.0f, 1.0f, alphaF);
				atlasImage->setPixel(rect.x + x + 1, rect.y + y + 1, color);
			}
		}

		// Create glyph data:
		// We need to store bitmap_left and bitmap_top as offsets
		// BUT Aabb2 expects mn < mx, so we can't store bitmap_top directly
		//
		// Solution: Store in bounds:
		// mn.x = bitmap_left, mn.y = -(bitmap_top) (negative, so min)
		// mx.x = bitmap_left + width, mx.y = -(bitmap_top - rows) = -bitmap_top + rows
		//
		// Then extract: bitmap_left = mn.x, bitmap_top = -mn.y, height = mx.y - mn.y
		BitmapFont::Glyph glyph;
		glyph.character = ch;

		glyph.bounds = Aabb2(
			Vector2((float)slot->bitmap_left, (float)(-slot->bitmap_top)),
			Vector2((float)(slot->bitmap_left + bitmap.width), (float)(-slot->bitmap_top + bitmap.rows)));
		glyph.uvMin = Vector2(
			(float)(rect.x + 1) / textureSize,
			(float)(rect.y + 1) / textureSize);
		glyph.uvMax = Vector2(
			(float)(rect.x + 1 + bitmap.width) / textureSize,
			(float)(rect.y + 1 + bitmap.rows) / textureSize);
		glyph.advance = (float)(slot->advance.x >> 6);

		bitmapFont->addGlyph(glyph);
	}

	// Set baseline to maximum glyph baseline offset
	bitmapFont->setBaseline((float)baselineOffset);

	// Extract kerning pairs if the font has kerning information
	if (FT_HAS_KERNING(face))
	{
		log::info << L"Font has kerning information, extracting kerning pairs..." << Endl;
		uint32_t kerningPairsCount = 0;

		for (uint32_t left : characters)
		{
			FT_UInt leftIndex = FT_Get_Char_Index(face, left);
			if (leftIndex == 0)
				continue;

			for (uint32_t right : characters)
			{
				FT_UInt rightIndex = FT_Get_Char_Index(face, right);
				if (rightIndex == 0)
					continue;

				FT_Vector kerning;
				error = FT_Get_Kerning(face, leftIndex, rightIndex, FT_KERNING_DEFAULT, &kerning);
				if (error)
					continue;

				if (kerning.x != 0)
				{
					float kerningOffset = (float)(kerning.x >> 6);
					bitmapFont->addKerning(left, right, kerningOffset);
					kerningPairsCount++;
				}
			}
		}

		log::info << L"Extracted " << kerningPairsCount << L" kerning pairs." << Endl;
	}
	else
	{
		log::info << L"Font does not have kerning information." << Endl;
	}

	FT_Done_Face(face);
	FT_Done_FreeType(library);

#if defined(_DEBUG)
	atlasImage->save(L"PaperFont" + toString(fontAsset->getSize()) + L".png");
#endif

	// Build texture
	const Guid textureGuid = outputGuid.permutation(1);
	const std::wstring texturePath = Path(outputPath).getPathOnly() + L"/Textures/" + textureGuid.format();

	Ref< render::TextureOutput > textureOutput = new render::TextureOutput();
	textureOutput->m_textureFormat = render::TfInvalid;
	textureOutput->m_normalMap = false;
	textureOutput->m_scaleDepth = 0.0f;
	textureOutput->m_generateMips = false;
	textureOutput->m_keepZeroAlpha = false;
	textureOutput->m_textureType = render::Tt2D;
	textureOutput->m_hasAlpha = true;
	textureOutput->m_generateAlpha = false;  // Don't generate - we set alpha directly
	textureOutput->m_invertAlpha = false;
	textureOutput->m_ignoreAlpha = false;
	textureOutput->m_premultiplyAlpha = false;
	textureOutput->m_scaleImage = false;
	textureOutput->m_enableCompression = false;
	textureOutput->m_alphaCoverageReference = 0.5f;
	textureOutput->m_systemTexture = true;

	if (!pipelineBuilder->buildAdHocOutput(
			textureOutput,
			texturePath,
			textureGuid,
			atlasImage))
	{
		log::error << L"Failed to build font texture." << Endl;
		return false;
	}

	bitmapFont->setTextureId(textureGuid);

	// Save bitmap font
	Ref< db::Instance > instance = pipelineBuilder->createOutputInstance(outputPath, outputGuid);
	if (!instance)
	{
		log::error << L"Failed to create output instance." << Endl;
		return false;
	}

	instance->setObject(bitmapFont);

	if (!instance->commit())
	{
		log::error << L"Failed to commit instance." << Endl;
		return false;
	}

	return true;
}

Ref< ISerializable > BitmapFontPipeline::buildProduct(
	editor::IPipelineBuilder* pipelineBuilder,
	const db::Instance* sourceInstance,
	const ISerializable* sourceAsset,
	const Object* buildParams) const
{
	T_FATAL_ERROR;
	return nullptr;
}

}
