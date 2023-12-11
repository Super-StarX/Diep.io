#include "Bullet.h"
#include "AIPlayer.h"

Bullet::Bullet(Player* owner, sf::Vector2f pos)
	: owner(owner),
	bulletSpeedMulti(owner->getBulletSpeedMulti()),
	bulletPenetration(owner->getBulletPenetration()),
	bulletDamage(owner->getBulletDamage()),
	Object(owner->getBulletRadius(), owner->getColor(), pos, owner->getBulletDamage()) {
}

float Bullet::getBulletSpeed() const {
	return bulletDefaultSpeed;
}

int Bullet::getDamage() const {
	return bulletDamage;
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

Player* Bullet::getOwner() const {
	return owner;
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
	// 检查与其他子弹的碰撞
	for (auto& bullet : bullets) {
		if (&bullet != this) {
			if (!sameOwner(&bullet) && collideWith(bullet)) {
				int damage = bullet.getDamage();
				bullet.reduceHealth(getDamage());
				reduceHealth(damage);
			}
		}
	}

	// 检查与玩家的碰撞
	if (!isMyOwner(&player) && collideWith(player)) {
		int health = player.getHealth();
		player.reduceHealth(getDamage());
		reduceHealth(health);
	}

	// 检查与资源的碰撞
	for (auto& resource : resources) {
		if (collideWith(resource)) {
			int health = resource.getHealth();
			resource.reduceHealth(getDamage());
			reduceHealth(health);

			if (resource.getHealth() <= 0)
				getOwner()->AddExp(500);
			else {
				// 为资源添加后坐力
				sf::Vector2f impactDirection = getVelocity();
				sf::Vector2f newVelocity = resource.getVelocity() + 0.2f * impactDirection;
				resource.setVelocity(newVelocity);
			}
		}
	}

	// 检查与敌人的碰撞
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
	sf::Vector2f vel = Velocity;
	float delta = Global::deltaTime;
	vel += Acceleration * delta;

	return std::sqrt(vel.x * vel.x + vel.y * vel.y);
}

sf::Vector2f Bullet::evaluatePosition() {
	sf::Vector2f vel = Velocity;
	float delta = Global::deltaTime;
	vel += Acceleration * delta;

	return getPosition() + Velocity * delta * bulletSpeedMulti;
}

void Bullet::update() {
	if (++life > bulletLife)
		reduceHealth(getHealth());

	float delta = Global::deltaTime;

	// 速度更新
	Velocity += Acceleration * delta;

	// 移动子弹
	move(Velocity.x * delta * bulletSpeedMulti, Velocity.y * delta * bulletSpeedMulti);

	float currentSpeed = std::sqrt(Velocity.x * Velocity.x + Velocity.y * Velocity.y);

	if (currentSpeed > MaxSpeed) {
		Velocity = (Velocity / currentSpeed) * MaxSpeed;
	}
}

ObjectType Bullet::WhatAmI() {
	return ObjectType::Bullet;
}

void Bullet::move(float x, float y) {
	sf::Vector2f curPos = body.getPosition();
	sf::FloatRect bounds = body.getGlobalBounds();

	if (curPos.x + x <= 0 ||
		curPos.x + x + bounds.width >= Global::mapWidth ||
		curPos.y + y <= 0 ||
		curPos.y + y + bounds.height >= Global::mapHeight)
		reduceHealth(10);

	Object::move(x, y);
}