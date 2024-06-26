#pragma once
#include "raylib.h"
#include "TextureMap.h"

class GameObject {
protected:
	Vector2 m_position;
	Vector2 m_velocity;
	float m_radius;

	TextureMap* textureMapPtr;
	int m_textureID;

	bool m_isDirty;		// indicates if we've already processed a collision response for this gameObject

public:
	GameObject();

	GameObject(const GameObject& copy);

	GameObject& operator=(const GameObject& copy);

	void Init(Vector2 position, Vector2 velocity, float radius, TextureMap* ptr, int m_textureID);
	void InitVecs(Vector2 position, Vector2 velocity);
	void Update(float dt);
	void Draw();

	float GetX() { return m_position.x; }
	float GetY() { return m_position.y; }
	void SetX(float x) { m_position.x = x; }
	void SetY(float y) { m_position.y = y; }

	const Vector2 GetPosition() const { return m_position; }
	void SetPosition(Vector2 position) { m_position = position; }

	const Vector2 GetVelocity() const { return m_velocity; }
	void SetVelocity(Vector2 velocity) { m_velocity = velocity; }

	float GetRadius() { return m_radius; }

	bool IsDirty() { return m_isDirty; }
	void SetDirty() { m_isDirty = true; }
};
