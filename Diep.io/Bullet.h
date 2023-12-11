#pragma once

#include "Object.h"

static int bulletLife = 300;
class Bullet : public Object {
public:
	Bullet(Player* owner, sf::Vector2f pos);

	float getBulletSpeed() const;
	int getDamage() const;
	bool sameOwner(const Bullet* target) const;
	bool isMyOwner(const Object* target) const;
	bool isAI() const;
	Player* getOwner() const;
	bool isVivid() const;
	void reduceHealth(int amount);
	void checkCollision();
	float evaluateDistance() const;
	sf::Vector2f evaluatePosition();
	void update();

protected:
	virtual ObjectType WhatAmI() override;
	virtual void move(float x, float y) override;

private:
	Player* owner;
	int life{ 0 };
	float bulletDefaultSpeed{ 150.0f };
	float bulletSpeedMulti{ 1.0f };
	float bulletPenetration{ 0.0f };
	int bulletDamage{ 0 };
};
