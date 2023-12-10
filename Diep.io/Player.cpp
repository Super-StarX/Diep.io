#include <iostream>
#include "Player.h"
#include "Bullet.h"
#include "Player.h"
#include "Random.h"

Player::Player(float radius, const sf::Color& color, const sf::Vector2f& position, int maxHealth) :
	Object(radius, color, position, maxHealth) {

	turret.setSize(sf::Vector2f(radius * 0.5f, radius * 1.8f));
	turret.setFillColor(color);
	turret.setOrigin(turret.getSize().x / 2, turret.getSize().y);
	turret.setPosition(body.getPosition());
	healthBar.setSize(sf::Vector2f(radius * 2.0f, 4.0f));
	healthBar.setFillColor(sf::Color::Red);
	healthBar.setOrigin(healthBar.getSize().x / 2, 20);
	healthBar.setPosition(body.getPosition().x, body.getPosition().y);
}

int Player::getExp() const {
	return exp;
}

void Player::AddExp(int amount) {
	exp += amount;
	if (exp >= 1000) {
		int count = exp / 1000;
		remainLevel += count;
		level += count;
		exp -= 1000 * count;
	}
}

int Player::getLevel() const {
	return level;
}

int Player::upgradeCount() const {
	return remainLevel > 0;
}

bool Player::isAI() {
	return false;
}

sf::Vector2f Player::getTurretPosition() const {
	return turret.getPosition();
}

float Player::getTurretRotation() {
	return turret.getRotation();
}

void Player::setTurretRotation(float degress) {
	turret.setRotation(degress);
}

void Player::calcTurretRotation(const sf::Vector2f& mousePos) {
	sf::Vector2f turretPos = turret.getPosition();
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

bool Player::fire(sf::Vector2f target, sf::Color color) {
	if (checkShot()) {
		Bullet bullet(this, color);
		sf::Vector2f direction = target - getTurretPosition();

		direction.x += random::randomFloat(-10, 10);
		direction.y += random::randomFloat(-10, 10);
		float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
		bullet.setVelocity(direction / length * bullet.getBulletSpeed());
		bullets.emplace_back(bullet);

		setVelocity(getVelocity() - 3.f * direction / length);
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
	bulletSpeedMulti += amount;
	remainLevel--;
	std::cout << "Upgrade Bullet Speed" << std::endl;
}

void Player::upgradeBulletPenetration(float amount) {
	bulletPenetration += amount;
	remainLevel--;
	std::cout << "Upgrade Bullet Penetration" << std::endl;
}

void Player::upgradeBulletDamage(int amount) {
	bulletDamage += amount;
	remainLevel--;
	std::cout << "Upgrade Bullet Damage" << std::endl;
}

void Player::upgradeReloadInterval(float amount) {
	reloadInterval -= amount;
	remainLevel--;
	std::cout << "Upgrade Reload Interval" << std::endl;
}

void Player::upgradeMovementSpeed(float amount) {
	setMaxSpeed(getMaxSpeed() + amount);
	remainLevel--;
	std::cout << "Upgrade Movement Speed" << std::endl;
}

ObjectType Player::WhatAmI() {
	return ObjectType::Player;
}

void Player::move(float x, float y) {
	Object::move(x, y);

	sf::Vector2f pos = body.getPosition();
	turret.setPosition(pos.x, pos.y);
	healthBar.setPosition(pos.x, pos.y);
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	Object::draw(target, states);

	// 画炮台和血条
	target.draw(turret, states);
	target.draw(healthBar, states);
}
