#include "engine/rendering/font.hpp"

#include <engine/core/memory.hpp>

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#include <algorithm>

namespace {
	constexpr const char MIN_CHAR = ' '; // ASCII 32 // TODO: ASCII 33 is !, check if I should exempt space
	constexpr const char MAX_CHAR = '~'; // ASCII 126

	constexpr const uint32 CHAR_TEXTURE_HEIGHT = 256;

	struct CharacterData {
		char symbol;
		
		uint32 width;
		uint32 height;
		uint8* data;

		int32 bearingX;
		int32 bearingY;

		uint32 advance;
	};

	bool loadCharacter(FT_Face& face, char c, ArrayList<CharacterData>& charData);
	uint32 leastPowerOfTwo(uint32 n);

	void blit(uint8* dest, const uint8* src, int32 destX, int32 destY,
			int32 srcWidth, int32 srcHeight,
			int32 destWidth, int32 destHeight);
};

Font::Font(RenderContext& context)
		: texture(context, 1, 1, GL_RED, nullptr, GL_RED) {}

bool Font::load(const StringView& fileName, uint32 fontSize) {
	FT_Library lib;
	FT_Face face;

	if (FT_Init_FreeType(&lib) != 0) {
		DEBUG_LOG("Font Loader", LOG_ERROR, "Failed to load FreeType");
		return false;
	}

	if (FT_New_Face(lib, fileName.data(), 0, &face) != 0) {
		DEBUG_LOG("Font Loader", LOG_ERROR, "Failed to load font file: %s",
				fileName.data());
		return false;
	}

	FT_Set_Pixel_Sizes(face, 0, fontSize);

	ArrayList<CharacterData> charData;
	uint32 totalWidth = 0;

	for (char c = MIN_CHAR; c <= MAX_CHAR; ++c) {
		if (!::loadCharacter(face, c, charData)) {
			DEBUG_LOG("Font Loader", LOG_ERROR, "Error loading font char %c for font %s",
					c, fileName.data());
		}

		totalWidth += charData.back().width;
	}

	std::sort(charData.begin(), charData.end(), [](auto& a, auto& b) {
		return a.width > b.width;
	});

	uint32 numRows = CHAR_TEXTURE_HEIGHT / fontSize;
	uint32 rowWidthPx = ::leastPowerOfTwo(totalWidth / numRows);

	uint8* fontBitmap = static_cast<uint8*>(Memory::malloc(rowWidthPx * CHAR_TEXTURE_HEIGHT));
	Memory::memset(fontBitmap, 0, rowWidthPx * CHAR_TEXTURE_HEIGHT);

	uint32 rowX = 0;
	uint32 rowY = 0;

	const float invWidth = 1.f / static_cast<float>(rowWidthPx);
	const float invHeight = 1.f / static_cast<float>(CHAR_TEXTURE_HEIGHT);

	for (auto& cd : charData) {
		if (rowX + cd.width > rowWidthPx) {
			rowX = 0;
			rowY += fontSize;
		}

		::blit(fontBitmap, cd.data, rowX, rowY, cd.width, cd.height,
				rowWidthPx, CHAR_TEXTURE_HEIGHT);

		Font::Character chr;

		chr.bearingX = cd.bearingX;
		chr.bearingY = cd.bearingY;
		chr.advance = static_cast<float>(cd.advance >> 6);

		chr.sizeX = static_cast<float>(cd.width);
		chr.sizeY = static_cast<float>(cd.height);

		chr.texCoordData[0] = static_cast<float>(rowX) * invWidth;
		chr.texCoordData[1] = static_cast<float>(rowY) * invHeight;
		chr.texCoordData[2] = static_cast<float>(cd.width) * invWidth;
		chr.texCoordData[3] = static_cast<float>(cd.height) * invHeight;

		characters.emplace(cd.symbol, chr);

		rowX += cd.width;

		Memory::free(cd.data);
	}

	texture.setImage(rowWidthPx, CHAR_TEXTURE_HEIGHT, fontBitmap);

	Memory::free(fontBitmap);

	FT_Done_Face(face);
	FT_Done_FreeType(lib);

	return true;
}

Texture& Font::getTexture() {
	return texture;
}

int32 Font::getWidth() const {
	return static_cast<int32>(texture.getWidth());
}

int32 Font::getHeight() const {
	return static_cast<int32>(texture.getHeight());
}

const Font::Character& Font::getCharacter(char c) {
	return characters[c];
}

Font::~Font() {
}

namespace {
	bool loadCharacter(FT_Face& face, char c, ArrayList<CharacterData>& charData) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0) {
			return false;
		}

		uint32 width = static_cast<uint32>(face->glyph->bitmap.width);
		uint32 height = static_cast<uint32>(face->glyph->bitmap.rows);

		CharacterData cd;

		cd.symbol = c;

		cd.width = width;
		cd.height = height;
		cd.data = static_cast<uint8*>(Memory::malloc(width * height));

		cd.bearingX = static_cast<int32>(face->glyph->bitmap_left);
		cd.bearingY = static_cast<int32>(face->glyph->bitmap_top);

		cd.advance = static_cast<uint32>(face->glyph->advance.x);
		
		Memory::memcpy(cd.data, face->glyph->bitmap.buffer, width * height);

		charData.push_back(cd);

		return true;
	}

	uint32 leastPowerOfTwo(uint32 n) {
		uint32 res = 1;

		while (n) {
			n >>= 1;
			res <<= 1;
		}

		return res;
	}

	void blit(uint8* dest, const uint8* src, int32 destX, int32 destY,
			int32 srcWidth, int32 srcHeight,
			int32 destWidth, int32 destHeight) {
		// TODO: make this algorithm less bad
		for (uint32 y = 0; y < srcHeight; ++y) {
			for (uint32 x = 0; x < srcWidth; ++x) {
				if (src[y * srcWidth + x] > 0) {
					dest[destWidth * (destY + y) + destX + x] = src[y * srcWidth + x];
				}
			}
		}
	}
};
