#include "Object.h"
#include "AIPlayer.h"
#include "Random.h"
#include "Global.h"
#include "Data.h"

static int curObjectId = 0;

template <class T>
void setOrigin(T shape) {
	sf::FloatRect Bounds = shape.getLocalBounds();
	shape.setOrigin(Bounds.width / 2, Bounds.height / 2);
}

ResourceType randomResourceType() {
	float randomNum = random::randomFloat(0, 100);
	if (randomNum < 1)
		return ResourceType::AlphaPentagon;
	else if (randomNum < 5)
		return ResourceType::Pentagon;
	else if (randomNum < 20)
		return ResourceType::Triangle;
	return ResourceType::Square;
}

Object::Object(float radius, const sf::Color& color, const point& position, int maxHealth)
	: body(radius), maxHealth(maxHealth), currentHealth(maxHealth) {
	// 将炮塔原点设置为其局部边界的中心
	body.setOrigin(radius, radius);
	body.setPosition(position);
	body.setFillColor(color);
	body.setOutlineThickness(radius / 5);
	auto lineColor = color;
	lineColor.r = static_cast<int>(lineColor.r * 0.75);
	lineColor.g = static_cast<int>(lineColor.g * 0.75);
	lineColor.b = static_cast<int>(lineColor.b * 0.75);
	body.setOutlineColor(lineColor);

	healthText.setFont(Global::font);
	healthText.setCharacterSize(static_cast<int>(radius) * 2);
	healthText.setFillColor(sf::Color::White);
	setOrigin(healthText);
	healthText.setPosition(body.getPosition());
	std::ostringstream ss;
	ss << maxHealth;
	healthText.setString(ss.str());
	ID = curObjectId++;
}

Object::Object(ResourceType type, const point& position) {
	// 将炮塔原点设置为其局部边界的中心
	isPolygon = true;
	switch (type)
	{
	case ResourceType::Square:
		polygonBody.setPointCount(4);	
		polygonBody.setPoint(0, point(10, 10));
		polygonBody.setPoint(1, point(-10, 10));
		polygonBody.setPoint(2, point(-10, -10));
		polygonBody.setPoint(3, point(10, -10));
		polygonBody.setFillColor(sf::Color(255, 232, 105));
		polygonBody.setOutlineColor(sf::Color(191, 174, 78));
		polygonRadius = 12;
		maxHealth = 10;
		exp = 10;
		break;
	case ResourceType::Triangle:
		polygonBody.setPointCount(3);
		polygonBody.setPoint(0, point(15,9));
		polygonBody.setPoint(1, point(-15, 9));
		polygonBody.setPoint(2, point(0, -18));
		polygonBody.setFillColor(sf::Color(252, 118, 119));
		polygonBody.setOutlineColor(sf::Color(183, 86, 87));
		polygonRadius = 13;
		maxHealth = 30;
		exp = 25;
		break;
	case ResourceType::Pentagon:
		polygonBody.setPointCount(5);
		polygonBody.setPoint(0, point(20, 8)); 
		polygonBody.setPoint(1, point(0, 20));
		polygonBody.setPoint(2, point(-20, 8)); 
		polygonBody.setPoint(3, point(-12, -16));
		polygonBody.setPoint(4, point(12, -16));
		polygonBody.setFillColor(sf::Color(118, 148, 252));
		polygonBody.setOutlineColor(sf::Color(85, 101, 109));
		polygonRadius = 16;
		maxHealth = 100;
		exp = 130;
		break;
	case ResourceType::AlphaPentagon:
		polygonBody.setPointCount(5);
		polygonBody.setPoint(0, point(100, 40));
		polygonBody.setPoint(1, point(0, 100)); 
		polygonBody.setPoint(2, point(-100, 40));
		polygonBody.setPoint(3, point(-60, -80)); 
		polygonBody.setPoint(4, point(60, -80));
		polygonBody.setFillColor(sf::Color(118, 148, 252));
		polygonBody.setOutlineColor(sf::Color(85, 101, 109));
		polygonRadius = 80;
		maxHealth = 3000;
		exp = 3000;
		break;
	default:
		break;
	}
	polygonBody.setPosition(position);
	polygonBody.setOutlineThickness(3);
	setOrigin(polygonBody);
	polygonBody.setRotation(random::randomFloat(0, 314));

	currentHealth = maxHealth;
	healthText.setFont(Global::font);
	healthText.setCharacterSize(20);
	healthText.setFillColor(sf::Color::White);
	setOrigin(healthText);
	healthText.setPosition(polygonBody.getPosition());
	std::ostringstream ss;
	ss << maxHealth;
	healthText.setString(ss.str());
	ID = curObjectId++;
}

