#include "Object.h"
#include "AIPlayer.h"
#include "Random.h"
#include "Global.h"

static int curObjectId = 0;

template <class T>
void setOrigin(T shape) {
	sf::FloatRect Bounds = shape.getLocalBounds();
	shape.setOrigin(Bounds.width / 2, Bounds.height / 2);
}

Object::Object(float radius, const sf::Color& color, const sf::Vector2f& position, int maxHealth)
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

Object::~Object() {
}

sf::FloatRect Object::getGlobalBounds() {
	return body.getGlobalBounds();
}

void Object::setID(int id) {
	ID = id;
}

const int Object::getID() const {
	return ID;
}

void Object::setVelocity(const sf::Vector2f& vel) {
	Velocity = vel;
}

const sf::Vector2f& Object::getVelocity() const {
	return Velocity;
}

void Object::setAcceleration(const sf::Vector2f& vel) {
	Acceleration = vel;
}

const sf::Vector2f& Object::getAcceleration() const {
	return Acceleration;
}

float Object::getSpeed() {
	return std::sqrt(Velocity.x * Velocity.x + Velocity.y * Velocity.y);
}

float Object::getMaxSpeed() {
	return MaxSpeed;
}

void Object::setMaxSpeed(float spd) {
	MaxSpeed = spd;
}

float Object::getRadius() {
	return body.getRadius();
}

sf::Vector2f Object::getPosition() {
	return body.getPosition();
}

void Object::setPosition(float x, float y) {
	body.setPosition(x, y);
	healthText.setPosition(x, y);
}

float Object::getTurretRotation() {
	return 0;
}

bool Object::collideWith(Object& other, float extraDistance) {
	sf::Vector2f distanceVec = getPosition() - other.getPosition();
	float distance = std::sqrt(distanceVec.x * distanceVec.x + distanceVec.y * distanceVec.y);
	float minDistance = getRadius() + other.getRadius() + extraDistance;
	return distance < minDistance;
}

bool Object::collideWith(const Object& other) const {
	double distance = std::sqrt(std::pow(body.getPosition().x - other.body.getPosition().x, 2) +
		std::pow(body.getPosition().y - other.body.getPosition().y, 2));
	return distance <= body.getRadius() + other.body.getRadius();
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

int Object::getHealth() const {
	return currentHealth;
}

void Object::setHealth(int health) {
	currentHealth = health;
}

void Object::update() {
	float delta = Global::deltaTime;

	//速度更新
	Velocity += Acceleration * delta;
	float inertia = pow(Global::inertia, delta); // 惯性^delta
	if (!Acceleration.x)
		Velocity.x *= inertia;

	if (!Acceleration.y)
		Velocity.y *= inertia;

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
	sf::Vector2f curPos = body.getPosition();
	sf::FloatRect playerBounds = body.getGlobalBounds();

	if (curPos.x + x < 0 ||
		curPos.x + x + playerBounds.width > Global::mapWidth)
		x = 0;
	if (curPos.y + y < 0 ||
		curPos.y + y + playerBounds.height > Global::mapHeight)
		y = 0;

	body.move(x, y);
	healthText.move(x, y);
}

void Object::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(body, states);
	target.draw(healthText, states);
}