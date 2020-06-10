#pragma once

#include <engine/core/common.hpp>
#include <engine/core/string-view.hpp>
#include <engine/core/hash-map.hpp>

#include <engine/rendering/texture.hpp>

class Font {
	public:
		struct Character {
			float texCoordData[4]; // posX, posY, offsetX, offsetY

			float sizeX;
			float sizeY;
			
			int32 bearingX;
			int32 bearingY;
			
			float advance;
		};

		Font(RenderContext& context);

		bool load(const StringView& fileName, uint32 fontSize);

		Texture& getTexture();

		int32 getWidth() const;
		int32 getHeight() const;

		const Character& getCharacter(char c);

		~Font();
	private:
		NULL_COPY_AND_ASSIGN(Font);

		uint32 fontSize;
		HashMap<char, Character> characters;

		Texture texture;
};
