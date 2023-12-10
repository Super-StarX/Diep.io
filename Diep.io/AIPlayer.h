#pragma once

#include "Player.h"

// 思考范围
static float thinkRange = 350.f;
class AIPlayer : public Player {
public:
	AIPlayer(float radius, const sf::Color& color, const sf::Vector2f& position, int maxHealth);

	virtual bool isAI();
	void checkCollision();
	float fastDistance(const sf::Vector2f& a, const sf::Vector2f& b);
	float evaluateSafety(const sf::Vector2f& position); // 评估位置的安全性得分
	sf::Vector2f computeNewPosition(float x, float y); // 规避碰撞
	void think();
	virtual ObjectType WhatAmI();
};
