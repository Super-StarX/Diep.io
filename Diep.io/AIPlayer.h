#pragma once

#include "Player.h"

// ˼����Χ
class AIPlayer : public Player {
public:
	AIPlayer(float radius, const sf::Color& color, const point& position, int maxHealth);

	void checkCollision();
	float fastDistance(const point& a, const point& b);
	float evaluateSafety(const point& position); // ����λ�õİ�ȫ�Ե÷�
	point computeNewPosition(float x, float y); // �����ײ
	void think();

private:
	virtual bool isAI();
	virtual ObjectType WhatAmI();
};
