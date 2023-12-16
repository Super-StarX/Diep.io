#include "Bullet.h"
#include "Turret.h"
#include "Player.h"
#include "Random.h"
#include "Global.h"

Turret::Turret(Player* owner, float sizeX, float sizeY, point offest, sf::Color color, float offestDir, float scale) :
	offestDir(offestDir), offest(offest) {
	shape.setSize(point(sizeX, sizeY));
	shape.setFillColor(sf::Color{ 158,158,158 });
	setOrigin(shape, offest);
	shape.setPosition(owner->getPosition());
	shape.setRotation(offestDir);
	shape.setOutlineThickness(3);
	shape.setOutlineColor(sf::Color{ 116,116,116 });
}

point Turret::fire(Player* owner, point target) const {
	point orgin = offest;
	orgin.y += shape.getSize().y / 2;
	orgin.y *= -1;

	float rad = math::rad(owner->getTurretRotation() + offestDir);
	auto bullet = new Bullet(owner, shape.getPosition() + orgin.rotation(rad));

	// 计算子弹发射角度
	point firePos = owner->getPosition();
	point direction = target - firePos;

	float accuracy = owner->getAccuracy();
	rad = math::rad(offestDir + random::randomFloat(-accuracy, accuracy));
	auto dir = direction.rotation(rad);

	float length = dir.length();
	bullet->setVelocity(dir / length * bullet->getBulletSpeed());
	bullets.emplace_back(bullet);
	return dir / length;
}

void Turret::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(shape, states);
}