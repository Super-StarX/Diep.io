#pragma once

#include "Object.h"
#include "Turret.h"

class Player : public Object {
public:
	Player(float radius, const sf::Color& color, const point& position, int maxHealth);
	Player(Player* owner);
	~Player();

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

	int getLevel() const { return level; }
	TurretTypes getType() const { return Type; }
	sf::Color getColor() const { return color; }
	int getDamage() const { return bulletDamage; }
	float getBulletSpeedMulti() const { return bulletSpeedMulti; }
	float getBulletPenetration() const { return bulletPenetration; }
	int getBulletDamage() const { return bulletDamage; }
	float getAccuracy() const { return accuracy; }
	float getBulletRadius();

	void changeTurret(TurretTypes type);
	void setTurretRotation(float degress);
	void calcTurretRotation(const point& mousePos);
	void setPosition(float x, float y);
	void reduceHealth(int amount);
	void checkMove(float moveSpeed);
	void checkCollision();
	bool fire(point target);

	void AddExp(int amount);
	int upgradeCount() const;
	void upgradeBulletSpeed(float amount);
	void upgradeBulletPenetration(float amount);
	void upgradeBulletDamage(int amount);
	//void upgradeHealthRegen();
	void upgradeReloadInterval(float amount);
	void upgradeMovementSpeed(float amount);

	virtual void update() override;
	virtual bool isAI() const { return false; }
	virtual float getTurretRotation() const { return turretRotating; }

protected:
	virtual ObjectType WhatAmI() const override { return ObjectType::Player; }
	virtual void checkFirendlyCollide(point vel) override;
	virtual void move(float x, float y) override;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	TurretTypes Type{ TurretTypes::Default };
	int level{ 1 };
	int remainUpgradeSkills{ 0 };
	int remainUpgradeType{ 0 };

	sf::Color color;
	sf::Text nameText;
	sf::RectangleShape healthBar;
	std::vector<Turret> turrets;
	std::vector<Player*> spawns;
	Player* owner;
	float turretRotating{ 0.f };

	float fireFrame{ 0.0f }; // 自上次射击以来经过的时间

	float bulletSpeedMulti{ 1.0f };
	float bulletPenetration{ 1.0f };
	int bulletDamage{ 2 };
	int HealthRegen { 0 };
	float rof{ 0.3f }; // 射击间隔，以秒为单位

	//hide data
	float accuracy{ 15.f };//不精准度
	float recoil{ 3.f };//后坐力
};
