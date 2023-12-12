#pragma once

#include "Player.h"

// 思考范围
class AIPlayer : public Player {
public:
	AIPlayer(float radius, const sf::Color& color, const point& position, int maxHealth);

	void checkCollision();
	float fastDistance(const point& a, const point& b);
	float evaluateSafety(const point& position); // 评估位置的安全性得分
	point computeNewPosition(float x, float y); // 规避碰撞
	void think();

private:
	virtual bool isAI();
	virtual ObjectType WhatAmI();
};
