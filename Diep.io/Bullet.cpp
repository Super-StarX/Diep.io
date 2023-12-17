#include "Bullet.h"
#include "AIPlayer.h"
#include "Data.h"
#include "Global.h"

Bullet::Bullet(Player* owner, point pos)
	: owner(owner),
	bulletSpeed(owner->getBulletSpeed()),
	bulletPenetration(owner->getBulletPenetration()),
	Object(owner->getBulletRadius(), owner->getColor(), pos, owner->getBulletDamage()) {
	helper::add(bullets, this);
	setTeam(owner->getTeam());
}

Bullet::~Bullet() {
	helper::erase(bullets, this);
}

bool Bullet::sameOwner(const Bullet* target) const {
	return owner == target->owner;
}

bool Bullet::isMyOwner(const Object* target) const {
	return owner == target;
}

bool Bullet::isAI() const {
	return owner->WhatAmI() == ObjectType::AIPlayer;
}

bool Bullet::isVivid() const {
	return !bulletPenetration;
}

void Bullet::reduceHealth(int amount) {
	/*if (bulletPenetration && getHealth() - amount < 0) {
		amount = 0;
		--bulletPenetration;
	}*/
	Object::reduceHealth(amount);
}

float Bullet::evaluateDistance() const {
	point vel = getVelocity();
	float delta = Global::deltaTime;
	vel += getAcceleration() * delta;

	return vel.length();
}

point Bullet::evaluatePosition() {
	point vel = getVelocity();
	float delta = Global::deltaTime;
	vel += getAcceleration() * delta;

	return getPosition() + (vel + point(bulletSpeed, bulletSpeed)) * delta;
}

void Bullet::updateMove() {
	float delta = Global::deltaTime;

	// 速度更新
	point vel = getVelocity();
	vel += getAcceleration() * delta;
	vel += point(bulletSpeed, bulletSpeed);

	// 移动子弹
	move(vel.x * delta, vel.y * delta);

	float currentSpeed = vel.length();

	if (currentSpeed > getMaxSpeed())
		vel = (vel / currentSpeed) * getMaxSpeed();

	setVelocity(vel);
}

void Bullet::update() {
	if (++life > bulletLife)
		reduceHealth(getHealth());
	Object::update();
}

void Bullet::move(float x, float y) {
	point curPos = getPosition();
	sf::FloatRect bounds = getGlobalBounds();

	point next = curPos + point(x, y);
	if (!next.inMap())
		reduceHealth(10);

	Object::move(x, y);
}