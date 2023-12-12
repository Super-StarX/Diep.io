#include "Bullet.h"
#include "AIPlayer.h"
#include "Data.h"
#include "Global.h"

Bullet::Bullet(Player* owner, point pos)
	: owner(owner),
	bulletSpeedMulti(owner->getBulletSpeedMulti()),
	bulletPenetration(owner->getBulletPenetration()),
	bulletDamage(owner->getBulletDamage()),
	Object(owner->getBulletRadius(), owner->getColor(), pos, owner->getBulletDamage()) {
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
	int health = getHealth() - amount;
	if (health > 0) {
		setHealth(health);
		bulletPenetration -= 1;
	}
	else {
		setHealth(0);
		bulletPenetration = 0;
	}
}

void Bullet::checkCollision() {
	// ����������ӵ�����ײ
	for (auto& bullet : bullets) {
		if (&bullet != this) {
			if (!sameOwner(&bullet) && collideWith(bullet)) {
				int damage = bullet.getDamage();
				bullet.reduceHealth(getDamage());
				reduceHealth(damage);
			}
		}
	}

	// �������ҵ���ײ
	if (!isMyOwner(&player) && collideWith(player)) {
		int health = player.getHealth();
		player.reduceHealth(getDamage());
		reduceHealth(health);
	}

	// �������Դ����ײ
	for (auto& resource : resources) {
		if (collideWith(resource)) {
			int health = resource.getHealth();
			resource.reduceHealth(getDamage());
			reduceHealth(health);

			if (resource.getHealth() <= 0)
				getOwner()->AddExp(500);
			else {
				// Ϊ��Դ��Ӻ�����
				auto newVelocity = resource.getVelocity() + 0.2f * getVelocity();
				resource.setVelocity(newVelocity);
			}
		}
	}

	// �������˵���ײ
	for (auto& enemy : enemies) {
		if (!isMyOwner(&enemy) && collideWith(enemy)) {
			int health = enemy.getHealth();
			enemy.reduceHealth(getDamage());
			reduceHealth(health);
			if (enemy.getHealth() <= 0)
				getOwner()->AddExp(2000);
		}
	}
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

void Bullet::update() {
	if (++life > bulletLife)
		reduceHealth(getHealth());

	float delta = Global::deltaTime;

	// �ٶȸ���
	point vel = getVelocity();
	vel += getAcceleration() * delta;

	// �ƶ��ӵ�
	move(vel.x * delta * bulletSpeedMulti, vel.y * delta * bulletSpeedMulti);

	float currentSpeed = vel.length();

	if (currentSpeed > getMaxSpeed())
		vel = (vel / currentSpeed) * getMaxSpeed();

	setVelocity(vel);
}

ObjectType Bullet::WhatAmI() {
	return ObjectType::Bullet;
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