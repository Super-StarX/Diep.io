#pragma once

#include "Object.h"
#include "Turret.h"

class Player : public Object {
public:
	Player(float radius, const sf::Color& color, const point& position, int maxHealth);

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

	int getLevel() const { return level; }
	sf::Color getColor() const { return color; }
	int getDamage() const { return bulletDamage; }
	float getBulletSpeedMulti() const { return bulletSpeedMulti; }
	float getBulletPenetration() const { return bulletPenetration; }
	int getBulletDamage() const { return bulletDamage; }
	float getAccuracy() const { return accuracy; }

	void changeTurret(TurretTypes type);
	void setTurretRotation(float degress);
	void calcTurretRotation(const point& mousePos);
	bool checkShot();
	void resetShot();
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

	virtual void update();
	virtual bool isAI();
	virtual float getBulletRadius();
	virtual float getTurretRotation() const { return turretRotating; }

protected:
	virtual ObjectType WhatAmI();
	virtual void move(float x, float y);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
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
	float accuracy{ 20.f };//����׼��
	float recoil{ 3.f };//������
};
