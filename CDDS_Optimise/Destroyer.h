#pragma once
#include "GameObject.h"

class Destroyer : public GameObject {
protected:
	float radius;
	int textureID;

public:
	Destroyer() {}

	Destroyer(Vector2 position, Vector2 velocity, TextureMap* ptr) {
		radius = 28;
		textureID = 1;
		Init(position, velocity, radius, ptr, textureID);
	}
};