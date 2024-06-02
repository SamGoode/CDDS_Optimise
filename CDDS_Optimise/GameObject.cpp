#include "GameObject.h"

GameObject::GameObject() {
	m_position = Vector2{ 0, 0 };
	m_velocity = Vector2{ 0, 0 };
	m_radius = 0;

	textureMapPtr = nullptr;
	m_textureID = 0;
}

GameObject::GameObject(const GameObject& copy) {
	m_position = copy.m_position;
	m_velocity = copy.m_velocity;
	m_radius = copy.m_radius;

	textureMapPtr = copy.textureMapPtr;
	m_textureID = copy.m_textureID;
}

GameObject& GameObject::operator=(const GameObject& copy) {
	m_position = copy.m_position;
	m_velocity = copy.m_velocity;
	m_radius = copy.m_radius;

	textureMapPtr = copy.textureMapPtr;
	m_textureID = copy.m_textureID;

	return *this;
}

void GameObject::Init(Vector2 position, Vector2 velocity, float radius, TextureMap* ptr, int textureID) {
	m_position = position;
	m_velocity = velocity;
	m_radius = radius;

	textureMapPtr = ptr;
	m_textureID = textureID;
}

void GameObject::InitVecs(Vector2 position, Vector2 velocity) {
	m_position = position;
	m_velocity = velocity;
}

void GameObject::Update(float dt) {
	m_position.x += m_velocity.x * dt;
	m_position.y += m_velocity.y * dt;

	m_isDirty = false;
}

void GameObject::Draw() {
	DrawTexture((*textureMapPtr)[m_textureID], m_position.x - m_radius, m_position.y - m_radius, WHITE);
	
	//DrawTexturePro((*textureMapPtr)[m_textureID], { 0, 0, m_radius*2, m_radius*2 - 1 }, { m_position.x - m_radius, m_position.y - m_radius, m_radius * 2, m_radius * 2 }, { 0, 0 }, 0, WHITE);
	//DrawCircle(m_position.x, m_position.y, 5, BLUE);
	//DrawCircleLines(m_position.x, m_position.y, m_radius, BLACK);
}