#pragma once
#include "GameObject.h"

class Critter : public GameObject {
public:
	Critter() {}

	Critter(Vector2 position, Vector2 velocity, TextureMap* ptr) {
		Init(position, velocity, 14, ptr, 0);
	}
};

