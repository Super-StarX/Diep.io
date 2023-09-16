
#include "Bullet.h"
#include "AIPlayer.h"

void Bullet::checkCollision() {
	// 检查与其他子弹的碰撞
	for (auto& bullet : bullets) {
		if (&bullet != this) {
			if (!sameOwner(&bullet) && collideWith(bullet)) {
				int damage = bullet.getDamage();
				bullet.reduceHealth(getDamage());
				reduceHealth(damage);
			}
		}
	}

	// 检查与玩家的碰撞 现在ismyowner还有问题,不能正常判断
	if (!isMyOwner(&player) && collideWith(player)) {
		int health = player.getHealth();
		player.reduceHealth(getDamage());
		reduceHealth(health);
	}

	// 检查与资源的碰撞
	for (auto& resource : resources) {
		if (collideWith(resource)) {
			int health = resource.getHealth();
			resource.reduceHealth(getDamage());
			reduceHealth(health);

			// 为资源添加后坐力
			sf::Vector2f impactDirection = getVelocity();
			sf::Vector2f newVelocity = resource.getVelocity() + 0.2f * impactDirection;
			resource.setVelocity(newVelocity);
		}
	}

	// 检查与敌人的碰撞
	for (auto& enemy : enemies) {
		if (!isMyOwner(&enemy) && collideWith(enemy)) {
			int health = enemy.getHealth();
			enemy.reduceHealth(getDamage());
			reduceHealth(health);
		}
	}
}
