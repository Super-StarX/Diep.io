#pragma once
#include "Player.h"

class Bullet : public Object {
public:
	Bullet(Player* owner, point pos);

	float getBulletSpeed() const { return bulletDefaultSpeed; }
	Player* getOwner() const { return owner; }

	bool sameOwner(const Bullet* target) const;
	bool isMyOwner(const Object* target) const;
	bool isAI() const;
	bool isVivid() const;
	float evaluateDistance() const;
	point evaluatePosition();

	virtual void reduceHealth(int amount) override;
	virtual void updateMove() override;
	virtual void update() override;

protected:
	virtual ObjectType WhatAmI() const override { return ObjectType::Bullet; }
	virtual void move(float x, float y) override;

private:
	Player* owner{ nullptr };
	int life{ 0 };
	float bulletDefaultSpeed{ 150.0f };
	float bulletSpeedMulti{ 1.0f };
	float bulletPenetration{ 0.0f };
};
