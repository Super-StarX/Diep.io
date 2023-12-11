#include <iostream>
#include "Player.h"

Player::Player(float radius, const sf::Color& color, const sf::Vector2f& position, int maxHealth) :
	Object(radius, color, position, maxHealth), color(color) {
	changeTurret(TurretTypes::Triplet);
	healthBar.setSize(sf::Vector2f(radius * 2.0f, 4.0f));
	healthBar.setFillColor(sf::Color::Red);
	healthBar.setOrigin(healthBar.getSize().x / 2, 20);
	healthBar.setPosition(body.getPosition().x, body.getPosition().y);
}

int Player::getExp() const {
	return exp;
}

void Player::AddExp(int amount) {
	if (level > 9)
		return;
	static int const nextLevelExp[] = { 1000,2000,3000,4000,5000,6000,7000,8000,9000,100000 };
	exp += amount;
	int need = nextLevelExp[level];
	if (exp >= need) {
		int count = exp / 1000;
		remainUpgradeSkills += 1;
		level += 1;
		exp -= need;
		if (exp >= need)
			this->AddExp(amount - need);
	}
}

int Player::getLevel() const {
	return level;
}

int Player::upgradeCount() const {
	return remainUpgradeSkills > 0;
}

bool Player::isAI() {
	return false;
}

void Player::changeTurret(TurretTypes type) {
	turrets.clear();
	float radius = getRadius();//15.f
	switch (type)
	{
	case Player::TurretTypes::Default:
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, sf::Vector2f{ 0,20 }, color, 0, 1));
		break;
		//初级
	case Player::TurretTypes::Twin:
		turrets.emplace_back(Turret(this, radius * 0.65f, radius * 2.2f, sf::Vector2f{ -10,20 }, color, 0, 1));
		turrets.emplace_back(Turret(this, radius * 0.65f, radius * 2.2f, sf::Vector2f{ 10,20 }, color, 0, 1));
		break;
	case Player::TurretTypes::Sniper:
		turrets.emplace_back(Turret(this, radius * 0.65f, radius * 2.8f, sf::Vector2f{ 0,40 }, color, 0, 1));
		break;
	case Player::TurretTypes::MachineGun:
		turrets.emplace_back(Turret(this, radius * 0.95f, radius * 2.2f, sf::Vector2f{ 0,20 }, color, 0, 1));
		break;
	case Player::TurretTypes::FlankGuard:
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, sf::Vector2f{ 0,20 }, color, 0, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 1.8f, sf::Vector2f{ 0,20 }, color, 180, 1));
		break;
		//中级
	case Player::TurretTypes::TripleShot:
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, sf::Vector2f{ 0,20 }, color, -45, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, sf::Vector2f{ 0,20 }, color, 0, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, sf::Vector2f{ 0,20 }, color, 45, 1));
		break;
	case Player::TurretTypes::GuadTank:
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, sf::Vector2f{ 0,20 }, color, 0, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, sf::Vector2f{ 0,20 }, color, 90, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, sf::Vector2f{ 0,20 }, color, 180, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, sf::Vector2f{ 0,20 }, color, 270, 1));
		break;
	case Player::TurretTypes::Assassin:
		break;
	case Player::TurretTypes::Overseer:
		break;
	case Player::TurretTypes::Destoryer:
		break;
	case Player::TurretTypes::Gunner:
		break;
	case Player::TurretTypes::TriAngle:
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, sf::Vector2f{ 0,20 }, color, 0, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 1.8f, sf::Vector2f{ 0,20 }, color, -160, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 1.8f, sf::Vector2f{ 0,20 }, color, 160, 1));
		break;
	case Player::TurretTypes::Smasher:
		break;
		//高级
	case Player::TurretTypes::Triplet:
		turrets.emplace_back(Turret(this, radius * 0.65f, radius * 1.8f, sf::Vector2f{ -11,20 }, color, 0, 1));
		turrets.emplace_back(Turret(this, radius * 0.65f, radius * 1.8f, sf::Vector2f{ 11,20 }, color, 0, 1));
		turrets.emplace_back(Turret(this, radius * 0.75f, radius * 2.2f, sf::Vector2f{ 0,20 }, color, 0, 1));
		break;
	case Player::TurretTypes::OctoTank:
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, sf::Vector2f{ 0,20 }, color, 0, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, sf::Vector2f{ 0,20 }, color, 45, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, sf::Vector2f{ 0,20 }, color, 90, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, sf::Vector2f{ 0,20 }, color, 135, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, sf::Vector2f{ 0,20 }, color, 180, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, sf::Vector2f{ 0,20 }, color, -45, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, sf::Vector2f{ 0,20 }, color, -90, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, sf::Vector2f{ 0,20 }, color, -135, 1));
		break;
	case Player::TurretTypes::Stalker:
		break;
	case Player::TurretTypes::Overlord:
		break;
	case Player::TurretTypes::Annihilator:
		break;
	case Player::TurretTypes::Streamliner:
		break;
	case Player::TurretTypes::Booster:
		break;
	case Player::TurretTypes::Landmine:
		break;
	case Player::TurretTypes::Spike:
		break;
	default:
		break;
	}
}

float Player::getTurretRotation() {
	return turretRotating;
}

void Player::setTurretRotation(float degress) {
	turretRotating = degress;
	for (auto& turret : turrets)
		turret.setRotation(degress);
}

