#pragma once

#include "Object.h"
#include "Turret.h"

class Player : public Object {
public:
	Player(float radius, const sf::Color& color, const sf::Vector2f& position, int maxHealth);

	enum class TurretTypes
	{
		//---------0-5级
		Default,

		//---------5-15级
		Twin,//左右双发
		Sniper,//一发2倍伤害但是rof/2
		MachineGun,//开枪更快
		FlankGuard,//前后开火

		//---------15-40级
		//Double to
		TripleShot,//散发3发
		GuadTank,//4个方向

		//Sniper to
		Assassin,//一发4倍伤害但是rof/2
		Overseer,//母舰

		//MachineGun to
		Destoryer,//一发巨大的炮弹,这炮弹能顶更多发子弹
		Gunner,//小机枪

		//FlankGuard to
		TriAngle,//屁股方向2发
		Smasher,//创人

		//终极
		Triplet,//一次三发
		OctoTank,//8个方向开火

		Stalker,//一次6倍伤害加超级穿透,速度很快
		Overlord,//

		Annihilator,//
		Streamliner,//射速超快

		Booster,//移速超快
		Landmine,//静止隐形
		Spike//反弹子弹

	};

	virtual bool isAI();
	virtual float getBulletRadius();
	void changeTurret(TurretTypes type);
	float getTurretRotation();
	void setTurretRotation(float degress);
	void calcTurretRotation(const sf::Vector2f& mousePos);
	bool checkShot();
	void resetShot();
	void setPosition(float x, float y);
	void reduceHealth(int amount);
	void checkMove(float moveSpeed);
	void checkCollision();
	bool fire(sf::Vector2f target);
	void update();

	int getExp() const;
	void AddExp(int amount);
	int getLevel() const;
	int upgradeCount() const;
	sf::Color getColor() const;
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
	int remainUpgradeSkills{ 0 };
	int remainUpgradeType{ 0 };

	//sf::RectangleShape turret;
	sf::Color color;
	sf::RectangleShape healthBar;
	sf::Text nameText;
	std::vector<Turret> turrets;
	float turretRotating{ 0.f };
	TurretTypes Type{ TurretTypes::Default };

	float timeSinceLastShot{ 0.0f }; // 自上次射击以来经过的时间

	float bulletSpeedMulti{ 1.0f };
	float bulletPenetration{ 1.0f };
	int bulletDamage{ 10 };
	//int HealthRegen { 0 };
	//int BodyDamage;
	float reloadInterval{ 0.3f }; // 射击间隔，以秒为单位
	//float movementSpeed;MaxSpeed

	//hide data
	float accuracy{ 20.f };
	float recoil{ 3.f };
};
