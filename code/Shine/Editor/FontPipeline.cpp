/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <ft2build.h>
#include FT_FREETYPE_H
#include <Core/Io/FileSystem.h>
#include <Core/Io/IStream.h>
#include <Core/Log/Log.h>
#include <Core/Math/Vector2i.h>
#include <Core/Misc/Murmur3.h>
#include <Core/Misc/SafeDestroy.h>
#include <Core/Misc/String.h>
#include <Core/Misc/TString.h>
#include <Core/Settings/PropertyGroup.h>
#include <Core/Settings/PropertyString.h>
#include <Database/Instance.h>
#include <Drawing/Image.h>
#include <Drawing/PixelFormat.h>
#include <Editor/IPipelineBuilder.h>
#include <Editor/IPipelineDepends.h>
#include <Editor/IPipelineSettings.h>
#include <Render/Editor/Texture/TextureOutput.h>
#include "Shine/Font/BitmapFontResource.h"
#include "Shine/Editor/FontAsset.h"
#include "Shine/Editor/FontPipeline.h"

namespace traktor::shine
{
	namespace
	{

// Default character set if none specified
const wchar_t* c_defaultCharacterSet =
	L" !\"#$%&'()*+,-./0123456789:;<=>?@"
	L"ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
	L"abcdefghijklmnopqrstuvwxyz{|}~"
	L"\u00A0\u00A1\u00A2\u00A3\u00A4\u00A5\u00A6\u00A7\u00A8\u00A9\u00AA\u00AB\u00AC\u00AD\u00AE\u00AF"
	L"\u00B0\u00B1\u00B2\u00B3\u00B4\u00B5\u00B6\u00B7\u00B8\u00B9\u00BA\u00BB\u00BC\u00BD\u00BE\u00BF"
	L"\u00C0\u00C1\u00C2\u00C3\u00C4\u00C5\u00C6\u00C7\u00C8\u00C9\u00CA\u00CB\u00CC\u00CD\u00CE\u00CF"
	L"\u00D0\u00D1\u00D2\u00D3\u00D4\u00D5\u00D6\u00D7\u00D8\u00D9\u00DA\u00DB\u00DC\u00DD\u00DE\u00DF"
	L"\u00E0\u00E1\u00E2\u00E3\u00E4\u00E5\u00E6\u00E7\u00E8\u00E9\u00EA\u00EB\u00EC\u00ED\u00EE\u00EF"
	L"\u00F0\u00F1\u00F2\u00F3\u00F4\u00F5\u00F6\u00F7\u00F8\u00F9\u00FA\u00FB\u00FC\u00FD\u00FE\u00FF";

/*!
 * \brief Simple atlas packer using shelf algorithm
 */
class AtlasPacker
{
public:
	explicit AtlasPacker(int32_t size, int32_t padding)
	:	m_size(size)
	,	m_padding(padding)
	,	m_currentY(0)
	,	m_currentX(0)
	,	m_rowHeight(0)
	{
	}

