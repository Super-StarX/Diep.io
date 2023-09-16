
#include "Bullet.h"
#include "Player.h"
#include "Random.h"

bool Player::fire(sf::Vector2f target, sf::Color color)
{
	if (checkShot()) {
		Bullet bullet(this, color);
		sf::Vector2f direction = target - getTurretPosition(); // ȷ��������̨��λ�ö������������λ��

		direction.x += random::randomFloat(-10, 10);
		direction.y += random::randomFloat(-10, 10);
		float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
		bullet.setVelocity(direction / length * bullet.getBulletSpeed());
		bullets.emplace_back(bullet);


		// ���������, ���ÿ���ʱ��
		setVelocity(getVelocity() - 3.f * direction / length);
		resetShot();

		return true;
	}
	return false;
}