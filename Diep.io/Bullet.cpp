#include "Bullet.h"
#include "AIPlayer.h"
#include "Data.h"
#include "Global.h"

Bullet::Bullet(Player* owner, point pos)
	: owner(owner),
	bulletSpeedMulti(owner->getBulletSpeedMulti()),
	bulletPenetration(owner->getBulletPenetration()),
	Object(owner->getBulletRadius(), owner->getColor(), pos, owner->getBulletDamage()) {
	setTeam(owner->getTeam());
}

bool Bullet::sameOwner(const Bullet* target) const {
	return owner == target->owner;
}

bool Bullet::isMyOwner(const Object* target) const {
	return owner == target;
}

bool Bullet::isAI() const {
	return owner->isAI();
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

	return getPosition() + vel * delta * bulletSpeedMulti;
}

void Bullet::updateMove() {
	float delta = Global::deltaTime;

	// 速度更新
	point vel = getVelocity();
	vel += getAcceleration() * delta;

	// 移动子弹
	move(vel.x * delta * bulletSpeedMulti, vel.y * delta * bulletSpeedMulti);

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

	if (curPos.x + x <= 0 ||
		curPos.x + x + bounds.width >= mapWidth ||
		curPos.y + y <= 0 ||
		curPos.y + y + bounds.height >= mapHeight)
		reduceHealth(10);

	Object::move(x, y);
}