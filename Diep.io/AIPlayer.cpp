#include "AIPlayer.h"
#include "Bullet.h"

AIPlayer::AIPlayer(float radius, const sf::Color& color, const sf::Vector2f& position, int maxHealth) :
    Player(radius, color, position, maxHealth) {
}

bool AIPlayer::isAI() {
    return true;
}

void AIPlayer::checkCollision() {
    Player::checkCollision();

    for (auto& enemy : enemies) {
        if (&enemy != this && collideWith(enemy)) {
            int health = getHealth();
            reduceHealth(enemy.getHealth());
            enemy.reduceHealth(health);
        }
    }
}

float AIPlayer::fastDistance(const sf::Vector2f& a, const sf::Vector2f& b) {
    return std::hypot(a.x - b.x, a.y - b.y);
}

float AIPlayer::evaluateSafety(const sf::Vector2f& position) {
    // 初始化得分
    float score = 0.0f;

    // 检查是否靠近子弹，并根据距离降低得分
    for (Bullet& bullet : bullets) {
        float distance = fastDistance(position, bullet.evaluatePosition());
        if (distance < thinkRange) {
            score -= (thinkRange - distance);
        }
    }

    // 定义最小安全距离
    constexpr float min_safe_distance = 80.0f;

    // 检查是否靠近玩家，并根据距离降低得分
    {
        float distance = fastDistance(position, player.getPosition());
        if (distance < min_safe_distance) {
            score -= (min_safe_distance - distance) * 10;
        }
    }

    // 检查是否靠近其他敌人，并根据距离降低得分
    for (AIPlayer& enemy : enemies) {
        if (&enemy != this) {
            float distance = fastDistance(position, enemy.getPosition());
            if (distance < min_safe_distance) {
                score -= (min_safe_distance - distance) * 15;
            }
        }
    }

    // 检查是否靠近资源对象，并根据距离降低得分
    for (Object& resource : resources) {
        float distance = fastDistance(position, resource.getPosition());
        if (distance < min_safe_distance) {
            score -= (min_safe_distance - distance) * 10;
        }
    }

    return score;
}

sf::Vector2f AIPlayer::computeNewPosition(float x, float y) {
    float move_dist = getMaxSpeed();

    if (x && y) { // 如果是斜向移动
        move_dist = move_dist / std::sqrt(2.0f);
    }

    return getPosition() + sf::Vector2f(x * move_dist, y * move_dist);
}

void AIPlayer::think() {
    // 计算与玩家的距离
    sf::Vector2f distanceVec = getPosition() - player.getPosition();
    float distance = std::sqrt(distanceVec.x * distanceVec.x + distanceVec.y * distanceVec.y);

    // 如果在思考范围内，瞄准玩家并开火
    if (distance < thinkRange) {
        calcTurretRotation(player.getPosition());
        fire(player.getPosition());
    }

    // 定义移动方向的列表
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

    // 寻找使评估分数最高的移动方向
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

ObjectType AIPlayer::WhatAmI() {
    return ObjectType::AIPlayer;
}