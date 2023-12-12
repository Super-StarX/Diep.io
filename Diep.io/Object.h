#pragma once
#include <SFML/Graphics.hpp>
#include <sstream>
#include "Data.h"

template <class T>
void setOrigin(T shape);
ResourceType randomResourceType();

enum class ObjectType {
	Object,
	Player,
	AIPlayer,
	Bullet,
};

/*
Object
Resource ÓÐÅö×² ¼õÑª ¹ßÐÔÒÆ¶¯
Player ÓÐÅö×² ¼õÑª ¹ßÐÔÒÆ¶¯ ÅÚËþ
AIPlayer ÓÐÅö×² ¼õÑª ¹ßÐÔÒÆ¶¯ ÅÚËþ Ë¼¿¼
Bullet ¼ì²âËùÓÐresourceºÍplayerµÄÅö×²
*/

class Object : public sf::Drawable {
public:
	Object(float radius, const sf::Color& color, const point& position, int maxHealth);
	Object(ResourceType type, const point& position);
	~Object();
	
	const int getID() const { return ID; }
	point getPosition() const { return isPolygon ? polygonBody.getPosition() : body.getPosition(); }
	sf::FloatRect getGlobalBounds() const { return isPolygon ? polygonBody.getGlobalBounds() : body.getGlobalBounds(); }
	const point& getVelocity() const { return Velocity; }
	const point& getAcceleration() const { return Acceleration; }
	float getSpeed() const { return std::sqrt(Velocity.x * Velocity.x + Velocity.y * Velocity.y); }
	float getMaxSpeed() const { return MaxSpeed; }
	float getRadius() const { return isPolygon ? polygonRadius : body.getRadius(); }
	int getmaxHealth() const { return maxHealth; }
	int getHealth() const { return currentHealth; }
	int getExp() const { return exp; }

	void setID(int id);
	void setMaxSpeed(float spd);
	void setVelocity(const point& vel);
	void setAcceleration(const point& vel);
	void setPosition(float x, float y);
	void setExp(int amount) { exp = amount; }
	bool collideWith(Object& other, float extraDistance);
	bool collideWith(const Object& other) const;
	void randomAddToMap();
	void reduceHealth(int amount);
	void setHealth(int health);

	virtual void update();
	virtual bool isVivid();
	virtual ObjectType WhatAmI();
	virtual float getTurretRotation() const { return 0; }

protected:
	virtual void move(float x, float y);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	int ID;
	bool isPolygon{ false };
	sf::CircleShape body;
	sf::ConvexShape polygonBody;
	float polygonRadius{ 0.f };
	int exp{ 0 };
	int maxHealth{ 0 };
	int currentHealth{ 0 };
	sf::Text healthText;
	point Acceleration;
	point Velocity;
	float MaxSpeed{ 150.0f };
};