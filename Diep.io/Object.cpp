#include "Object.h"
#include "AIPlayer.h"
#include "Random.h"
#include "Global.h"
#include "Data.h"

static int curObjectId = 0;

ResourceType Object::randomResourceType() {
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
	helper::add(objects, this);
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
	healthText.setString(std::to_string(maxHealth));
	ID = curObjectId++;
}

Object::Object(ResourceType type, const point& position) {
	helper::add(objects, this);
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
	healthText.setString(std::to_string(maxHealth));
	ID = curObjectId++;

	isResource = true;
	++Global::resourceCount;
}

Object::Object(const sf::Color& color, const point& position, int maxHealth) :maxHealth(maxHealth), currentHealth(maxHealth) {
	helper::add(objects, this);
	isPolygon = true;
	polygonRadius = 10.5f;
	polygonBody.setPointCount(3);
	polygonBody.setPoint(0, point(10, -6));
	polygonBody.setPoint(1, point(-10, -6));
	polygonBody.setPoint(2, point(0, 12));
	polygonBody.setFillColor(color);
	polygonBody.setOutlineColor(sf::Color(85, 101, 109));
	polygonBody.setPosition(position);
	polygonBody.setOutlineThickness(3);
	setOrigin(polygonBody);

	healthText.setFont(Global::font);
	healthText.setCharacterSize(20);
	healthText.setFillColor(sf::Color::White);
	setOrigin(healthText);
	healthText.setPosition(polygonBody.getPosition());
	healthText.setString(std::to_string(maxHealth));
	ID = curObjectId++;
}

Object::~Object() {
	//不需要在这里删除,update结束会统一删
	//helper::erase(objects, this);
	if(isResource)
		--Global::resourceCount;
}

void Object::setPosition(float x, float y) {
	body.setPosition(x, y);
	polygonBody.setPosition(x, y);
	healthText.setPosition(x, y);
}

void Object::setRotation(float degress) {
	polygonBody.setRotation(degress);
}

bool Object::isCollideWith(const Object* other, float extraDistance) const {
	float distance = getPosition().distance(other->getPosition());
	return distance < getRadius() + other->getRadius() + extraDistance;
}

void Object::randomAddToMap() {
	bool isValid; // 是有效的
	do {
		isValid = true;
		float randomX = random::randomFloat(0, mapWidth);
		float randomY = random::randomFloat(0, mapHeight);
		setPosition(randomX, randomY);
		for (auto other : objects) {
			if (other != this) {
				float minDistance = other->WhatAmI() == ObjectType::Object ? 0
					: (other->getRadius() + getRadius()) * 2.f;
				if (isCollideWith(other, minDistance)) {
					isValid = false;
					break;
				}
			}
		}
	} while (!isValid);
}

void Object::reduceHealth(int amount) {
	currentHealth = std::max(0, currentHealth - amount);
	std::ostringstream ss;
	ss << currentHealth << "/" << maxHealth;
	healthText.setString(ss.str());
}

void Object::updateMove() {
	//速度更新
	float delta = Global::deltaTime;
	Velocity += Acceleration * delta;
	float curInertia = pow(inertia, delta); // 惯性^delta
	if (!Acceleration.x)
		Velocity.x *= curInertia;

	if (!Acceleration.y)
		Velocity.y *= curInertia;

	// 将炮台速度限制在最大速度范围内
	float currentSpeed = getSpeed();
	if (currentSpeed > MaxSpeed)
		Velocity = (Velocity / currentSpeed) * MaxSpeed;

	checkFirendlyCollide(Velocity);

	// 防止速度过小时产生抖动
	if (abs(Velocity.x) < 0.1)
		Velocity.x = 0.0;
	if (abs(Velocity.y) < 0.1)
		Velocity.y = 0.0;

	// 移动炮台
	move(Velocity.x * delta, Velocity.y * delta);
}
void Object::update() {
	if (!isDied &&!currentHealth) {
		sf::Color color = body.getFillColor();
		auto alpha = std::max(0, static_cast<int>(color.a - 1024 * Global::deltaTime));
		color.a = alpha;
		body.setFillColor(color);
		color = body.getOutlineColor();
		color.a = alpha;
		body.setOutlineColor(color);
		color = polygonBody.getFillColor();
		color.a = alpha;
		polygonBody.setFillColor(color);
		color = polygonBody.getOutlineColor();
		color.a = alpha;
		polygonBody.setOutlineColor(color);
		color = healthText.getFillColor();
		color.a = alpha;
		healthText.setFillColor(color);
		if (alpha <= 0)
			isDied = true;
	}
	updateMove();
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