void Player::calcTurretRotation(const sf::Vector2f& mousePos) {
	sf::Vector2f turretPos = getPosition();
	sf::Vector2f direction = mousePos - turretPos;

	// 计算夹角弧度
	float radians = std::atan2(direction.y, direction.x);

	// 转换为度数并设置炮台旋转角度
	float degrees = radians * (180.0f / 3.14159265f) + 90.0f; // 注意 +90 表示朝着鼠标
	setTurretRotation(degrees);
}

bool Player::checkShot() {
	// 检查是否满足射击条件
	return timeSinceLastShot > reloadInterval;
}

void Player::resetShot() {
	timeSinceLastShot = 0.0;
}

void Player::setPosition(float x, float y) {
	Object::setPosition(x, y);

	for (auto& turret : turrets)
		turret.setPosition(x, y);
	healthBar.setPosition(x, y);
}

void Player::reduceHealth(int amount) {
	Object::reduceHealth(amount);

	// 重新计算血条
	float percentage = static_cast<float>(currentHealth) / maxHealth;
	healthBar.setSize(sf::Vector2f(healthBar.getSize().x * percentage, healthBar.getSize().y));

	// 检测是否游戏结束
	if (!currentHealth && &Global::currentPlayer == this) {
		Global::isGameOver = true;
	}
}

void Player::checkMove(float moveSpeed) {
	bool noXAcceleration = true;
	bool noYAcceleration = true;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		if (Acceleration.x > 0)
			Acceleration.x = 0.0f;
		Acceleration.x = -400.0f;
		noXAcceleration = false;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		if (Acceleration.x < 0)
			Acceleration.x = 0.0f;
		Acceleration.x = 400.0f;
		noXAcceleration = false;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		if (Acceleration.y > 0)
			Acceleration.y = 0.0f;
		Acceleration.y = -400.0f;
		noYAcceleration = false;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		if (Acceleration.y < 0)
			Acceleration.y = 0.0f;
		Acceleration.y = 400.0f;
		noYAcceleration = false;
	}
	if (noXAcceleration) {
		Acceleration.x = 0.0f;
	}
	if (noYAcceleration) {
		Acceleration.y = 0.0f;
	}
}

void Player::checkCollision() {
	// 检查与资源的碰撞
	for (auto& resource : resources) {
		if (collideWith(resource)) {
			int health = getHealth();
			reduceHealth(resource.getHealth());
			resource.reduceHealth(health);
			if (resource.getHealth() <= 0)
				AddExp(500);
		}
	}
}

float Player::getBulletRadius(){
	int damage = this->getBulletDamage();
	if (damage > 15)
		return 6.f;
	else if (damage > 25)
		return 7.f;
	else if (damage > 40)
		return 8.f;
	else if (damage > 60)
		return 9.f;
	else if (damage > 100)
		return 10.f;
	return 5.f;
}

bool Player::fire(sf::Vector2f target) {
	if (checkShot()) {
		for (auto& turret : turrets) {
			auto backVel = turret.fire(this, target);
			setVelocity(getVelocity() - backVel);
		}
		resetShot();
		return true;
	}
	return false;
}

void Player::update() {
	Object::update();
	//rof更新
	timeSinceLastShot += Global::deltaTime;
}

sf::Color Player::getColor() const {
	return color;
}

int Player::getDamage() const {
	return bulletDamage;
}

float Player::getBulletSpeedMulti() const {
	return bulletSpeedMulti;
}

float Player::getBulletPenetration() const {
	return bulletPenetration;
}

int Player::getBulletDamage() const {
	return bulletDamage;
}

void Player::upgradeBulletSpeed(float amount) {
	if (!remainUpgradeSkills)
		return;
	bulletSpeedMulti += amount;
	remainUpgradeSkills--;
	std::cout << "Upgrade Bullet Speed" << std::endl;
}

void Player::upgradeBulletPenetration(float amount) {
	if (!remainUpgradeSkills)
		return;
	bulletPenetration += amount;
	remainUpgradeSkills--;
	std::cout << "Upgrade Bullet Penetration" << std::endl;
}

void Player::upgradeBulletDamage(int amount) {
	if (!remainUpgradeSkills)
		return;
	bulletDamage += amount;
	remainUpgradeSkills--;
	std::cout << "Upgrade Bullet Damage" << std::endl;
}

void Player::upgradeReloadInterval(float amount) {
	if (!remainUpgradeSkills)
		return;
	reloadInterval -= amount;
	remainUpgradeSkills--;
	std::cout << "Upgrade Reload Interval" << std::endl;
}

void Player::upgradeMovementSpeed(float amount) {
	if (!remainUpgradeSkills)
		return;
	setMaxSpeed(getMaxSpeed() + amount);
	remainUpgradeSkills--;
	std::cout << "Upgrade Movement Speed" << std::endl;
}

ObjectType Player::WhatAmI() {
	return ObjectType::Player;
}

void Player::move(float x, float y) {
	Object::move(x, y);

	sf::Vector2f pos = body.getPosition();
	for (auto& turret : turrets)
		turret.setPosition(pos.x, pos.y);
	healthBar.setPosition(pos.x, pos.y);
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for (auto& turret : turrets)
		turret.draw(target, states);
	Object::draw(target, states);
	target.draw(healthBar, states);
}
