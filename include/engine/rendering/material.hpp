#pragma once

class Texture;

class Material {
	public:
		Texture* diffuse;
		Texture* normalMap;
		Texture* materialMap;
		Texture* displacementMap;

		float displacementScale;
	private:
};

