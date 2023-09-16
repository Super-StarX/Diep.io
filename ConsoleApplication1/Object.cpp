#include "AIPlayer.h"
#include "Random.h"
#include "Global.h"

void Object::randomAddToMap() {
	bool isValid; // ����Ч��
	do {
		isValid = true;
		float randomX = random::randomFloat(0, mapWidth);
		float randomY = random::randomFloat(0, mapHeight);
		setPosition(randomX, randomY);
		// ����Ƿ���������ص���������������Ұ뾶���²������˰뾶֮�͵ľ���
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
