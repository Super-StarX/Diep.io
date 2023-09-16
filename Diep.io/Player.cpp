
#include "Bullet.h"
#include "Player.h"
#include "Random.h"

bool Player::fire(sf::Vector2f target, sf::Color color)
{
	if (checkShot()) {
		Bullet bullet(this, color);
		sf::Vector2f direction = target - getTurretPosition(); // 确保这是炮台的位置而不仅仅是玩家位置

		direction.x += random::randomFloat(-10, 10);
		direction.y += random::randomFloat(-10, 10);
		float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
		bullet.setVelocity(direction / length * bullet.getBulletSpeed());
		bullets.emplace_back(bullet);


		// 开火后坐力, 重置开火时间
		setVelocity(getVelocity() - 3.f * direction / length);
		resetShot();

		return true;
	}
	return false;
}