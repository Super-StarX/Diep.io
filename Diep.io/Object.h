#pragma once
#include <SFML/Graphics.hpp>
#include <sstream>
#include "Global.h"

template <class T>
void setOrigin(T shape);

enum class ObjectType {
	Object,
	Player,
	AIPlayer,
	Bullet,
};

/*
Object
Resource 有碰撞 减血 惯性移动
Player 有碰撞 减血 惯性移动 炮塔
AIPlayer 有碰撞 减血 惯性移动 炮塔 思考
Bullet 检测所有resource和player的碰撞
*/

class Object : public sf::Drawable {
public:
	Object(float radius, const sf::Color& color, const sf::Vector2f& position, int maxHealth);
	~Object();

	sf::FloatRect getGlobalBounds();
	void setID(int id);
	const int getID() const;
	void setVelocity(const sf::Vector2f& vel);
	const sf::Vector2f& getVelocity() const;
	void setAcceleration(const sf::Vector2f& vel);
	const sf::Vector2f& getAcceleration() const;
	float getSpeed();
	float getMaxSpeed();
	void setMaxSpeed(float spd);
	float getRadius();
	sf::Vector2f getPosition();
	void setPosition(float x, float y);
	bool collideWith(Object& other, float extraDistance);
	bool collideWith(const Object& other) const;
	void randomAddToMap();
	void reduceHealth(int amount);
	int getHealth() const;
	void setHealth(int health);
	void update();
	virtual bool isVivid();
	virtual ObjectType WhatAmI();
	virtual float getTurretRotation();

protected:
	virtual void move(float x, float y);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	int ID;
	sf::CircleShape body;
	int maxHealth;
	int currentHealth;
	sf::Text healthText;
	sf::Vector2f Acceleration;
	sf::Vector2f Velocity;
	float MaxSpeed{ 150.0f };
};