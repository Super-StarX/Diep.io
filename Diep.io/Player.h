#pragma once

#include "Object.h"

class Player : public Object {
public:
	Player(float radius, const sf::Color& color, const sf::Vector2f& position, int maxHealth) :
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

	virtual bool isAI() {
		return false;
	}

	sf::Vector2f getTurretPosition() const {
		return turret.getPosition();
	}
	
	float getTurretRotation() {
		return turret.getRotation();
	}
	
	void setTurretRotation(float degress) {
		turret.setRotation(degress);
	}

	void calcTurretRotation(const sf::Vector2f& mousePos) {
		sf::Vector2f turretPos = turret.getPosition();
		sf::Vector2f direction = mousePos - turretPos;

		// 计算夹角弧度
		float radians = std::atan2(direction.y, direction.x);

		// 转换为度数并设置炮台旋转角度
		float degrees = radians * (180.0f / 3.14159265f) + 90.0f; // 注意 +90 表示朝着鼠标
		setTurretRotation(degrees);
	}

	bool checkShot() {
		// 检查是否满足射击条件
		return timeSinceLastShot > reloadInterval;
	}

	void resetShot() {
		timeSinceLastShot = 0.0;
	}

	void setPosition(float x, float y) {
		Object::setPosition(x, y);

		turret.setPosition(x, y);
		healthBar.setPosition(x, y);
	}

	void reduceHealth(int amount) {
		Object::reduceHealth(amount);

		// 重新计算血条
		float percentage = static_cast<float>(currentHealth) / maxHealth;
		healthBar.setSize(sf::Vector2f(healthBar.getSize().x * percentage, healthBar.getSize().y));

		// 检测是否游戏结束
		if (!currentHealth && &Global::currentPlayer == this) {
			Global::isGameOver = true;
		}
	}

	void checkMove(float moveSpeed) {
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

	void checkCollision() {
		// 检查与资源的碰撞
		for (auto& resource : resources) {
			if (collideWith(resource)) {
				int health = getHealth();
				reduceHealth(resource.getHealth());
				resource.reduceHealth(health);
			}
		}
	}

	bool fire(sf::Vector2f target, sf::Color color);

	void update() {
		Object::update();
		//rof更新
		timeSinceLastShot += Global::deltaTime;
	}

	float bulletSpeedMulti{ 1.0f };
	float bulletPenetration{ 1.0f };
	int bulletDamage{ 10 };
	//int HealthRegen { 0 };
	//int BodyDamage;
	float reloadInterval{ 0.3f }; // 射击间隔，以秒为单位
	//float movementSpeed;MaxSpeed
protected:
	virtual void move(float x, float y) {
		Object::move(x, y);

		sf::Vector2f pos = body.getPosition();
		turret.setPosition(pos.x, pos.y);
		healthBar.setPosition(pos.x, pos.y);
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		Object::draw(target, states);

		// 画炮台和血条
		target.draw(turret, states);
		target.draw(healthBar, states);
	}

	sf::RectangleShape turret;
	sf::RectangleShape healthBar;
	sf::Text nameText;

	float timeSinceLastShot{ 0.0f }; // 自上次射击以来经过的时间
};