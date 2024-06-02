#pragma once
#include "GameObject.h"

class Destroyer : public GameObject {
public:
	Destroyer() {}

	Destroyer(Vector2 position, Vector2 velocity, TextureMap* ptr) {
		Init(position, velocity, 28, ptr, 1);
	}
};