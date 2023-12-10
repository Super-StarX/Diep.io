#pragma once

#include "Object.h"

class Player : public Object {
public:
	Player(float radius, const sf::Color& color, const sf::Vector2f& position, int maxHealth);

	virtual bool isAI();
	sf::Vector2f getTurretPosition() const;
	float getTurretRotation();
	void setTurretRotation(float degress);
	void calcTurretRotation(const sf::Vector2f& mousePos);
	bool checkShot();
	void resetShot();
	void setPosition(float x, float y);
	void reduceHealth(int amount);
	void checkMove(float moveSpeed);
	void checkCollision();
	bool fire(sf::Vector2f target, sf::Color color);
	void update();

	int getExp() const;
	void AddExp(int amount);
	int getLevel() const;
	int upgradeCount() const;
	int getDamage() const;
	float getBulletSpeedMulti() const;
	float getBulletPenetration() const;
	int getBulletDamage() const;

	void upgradeBulletSpeed(float amount);
	void upgradeBulletPenetration(float amount);
	void upgradeBulletDamage(int amount);
	//void upgradeHealthRegen();
	void upgradeReloadInterval(float amount);
	void upgradeMovementSpeed(float amount);

protected:
	virtual ObjectType WhatAmI();
	virtual void move(float x, float y);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	int exp{ 0 };
	int level{ 1 };
	int remainLevel{ 0 };

	sf::RectangleShape turret;
	sf::RectangleShape healthBar;
	sf::Text nameText;

	float timeSinceLastShot{ 0.0f }; // 自上次射击以来经过的时间

	float bulletSpeedMulti{ 1.0f };
	float bulletPenetration{ 1.0f };
	int bulletDamage{ 10 };
	//int HealthRegen { 0 };
	//int BodyDamage;
	float reloadInterval{ 0.3f }; // 射击间隔，以秒为单位
	//float movementSpeed;MaxSpeed
};
