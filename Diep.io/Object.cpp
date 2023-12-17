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
	setOrigin(body);
	body.setPosition(position);
	body.setFillColor(color);
	body.setOutlineThickness(3);
	auto lineColor = color;
	lineColor.r = static_cast<int>(lineColor.r * 0.75);
	lineColor.g = static_cast<int>(lineColor.g * 0.75);
	lineColor.b = static_cast<int>(lineColor.b * 0.75);
	body.setOutlineColor(lineColor);

	healthText.setFont(Global::font);
	healthText.setCharacterSize(static_cast<int>(radius) );
	healthText.setFillColor(sf::Color::White);
	setOrigin(healthText);
	healthText.setPosition(body.getPosition());
	healthText.setString(std::to_string(maxHealth));
	ID = curObjectId++;
}

//资源
Object::Object(ResourceType type, const point& position) {
	helper::add(objects, this);
	isPolygon = true;
	switch (type)
	{
	case ResourceType::Square:
		initConvexShape2(polygonBody, { point(10, 10),point(-10, 10),point(-10, -10),point(10, -10) });
		polygonBody.setFillColor(sf::Color(255, 232, 105));
		polygonBody.setOutlineColor(sf::Color(191, 174, 78));
		polygonRadius = 12;
		maxHealth = 10;
		exp = 10;
		break;
	case ResourceType::Triangle:
		initConvexShape2(polygonBody, { point(15,9),point(-15, 9),point(0, -18) });
		polygonBody.setFillColor(sf::Color(252, 118, 119));
		polygonBody.setOutlineColor(sf::Color(183, 86, 87));
		polygonRadius = 13;
		maxHealth = 30;
		exp = 25;
		break;
	case ResourceType::Pentagon:
		initConvexShape2(polygonBody, { point(20, 8),point(0, 20),point(-20, 8),point(-12, -16),point(12, -16) });
		polygonBody.setFillColor(sf::Color(118, 148, 252));
		polygonBody.setOutlineColor(sf::Color(85, 101, 109));
		polygonRadius = 17;
		maxHealth = 100;
		exp = 130;
		break;
	case ResourceType::AlphaPentagon:
		initConvexShape2(polygonBody, { point(100, 40),point(0, 100),point(-100, 40),point(-60, -80),point(60, -80) });
		polygonBody.setFillColor(sf::Color(118, 148, 252));
		polygonBody.setOutlineColor(sf::Color(85, 101, 109));
		polygonRadius = 85;
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

//子机
Object::Object(const sf::Color& color, const point& position, int maxHealth, float maxSpeed)
	:maxHealth(maxHealth), currentHealth(maxHealth), MaxSpeed(maxSpeed){
	helper::add(objects, this);
	isPolygon = true;
	polygonRadius = 10.5f;
	initConvexShape2(polygonBody, { point(10, -6),point(-10, -6),point(0, 12) });
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

void Object::collision(Object* other) {
	point normal = getPosition() - other->getPosition();
	point unit_normal = normal * (1.0f / sqrtf(normal.dot(normal)));
	point unit_tangent{ -unit_normal.y, unit_normal.x };

	point v1 = getVelocity();
	point v2 = other->getVelocity();
	float v1n = unit_normal.dot(v1);
	float v1t = unit_tangent.dot(v1);
	float v2n = unit_normal.dot(v2);
	float v2t = unit_tangent.dot(v2);

	// Velocity components in normal direction after collision, tangent components remain same
	float m1 = getMass();
	float m2 = other->getMass();
	float v1n_final = (v1n * (m1 - m2) + 2 * m2 * v2n) / (m1 + m2);
	float v2n_final = (v2n * (m2 - m1) + 2 * m1 * v1n) / (m1 + m2);

	// Convert scalar normal and tangential speeds into vectors
	point v1n_final_vec = unit_normal * v1n_final;
	point v1t_final_vec = unit_tangent * v1t;
	point v2n_final_vec = unit_normal * v2n_final;
	point v2t_final_vec = unit_tangent * v2t;

	// Update velocities with final velocities after collision
	setVelocity(v1n_final_vec + v1t_final_vec);
	other->setVelocity(v2n_final_vec + v2t_final_vec);
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
				float minDistance = other->getRadius() + getRadius();
				if (other->WhatAmI() != ObjectType::Object)
					minDistance *= 2;
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

void Object::updateDying() {
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
	if (!isDied && !currentHealth)
		updateDying();
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
	setPosition(curPos.x + x, curPos.y + y);
}

void Object::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (isPolygon)
		target.draw(polygonBody, states);
	else
		target.draw(body, states);
	target.draw(healthText, states);
}