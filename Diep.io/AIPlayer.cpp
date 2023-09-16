
#include "AIPlayer.h"
#include "Bullet.h"
float AIPlayer::evaluateSafety(const sf::Vector2f& position) {
	// �����λ�õ÷�,�����ϰ���,����Խ��
	float score = 0.0f;

	for (Bullet& bullet : bullets) {
		float distance = fastDistance(position, bullet.evaluatePosition());
		if (distance < thinkRange) {
			score -= (thinkRange - distance); // Խ�����ϰ������Խ��
		}
	}

	constexpr float min_safe_distance = 80.0f;
	{
		float distance = fastDistance(position, player.getPosition());
		if (distance < min_safe_distance) {
			score -= (min_safe_distance - distance) * 10; // Խ�����ϰ������Խ��
		}
	}

	for (AIPlayer& enemy : enemies) {
		float distance = fastDistance(position, enemy.getPosition());
		if (distance < min_safe_distance) {
			score -= (min_safe_distance - distance) * 15; // Խ�����ϰ������Խ��
		}
	}

	for (Object& resource : resources) {
		float distance = fastDistance(position, resource.getPosition());
		if (distance < min_safe_distance) {
			score -= (min_safe_distance - distance) * 10; // Խ�����ϰ������Խ��
		}
	}

	return score;
}

void AIPlayer::think() {
	// �������
	sf::Vector2f distanceVec = getPosition() - player.getPosition();
	float distance = std::sqrt(distanceVec.x * distanceVec.x + distanceVec.y * distanceVec.y);
	if (distance < thinkRange) {
		calcTurretRotation(player.getPosition());
		fire(player.getPosition(), sf::Color::Red);
	}

	// ��ܵ���
	const std::vector<sf::Vector2f> directions = {
	{  0, -1}, // ����
	{  0,  1}, // ����
	{ -1,  0}, // ����
	{  1,  0}, // ����
	{ -1, -1}, // ������
	{  1, -1}, // ������
	{ -1,  1}, // ������
	{  1,  1}, // ������
	{  0,  0}  // ԭ�ز���
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

	// ʵ���ƶ�
	Acceleration = best_direction * 400.0f;
}
