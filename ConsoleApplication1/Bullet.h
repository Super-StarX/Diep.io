#pragma once

#include "Object.h"
#include "Player.h"

class Bullet : public Object {
public:
	Bullet(Player* owner, sf::Color color) : 
		owner(owner),
		bulletSpeedMulti(owner->bulletSpeedMulti), 
		bulletPenetration(owner->bulletPenetration),
		bulletDamage(owner->bulletDamage),
		Object(owner->bulletDamage / 2.f,
			color,
			owner->getPosition(),
			owner->bulletDamage) {
	}

	~Bullet() {
	}

	float getBulletSpeed() {
		return bulletDefaultSpeed;
	}
	
	int getDamage() {
		return bulletDamage;
	}
	
	void reduceHealth(int amount) {
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

	bool sameOwner(Bullet* target) {
		return owner == target->owner;
	}
	
	bool isMyOwner(Object* target) {
		return owner == target;
	}
	
	bool isAI() {
		return owner->isAI();
	}
	
	Player* getOwner() {
		return owner;
	}

	virtual bool isVivid() {
		return !bulletPenetration;
	}

	void checkCollision();

	float evaluateDistance() {
		sf::Vector2f vel = Velocity;
		float delta = Global::deltaTime;
		vel += Acceleration * delta;

		return std::sqrt(vel.x * vel.x + vel.y * vel.y);
	}
	
	sf::Vector2f evaluatePosition() {
		sf::Vector2f vel = Velocity;
		float delta = Global::deltaTime;
		vel += Acceleration * delta;

		return getPosition() + Velocity * delta * bulletSpeedMulti;
	}

	// 根据自身速度的移动
	void update() {
		float delta = Global::deltaTime;

		// 速度更新
		Velocity += Acceleration * delta;

		// 无摩擦
		//Velocity *= pow(Global::inertia, delta);
		
		// 移动子弹
		move(Velocity.x * delta * bulletSpeedMulti, Velocity.y * delta * bulletSpeedMulti);

		float currentSpeed = std::sqrt(Velocity.x * Velocity.x + Velocity.y * Velocity.y);

		if (currentSpeed > MaxSpeed) {
			Velocity = (Velocity / currentSpeed) * MaxSpeed;
		}

	}
protected:
	virtual void move(float x, float y) {
		sf::Vector2f curPos = body.getPosition();
		sf::FloatRect bounds = body.getGlobalBounds();

		if (curPos.x + x <= 0 ||
			curPos.x + x + bounds.width >= Global::mapWidth ||
			curPos.y + y <= 0 ||
			curPos.y + y + bounds.height >= Global::mapHeight)
			reduceHealth(10);

		Object::move(x, y);
	}

	Player* owner;
	float bulletDefaultSpeed{ 150.0f };
	float bulletSpeedMulti{ 1.0f };
	float bulletPenetration{ 0.0f };
	int bulletDamage{ 0 };
};