Object::~Object() {
}

void Object::setID(int id) {
	ID = id;
}

void Object::setVelocity(const point& vel) {
	Velocity = vel;
}

void Object::setAcceleration(const point& vel) {
	Acceleration = vel;
}

void Object::setPosition(float x, float y) {
	body.setPosition(x, y);
	polygonBody.setPosition(x, y);
	healthText.setPosition(x, y);
}

bool Object::collideWith(Object& other, float extraDistance) {
	point distanceVec = getPosition() - other.getPosition();
	float distance = std::sqrt(distanceVec.x * distanceVec.x + distanceVec.y * distanceVec.y);
	float minDistance = getRadius() + other.getRadius() + extraDistance;
	return distance < minDistance;
}

bool Object::collideWith(const Object& other) const {
	double distance = std::sqrt(std::pow(getPosition().x - other.getPosition().x, 2) +
		std::pow(getPosition().y - other.getPosition().y, 2));
	return distance <= getRadius() + other.getRadius();
}

void Object::randomAddToMap() {
	bool isValid; // 是有效的
	do {
		isValid = true;
		float randomX = random::randomFloat(0, mapWidth);
		float randomY = random::randomFloat(0, mapHeight);
		setPosition(randomX, randomY);
		// 检查是否与玩家有重叠，并保持至少玩家半径和新产生敌人半径之和的距离
		float minDistance = player.getRadius() + getRadius();
		if (collideWith(player, minDistance * 2.f)) {
			isValid = false;
		}

		for (auto& other : resources) {
			if (&other != this) {
				minDistance = other.getRadius() + getRadius();
				if (collideWith(other, minDistance * 2.f)) {
					isValid = false;
					break;
				}
			}
		}
		for (auto& other : enemies) {
			if (&other != this) {
				minDistance = other.getRadius() + getRadius();
				if (collideWith(other, minDistance * 2.f)) {
					isValid = false;
					break;
				}
			}
		}
	} while (!isValid);
}

void Object::reduceHealth(int amount) {
	currentHealth -= amount;
	std::ostringstream ss;
	ss << currentHealth << "/" << maxHealth;
	healthText.setString(ss.str());
	if (currentHealth < 0) {
		currentHealth = 0;
	}
}

void Object::setMaxSpeed(float spd) {
	MaxSpeed = spd;
}

void Object::setHealth(int health) {
	currentHealth = health;
}

void Object::update() {
	float delta = Global::deltaTime;

	//速度更新
	Velocity += Acceleration * delta;
	float curInertia = pow(inertia, delta); // 惯性^delta
	if (!Acceleration.x)
		Velocity.x *= curInertia;

	if (!Acceleration.y)
		Velocity.y *= curInertia;

	// 将炮台速度限制在最大速度范围内
	float currentSpeed = getSpeed();
	if (currentSpeed > MaxSpeed) {
		Velocity = (Velocity / currentSpeed) * MaxSpeed;
	}

	// 防止速度过小时产生抖动
	if (abs(Velocity.x) < 0.1) {
		Velocity.x = 0.0;
	}
	if (abs(Velocity.y) < 0.1) {
		Velocity.y = 0.0;
	}

	// 移动炮台
	move(Velocity.x * delta, Velocity.y * delta);
}

bool Object::isVivid() {
	return !getHealth();
}

ObjectType Object::WhatAmI() {
	return ObjectType::Object;
}

void Object::move(float x, float y) {
	point curPos = getPosition();
	sf::FloatRect playerBounds = getGlobalBounds();

	if (curPos.x + x < 0 ||
		curPos.x + x + playerBounds.width > mapWidth)
		x = 0;
	if (curPos.y + y < 0 ||
		curPos.y + y + playerBounds.height > mapHeight)
		y = 0;

	polygonBody.move(x, y);
	body.move(x, y);
	healthText.move(x, y);
}

void Object::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (isPolygon)
		target.draw(polygonBody, states);
	else
		target.draw(body, states);
	target.draw(healthText, states);
}