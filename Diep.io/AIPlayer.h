#pragma once

#include "Player.h"

class AIPlayer : public Player {
public:
	AIPlayer(float radius, const sf::Color& color, const point& position, int maxHealth);
	~AIPlayer();
	void checkCollision();
	float fastDistance(const point& a, const point& b);
	float evaluateSafety(const point& position); // 评估位置的安全性得分
	point computeNewPosition(float x, float y); // 规避碰撞
	void think();

	virtual void update() override;
private:
	virtual bool isAI() const override { return true; }
	virtual ObjectType WhatAmI() const override { return ObjectType::AIPlayer; }
};