	Vector2i pack(int32_t width, int32_t height)
	{
		// Add padding
		width += m_padding * 2;
		height += m_padding * 2;

		// Check if fits on current row
		if (m_currentX + width > m_size)
		{
			// Move to next row
			m_currentX = 0;
			m_currentY += m_rowHeight;
			m_rowHeight = 0;
		}

		// Check if fits in atlas at all
		if (m_currentY + height > m_size)
			return Vector2i(-1, -1); // Atlas full

		Vector2i pos(m_currentX + m_padding, m_currentY + m_padding);

		// Advance
		m_currentX += width;
		m_rowHeight = max(m_rowHeight, height);

		return pos;
	}

private:
	int32_t m_size;
	int32_t m_padding;
	int32_t m_currentY;
	int32_t m_currentX;
	int32_t m_rowHeight;
};

	}

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.shine.FontPipeline", 0, FontPipeline, editor::IPipeline)

bool FontPipeline::create(const editor::IPipelineSettings* settings, db::Database* database)
{
	m_assetPath = settings->getPropertyIncludeHash<std::wstring>(L"Pipeline.AssetPath", L"");
	return true;
}

void FontPipeline::destroy()
{
}

TypeInfoSet FontPipeline::getAssetTypes() const
{
	TypeInfoSet assetTypes;
	assetTypes.insert<FontAsset>();
	return assetTypes;
}

bool FontPipeline::shouldCache() const
{
	return true;
}

uint32_t FontPipeline::hashAsset(const ISerializable* sourceAsset) const
{
	const FontAsset* fontAsset = checked_type_cast<const FontAsset*>(sourceAsset);

	Murmur3 hash;
	hash.begin();

	// Hash font file path and content
	Path fontFile = Path(m_assetPath) + fontAsset->getFontFile();
	hash.feed(fontFile.getPathNameOS());

	Ref<IStream> stream = FileSystem::getInstance().open(fontFile, File::FmRead);
	if (stream)
	{
		uint8_t buffer[4096];
		int64_t nread;
		while ((nread = stream->read(buffer, sizeof(buffer))) > 0)
			hash.feedBuffer(buffer, (int)nread);
	}

	// Hash settings
	hash.feed(fontAsset->getFontSize());
	hash.feed(fontAsset->getCharacterSet());
	hash.feed(fontAsset->getAtlasSize());
	hash.feed(fontAsset->getAtlasPadding());
	hash.feed(fontAsset->getEnableSDF());
	hash.feed(fontAsset->getSDFSpread());
	hash.feed(fontAsset->getEnableAntialiasing());
	hash.feed(fontAsset->getEnableKerning());

	hash.end();
	return hash.get();
}

bool FontPipeline::buildDependencies(
	editor::IPipelineDepends* pipelineDepends,
	const db::Instance* sourceInstance,
	const ISerializable* sourceAsset,
	const std::wstring& outputPath,
	const Guid& outputGuid
) const
{
	const FontAsset* fontAsset = checked_type_cast<const FontAsset*>(sourceAsset);

	// Font file is an external dependency, tracked via hash in hashAsset()
	// No additional pipeline dependencies needed

	return true;
}

bool FontPipeline::buildOutput(
	editor::IPipelineBuilder* pipelineBuilder,
	const editor::PipelineDependencySet* dependencySet,
	const editor::PipelineDependency* dependency,
	const db::Instance* sourceInstance,
	const ISerializable* sourceAsset,
	const std::wstring& outputPath,
	const Guid& outputGuid,
	const Object* buildParams,
	uint32_t reason
) const
{
	const FontAsset* fontAsset = checked_type_cast<const FontAsset*>(sourceAsset);

	// Resolve font file path
	Path fontFile = Path(m_assetPath) + fontAsset->getFontFile();
	if (!FileSystem::getInstance().exist(fontFile))
	{
		log::error << L"Font file not found: " << fontFile.getPathNameOS() << Endl;
		return false;
	}

	// Initialize FreeType
	FT_Library ftLibrary;
	if (FT_Init_FreeType(&ftLibrary) != 0)
	{
		log::error << L"Failed to initialize FreeType" << Endl;
		return false;
	}

	// Load font face
	FT_Face ftFace;
	std::string fontFilePath = wstombs(fontFile.getPathNameOS());
	if (FT_New_Face(ftLibrary, fontFilePath.c_str(), 0, &ftFace) != 0)
	{
		log::error << L"Failed to load font: " << fontFile.getPathName() << Endl;
		FT_Done_FreeType(ftLibrary);
		return false;
	}

	// Set font size
	FT_Set_Pixel_Sizes(ftFace, 0, (FT_UInt)fontAsset->getFontSize());

	// Get font name
	std::wstring fontName = fontFile.getFileNameNoExtension();
	if (ftFace->family_name)
		fontName = mbstows(ftFace->family_name);

	// Determine character set
	std::wstring charSet = fontAsset->getCharacterSet();
	if (charSet.empty())
		charSet = c_defaultCharacterSet;

	// Create atlas image
	int32_t atlasSize = fontAsset->getAtlasSize();
	int32_t atlasPadding = fontAsset->getAtlasPadding();

	Ref<drawing::Image> atlasImage = new drawing::Image(
		drawing::PixelFormat::getR8(),
		atlasSize,
		atlasSize
	);
	atlasImage->clear(Color4f(0, 0, 0, 0));

	// Pack glyphs into atlas
	AtlasPacker packer(atlasSize, atlasPadding);
	AlignedVector<BitmapFontResource::Glyph> glyphs;
	glyphs.reserve(charSet.size());

	FT_Int32 loadFlags = FT_LOAD_DEFAULT;
	if (fontAsset->getEnableAntialiasing())
		loadFlags |= FT_LOAD_RENDER;
	else
		loadFlags |= FT_LOAD_RENDER | FT_LOAD_MONOCHROME;

	for (wchar_t ch : charSet)
	{
		// Load glyph
		if (FT_Load_Char(ftFace, ch, loadFlags) != 0)
		{
			log::warning << L"Failed to load glyph for character " << (int)ch << Endl;
			continue;
		}

		FT_GlyphSlot glyph = ftFace->glyph;

		// Pack into atlas
		Vector2i pos = packer.pack(glyph->bitmap.width, glyph->bitmap.rows);
		if (pos.x < 0)
		{
			log::warning << L"Atlas full, could not fit all glyphs" << Endl;
			break;
		}

		// Copy glyph bitmap to atlas
		for (uint32_t y = 0; y < glyph->bitmap.rows; ++y)
		{
			for (uint32_t x = 0; x < glyph->bitmap.width; ++x)
			{
				uint8_t pixel = glyph->bitmap.buffer[y * glyph->bitmap.pitch + x];
				atlasImage->setPixel(pos.x + x, pos.y + y, Color4f(pixel / 255.0f, 0, 0, 0));
			}
		}

		// Create glyph metrics
		BitmapFontResource::Glyph g;
		g.character = ch;
		g.width = (float)glyph->bitmap.width;
		g.height = (float)glyph->bitmap.rows;
		g.bearingX = (float)glyph->bitmap_left;
		g.bearingY = (float)glyph->bitmap_top;
		g.advance = (float)(glyph->advance.x >> 6);  // Convert from 26.6 fixed point

		// Calculate UV coordinates
		g.u0 = pos.x / (float)atlasSize;
		g.v0 = pos.y / (float)atlasSize;
		g.u1 = (pos.x + g.width) / (float)atlasSize;
		g.v1 = (pos.y + g.height) / (float)atlasSize;

		glyphs.push_back(g);
	}

	// Extract kerning pairs
	SmallMap<uint32_t, float> kerning;
	if (fontAsset->getEnableKerning() && FT_HAS_KERNING(ftFace))
	{
		for (wchar_t left : charSet)
		{
			FT_UInt leftIndex = FT_Get_Char_Index(ftFace, left);
			if (leftIndex == 0) continue;

			for (wchar_t right : charSet)
			{
				FT_UInt rightIndex = FT_Get_Char_Index(ftFace, right);
				if (rightIndex == 0) continue;

				FT_Vector delta;
				FT_Get_Kerning(ftFace, leftIndex, rightIndex, FT_KERNING_DEFAULT, &delta);

				if (delta.x != 0)
				{
					float kernValue = (float)(delta.x >> 6);  // Convert from 26.6 fixed point
					uint32_t key = (static_cast<uint32_t>(left) << 16) | static_cast<uint32_t>(right);
					kerning[key] = kernValue;
				}
			}
		}
	}

	// Get font metrics
	float fontSize = fontAsset->getFontSize();
	float lineHeight = (float)(ftFace->size->metrics.height >> 6);
	float ascent = (float)(ftFace->size->metrics.ascender >> 6);
	float descent = (float)(ftFace->size->metrics.descender >> 6);

	// Create atlas texture resource
	// TODO: Implement proper texture asset creation and building
	// For now, we'll skip the texture creation and just create the font resource
	Guid atlasTextureGuid = Guid::create();

	// TODO: Save atlas image and create texture asset properly
	log::warning << L"Font pipeline: Texture atlas creation not yet fully implemented" << Endl;

	// Create BitmapFontResource
	Ref<BitmapFontResource> font = new BitmapFontResource();
	font->create(
		fontName,
		fontSize,
		lineHeight,
		ascent,
		descent,
		glyphs,
		kerning,
		resource::Id<render::ITexture>(atlasTextureGuid)
	);

	// Commit Font resource
	Ref<db::Instance> instance = pipelineBuilder->createOutputInstance(outputPath, outputGuid);
	if (!instance)
	{
		log::error << L"Failed to create font instance" << Endl;
		FT_Done_Face(ftFace);
		FT_Done_FreeType(ftLibrary);
		return false;
	}

	instance->setObject(font);
	if (!instance->commit())
	{
		log::error << L"Failed to commit font instance" << Endl;
		FT_Done_Face(ftFace);
		FT_Done_FreeType(ftLibrary);
		return false;
	}

	// Cleanup
	FT_Done_Face(ftFace);
	FT_Done_FreeType(ftLibrary);

	log::info << L"Font pipeline: Created font \"" << fontName << L"\" with " << glyphs.size() << L" glyphs, " << kerning.size() << L" kerning pairs" << Endl;

	return true;
}

Ref<ISerializable> FontPipeline::buildProduct(
	editor::IPipelineBuilder* pipelineBuilder,
	const db::Instance* sourceInstance,
	const ISerializable* sourceAsset,
	const Object* buildParams
) const
{
	// FontPipeline uses buildOutput instead of buildProduct
	// Return nullptr to indicate buildOutput should be used
	return nullptr;
}

}
