#include "Bullet.h"
#include "Turret.h"
#include "Player.h"
#include "Random.h"

Turret::Turret(Player* owner, float sizeX, float sizeY, sf::Vector2f offest, sf::Color color, float offestDir, float scale) :
	offestDir(offestDir), offest(offest) {
	shape.setSize(sf::Vector2f(sizeX, sizeY));
	shape.setFillColor(sf::Color{ 158,158,158 });
	shape.setOrigin(sizeX / 2 + offest.x, sizeY / 2 + offest.y);
	shape.setPosition(owner->getPosition());
	shape.setRotation(offestDir);
	shape.setOutlineThickness(owner->getRadius() / 5);
	shape.setOutlineColor(sf::Color{ 116,116,116 });
}

void Turret::setRotation(float dir) {
	shape.setRotation(dir + offestDir);
}

void Turret::setPosition(float x, float y) {
	shape.setPosition(x, y);
}

sf::Vector2f Turret::fire(Player* owner, sf::Vector2f target) const {
	auto orgin = offest;
	orgin.y += shape.getSize().y / 2;
	float rad = (owner->getTurretRotation() + offestDir) / (180.0f / 3.14159265f);
	float x = orgin.x * cos(rad) + orgin.y * sin(rad);
	float y = orgin.x * sin(rad) - orgin.y * cos(rad);

	Bullet bullet(owner, shape.getPosition() + sf::Vector2f{ x,y });
	sf::Vector2f firePos = owner->getPosition();
	sf::Vector2f direction = target - firePos;

	direction.x += random::randomFloat(-20, 20);
	direction.y += random::randomFloat(-20, 20);
	if (offestDir) {
		rad = offestDir * 3.14159265f / 180.f;
		x = direction.x * cos(rad) - direction.y * sin(rad);
		y = direction.x * sin(rad) + direction.y * cos(rad);
		direction = { x,y };
	}

	float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
	bullet.setVelocity(direction / length * bullet.getBulletSpeed());
	bullets.emplace_back(bullet);
	return 3.f * direction / length;
}

void Turret::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(shape, states);
}