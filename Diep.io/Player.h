#pragma once

#include "Object.h"
#include "Turret.h"

class Player : public Object {
public:
	Player(float radius, const sf::Color& color, const point& position, int maxHealth, int level);
	Player(Player* owner);
	~Player();

	int getLevel() const { return level; }
	TurretTypes getType() const { return Type; }
	sf::Color getColor() const { return color; }
	int getDamage() const { return bulletDamage; }
	float getBulletSpeed() const { return bulletSpeed; }
	float getBulletPenetration() const { return bulletPenetration; }
	int getBulletDamage() const { return bulletDamage; }
	float getAccuracy() const { return accuracy; }
	float getBulletRadius();

	void changeTurret(TurretTypes type);
	void setTurretRotation(float degress);
	void calcTurretRotation(const point& mousePos);
	void reduceHealth(int amount);
	void checkMove(float moveSpeed);
	void checkCollision();
	bool fire(point target);

	void setLevel(int amount);
	bool upgradeCount() const;
	void upgradeBulletSpeed(float amount);
	void upgradeBulletPenetration(float amount);
	void upgradeBulletDamage(int amount);
	//void upgradeHealthRegen();
	void upgradeReloadInterval(float amount);
	void upgradeMovementSpeed(float amount);
	void updateSpawns();

	virtual void updateDying() override;
	virtual void update() override;
	virtual void setPosition(float x, float y) override;
	virtual float getTurretRotation() const override { return turretRotating; }
	virtual float getMass() const { return owner ? 0 : getRadius() * getRadius(); }
	virtual void AddExp(int amount) override;
	virtual ObjectType WhatAmI() const override { return ObjectType::Player; }

protected:
	virtual void checkFirendlyCollide(point& vel) override;
	virtual void move(float x, float y) override;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	TurretTypes Type{ TurretTypes::Default };
	int level{ 1 };
	int remainUpgradeSkills{ 0 };
	int remainUpgradeType{ 0 };

	sf::Color color;
	sf::Text levelText;
	sf::RectangleShape healthBar;
	sf::RectangleShape healthBorder;
	std::vector<Turret> turrets;
	std::vector<Player*> spawns;
	Player* owner;
	float turretRotating{ 0.f };

	float fireFrame{ 0.0f }; // 自上次射击以来经过的时间

	float bulletSpeed{ 150.0f };
	float bulletPenetration{ 1.0f };
	int bulletDamage{ 2 };
	int HealthRegen { 0 };
	float rof{ 0.3f }; // 射击间隔，以秒为单位

	//hide data
	float accuracy{ 15.f };//不精准度
	float recoil{ 3.f };//后坐力
};
