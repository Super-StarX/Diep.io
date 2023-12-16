#pragma once
#include <SFML/Graphics.hpp>
#include <sstream>
#include "Data.h"

/*
Object
Resource 有碰撞 减血 惯性移动
Player 有碰撞 减血 惯性移动 炮塔
AIPlayer 有碰撞 减血 惯性移动 炮塔 思考
Bullet 检测所有resource和player的碰撞
*/

class Object : public sf::Drawable {
public:
	//创建单位
	Object(float radius, const sf::Color& color, const point& position, int maxHealth);
	//创建资源
	Object(ResourceType type, const point& position);
	//创建子机
	Object(const sf::Color& color, const point& position, int maxHealth);
	~Object();

	const int getID() const { return ID; }
	point getPosition() const { return isPolygon ? polygonBody.getPosition() : body.getPosition(); }
	sf::FloatRect getGlobalBounds() const { return isPolygon ? polygonBody.getGlobalBounds() : body.getGlobalBounds(); }
	const point& getVelocity() const { return Velocity; }
	float getSpeed() const { return Velocity.length(); }
	float getMaxSpeed() const { return MaxSpeed; }
	const point& getAcceleration() const { return Acceleration; }
	float getMaxAcceleration() const { return MaxAcceleration; }
	float getRadius() const { return isPolygon ? polygonRadius : body.getRadius(); }
	float getMass() const { return getRadius() * getRadius(); }
	int getMaxHealth() const { return maxHealth; }
	int getHealth() const { return currentHealth; }
	int getExp() const { return exp; }
	int getTeam() const { return team; }
	int getBodyDamage() const { return bodyDamage; }
	bool isDie() const { return isDied; }

	void setID(int id) { ID = id; }
	void setRadius(float radius) { body.setRadius(radius); }
	void setMaxSpeed(float spd) { MaxSpeed = spd; }
	void setVelocity(const point& vel) { Velocity = vel; }
	void setAcceleration(const point& vel) { Acceleration = vel; }
	void setRotation(float degress);
	void setHealth(int health) { currentHealth = health; }
	void setExp(int amount) { exp = amount; }
	void setTeam(int idx) { team = idx; }
	bool isCollideWith(const Object* other, float extraDistance = 0.f) const;
	void collision(Object* other);
	void randomAddToMap();

	virtual void updateMove();
	virtual void update();
	virtual void setPosition(float x, float y);
	virtual void reduceHealth(int amount);
	virtual ObjectType WhatAmI() const { return ObjectType::Object; }
	virtual float getTurretRotation() const { return 0; }
	virtual void AddExp(int amount) { return; }
	virtual point evaluatePosition() { return getPosition(); }

	static ResourceType randomResourceType();
protected:
	virtual void checkFirendlyCollide(point vel) { return; }
	virtual void move(float x, float y);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::Text healthText;
	sf::CircleShape body;
	sf::ConvexShape polygonBody;
private:

	int ID{ 0 };
	int team{ 0 };//资源是0=中立,蓝1红2
	int exp{ 0 };
	int maxHealth{ 0 };
	int currentHealth{ 0 };
	int bodyDamage{ 50 };
	float polygonRadius{ 0.f };
	point Acceleration{ 0,0 };
	point Velocity{ 0,0 };
	float MaxAcceleration{ 400.f };
	float MaxSpeed{ 150.f };

	bool isDied{ false };
	bool isResource{ false };
	bool isPolygon{ false };
};