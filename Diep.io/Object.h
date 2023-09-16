#pragma once
#include <SFML/Graphics.hpp>
#include <sstream>

#include "Global.h"

template<class T>
void setOrigin(T shape) {
	sf::FloatRect Bounds = shape.getLocalBounds();
	shape.setOrigin(Bounds.width / 2, Bounds.height / 2);
}

static int curObjectId = 0;
class Object : public sf::Drawable {
public:

	Object(float radius, const sf::Color& color, const sf::Vector2f& position, int maxHealth) :
		body(radius), maxHealth(maxHealth), currentHealth(maxHealth){

		// 将炮塔原点设置为其局部边界的中心
		body.setOrigin(body.getRadius(), body.getRadius());
		body.setPosition(position);
		body.setFillColor(color);

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

	~Object() {
	}

	sf::FloatRect getGlobalBounds() {
		return body.getGlobalBounds();
	}

	void setID(int id) {
		ID = id;
	}

	const int getID() const {
		return ID;
	}
	
	void setVelocity(const sf::Vector2f& vel) {
		Velocity = vel;
	}

	const sf::Vector2f& getVelocity() const {
		return Velocity;
	}
	
	void setAcceleration(const sf::Vector2f& vel) {
		Acceleration = vel;
	}

	const sf::Vector2f& getAcceleration() const {
		return Acceleration;
	}

	float getSpeed() {
		return std::sqrt(Velocity.x * Velocity.x + Velocity.y * Velocity.y);
	}
	
	float getMaxSpeed() {
		return MaxSpeed;
	}
	
	void setMaxSpeed(float spd) {
		MaxSpeed = spd;
	}

	float getRadius() {
		return body.getRadius();
	}
	
	sf::Vector2f getPosition() {
		return body.getPosition();
	}

	void setPosition(float x, float y) {
		body.setPosition(x, y);
		healthText.setPosition(x, y);
	}

	bool collideWith(Object& other, float extraDistance) {
		sf::Vector2f distanceVec = getPosition() - other.getPosition();
		float distance = std::sqrt(distanceVec.x * distanceVec.x + distanceVec.y * distanceVec.y);
		float minDistance = getRadius() + other.getRadius() + extraDistance;

		return distance < minDistance;
	}

	bool collideWith(const Object& other) const {
		double distance = std::sqrt(std::pow(body.getPosition().x - other.body.getPosition().x, 2) +
			std::pow(body.getPosition().y - other.body.getPosition().y, 2));
		return distance <= body.getRadius() + other.body.getRadius();
	}

	void randomAddToMap();

	void reduceHealth(int amount) {
		currentHealth -= amount;
		std::ostringstream ss;
		ss << currentHealth << "/" << maxHealth;
		healthText.setString(ss.str());

		if (currentHealth < 0) {
			currentHealth = 0;
		}
	}

	int getHealth() const {
		return currentHealth;
	}
	
	void setHealth(int health) {
		currentHealth = health;
	}

	void update() {
		float delta = Global::deltaTime;

		//速度更新
		Velocity += Acceleration * delta;
		float inertia = pow(Global::inertia, delta); // 惯性^delta
		if (!Acceleration.x )
			Velocity.x *= inertia;
		
		if (!Acceleration.y )
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

	virtual bool isVivid() {
		return !getHealth();
	}

protected:
	virtual void move(float x, float y) {
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

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		target.draw(body, states);
		target.draw(healthText, states);
	}

	int ID;
	sf::CircleShape body;
	int maxHealth;
	int currentHealth;
	sf::Text healthText;
	sf::Vector2f Acceleration;
	sf::Vector2f Velocity;
	float MaxSpeed{ 150.0f };
};