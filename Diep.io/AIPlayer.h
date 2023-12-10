#pragma once

#include "Player.h"

// ˼����Χ
static float thinkRange = 350.f;
class AIPlayer : public Player {
public:
	AIPlayer(float radius, const sf::Color& color, const sf::Vector2f& position, int maxHealth);

	virtual bool isAI();
	void checkCollision();
	float fastDistance(const sf::Vector2f& a, const sf::Vector2f& b);
	float evaluateSafety(const sf::Vector2f& position); // ����λ�õİ�ȫ�Ե÷�
	sf::Vector2f computeNewPosition(float x, float y); // �����ײ
	void think();
	virtual ObjectType WhatAmI();
};
