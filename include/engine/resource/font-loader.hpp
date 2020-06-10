#pragma once

#include <engine/resource/resource-loader.hpp>

#include <engine/rendering/font.hpp>

class FontLoader final : public ResourceLoader<FontLoader, Font> {
    public:
        Memory::SharedPointer<Font> load(const StringView& fileName, uint32 fontSize) const {
			auto& context = RenderContext::ref();

            Memory::SharedPointer<Font> font = Memory::make_shared<Font>(context);

            if (!font->load(fileName, fontSize)) {
                return nullptr;
            }

            return font;
        }
    private:
};
