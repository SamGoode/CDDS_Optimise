#pragma once
#include "GameObject.h"

class Critter : public GameObject {
protected:
	float radius;
	int textureID;

public:
	Critter() {}

	Critter(Vector2 position, Vector2 velocity, TextureMap* ptr) {
		radius = 14;
		textureID = 0;
		Init(position, velocity, radius, ptr, textureID);
	}
};

