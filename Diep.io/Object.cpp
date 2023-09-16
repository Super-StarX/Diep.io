#include "AIPlayer.h"
#include "Random.h"
#include "Global.h"

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
