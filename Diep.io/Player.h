#pragma once

#include "Object.h"
#include "Turret.h"

class Player : public Object {
public:
	Player(float radius, const sf::Color& color, const sf::Vector2f& position, int maxHealth);

	enum class TurretTypes
	{
		//---------0-5��
		Default,

		//---------5-15��
		Twin,//����˫��
		Sniper,//һ��2���˺�����rof/2
		MachineGun,//��ǹ����
		FlankGuard,//ǰ�󿪻�

		//---------15-40��
		//Double to
		TripleShot,//ɢ��3��
		GuadTank,//4������

		//Sniper to
		Assassin,//һ��4���˺�����rof/2
		Overseer,//ĸ��

		//MachineGun to
		Destoryer,//һ���޴���ڵ�,���ڵ��ܶ����෢�ӵ�
		Gunner,//С��ǹ

		//FlankGuard to
		TriAngle,//ƨ�ɷ���2��
		Smasher,//����

		//�ռ�
		Triplet,//һ������
		OctoTank,//8�����򿪻�

		Stalker,//һ��6���˺��ӳ�����͸,�ٶȺܿ�
		Overlord,//

		Annihilator,//
		Streamliner,//���ٳ���

		Booster,//���ٳ���
		Landmine,//��ֹ����
		Spike//�����ӵ�

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

	float timeSinceLastShot{ 0.0f }; // ���ϴ��������������ʱ��

	float bulletSpeedMulti{ 1.0f };
	float bulletPenetration{ 1.0f };
	int bulletDamage{ 10 };
	//int HealthRegen { 0 };
	//int BodyDamage;
	float reloadInterval{ 0.3f }; // ������������Ϊ��λ
	//float movementSpeed;MaxSpeed

	//hide data
	float accuracy{ 20.f };
	float recoil{ 3.f };
};
