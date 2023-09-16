#pragma once

#include "Player.h"

class AIPlayer : public Player {
public:
	AIPlayer(float radius, const sf::Color& color, const sf::Vector2f& position, int maxHealth) :
		Player(radius, color, position, maxHealth) {

	}

	virtual bool isAI() {
		return true;
	}

	void checkCollision() {
		Player::checkCollision();
		// 检查与敌人的碰撞
		for (auto& enemy : enemies) {
			if (&enemy != this && collideWith(enemy)) {
				int health = getHealth();
				reduceHealth(enemy.getHealth());
				enemy.reduceHealth(health);
			}
		}
	}

	float fastDistance(const sf::Vector2f& a, const sf::Vector2f& b) {
		return std::hypot(a.x - b.x, a.y - b.y);
	}

	float evaluateSafety(const sf::Vector2f& position);

	sf::Vector2f computeNewPosition(float x, float y) {
		float move_dist = getMaxSpeed();

		if (x && y) { // 如果是斜向移动
			move_dist = move_dist / std::sqrt(2.0f);
		}

		return getPosition() + sf::Vector2f(x * move_dist, y * move_dist);
	}

	void think();
public:
	float thinkRange = 350.f;
};