#pragma once

#include <engine/resource/resource-loader.hpp>

#include <engine/rendering/material.hpp>

class MaterialLoader final : public ResourceLoader<MaterialLoader, Material> {
	public:
		Memory::SharedPointer<Material> load(Texture& diffuse, Texture& normal,
				Texture& material, Texture& depthMap, float displacementScale) const {
			auto mt = Memory::make_shared<Material>();
				mt->diffuse = &diffuse;
				mt->normalMap = &normal;
				mt->materialMap = &material;
				mt->displacementMap = &depthMap;
				mt->displacementScale = displacementScale;

			return mt;
		}
	private:
};
