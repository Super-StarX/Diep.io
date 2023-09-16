
#include "Bullet.h"
#include "AIPlayer.h"

void Bullet::checkCollision() {
	// ����������ӵ�����ײ
	for (auto& bullet : bullets) {
		if (&bullet != this) {
			if (!sameOwner(&bullet) && collideWith(bullet)) {
				int damage = bullet.getDamage();
				bullet.reduceHealth(getDamage());
				reduceHealth(damage);
			}
		}
	}

	// �������ҵ���ײ ����ismyowner��������,���������ж�
	if (!isMyOwner(&player) && collideWith(player)) {
		int health = player.getHealth();
		player.reduceHealth(getDamage());
		reduceHealth(health);
	}

	// �������Դ����ײ
	for (auto& resource : resources) {
		if (collideWith(resource)) {
			int health = resource.getHealth();
			resource.reduceHealth(getDamage());
			reduceHealth(health);

			// Ϊ��Դ��Ӻ�����
			sf::Vector2f impactDirection = getVelocity();
			sf::Vector2f newVelocity = resource.getVelocity() + 0.2f * impactDirection;
			resource.setVelocity(newVelocity);
		}
	}

	// �������˵���ײ
	for (auto& enemy : enemies) {
		if (!isMyOwner(&enemy) && collideWith(enemy)) {
			int health = enemy.getHealth();
			enemy.reduceHealth(getDamage());
			reduceHealth(health);
		}
	}
}
