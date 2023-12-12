#pragma once

#include "Object.h"

class Player;
class Bullet : public Object {
public:
	Bullet(Player* owner, point pos);

	float getBulletSpeed() const { return bulletDefaultSpeed; }
	int getDamage() const { return bulletDamage; }
	Player* getOwner() const { return owner; }

	bool sameOwner(const Bullet* target) const;
	bool isMyOwner(const Object* target) const;
	bool isAI() const;
	bool isVivid() const;
	void reduceHealth(int amount);
	void checkCollision();
	float evaluateDistance() const;
	point evaluatePosition();

	virtual void update();

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
