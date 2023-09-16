
#include "AIPlayer.h"
#include "Bullet.h"
float AIPlayer::evaluateSafety(const sf::Vector2f& position) {
	// 计算该位置得分,靠近障碍物,分数越低
	float score = 0.0f;

	for (Bullet& bullet : bullets) {
		float distance = fastDistance(position, bullet.evaluatePosition());
		if (distance < thinkRange) {
			score -= (thinkRange - distance); // 越靠近障碍物，分数越低
		}
	}

	constexpr float min_safe_distance = 80.0f;
	{
		float distance = fastDistance(position, player.getPosition());
		if (distance < min_safe_distance) {
			score -= (min_safe_distance - distance) * 10; // 越靠近障碍物，分数越低
		}
	}

	for (AIPlayer& enemy : enemies) {
		float distance = fastDistance(position, enemy.getPosition());
		if (distance < min_safe_distance) {
			score -= (min_safe_distance - distance) * 15; // 越靠近障碍物，分数越低
		}
	}

	for (Object& resource : resources) {
		float distance = fastDistance(position, resource.getPosition());
		if (distance < min_safe_distance) {
			score -= (min_safe_distance - distance) * 10; // 越靠近障碍物，分数越低
		}
	}

	return score;
}

void AIPlayer::think() {
	// 攻击玩家
	sf::Vector2f distanceVec = getPosition() - player.getPosition();
	float distance = std::sqrt(distanceVec.x * distanceVec.x + distanceVec.y * distanceVec.y);
	if (distance < thinkRange) {
		calcTurretRotation(player.getPosition());
		fire(player.getPosition(), sf::Color::Red);
	}

	// 规避弹道
	const std::vector<sf::Vector2f> directions = {
	{  0, -1}, // 上移
	{  0,  1}, // 下移
	{ -1,  0}, // 左移
	{  1,  0}, // 右移
	{ -1, -1}, // 左上移
	{  1, -1}, // 右上移
	{ -1,  1}, // 左下移
	{  1,  1}, // 右下移
	{  0,  0}  // 原地不动
	};

	float best_score = -1e9;
	sf::Vector2f best_direction;

	for (const auto& dir : directions) {
		sf::Vector2f new_position = computeNewPosition(dir.x, dir.y);
		float score = evaluateSafety(new_position);

		if (score > best_score) {
			best_score = score;
			best_direction = dir;
		}
	}

	// 实际移动
	Acceleration = best_direction * 400.0f;
}
