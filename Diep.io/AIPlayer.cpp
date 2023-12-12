#include "AIPlayer.h"
#include "Bullet.h"
#include "Global.h"
#include "Data.h"

AIPlayer::AIPlayer(float radius, const sf::Color& color, const point& position, int maxHealth) :
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

float AIPlayer::fastDistance(const point& a, const point& b) {
    return std::hypot(a.x - b.x, a.y - b.y);
}

float AIPlayer::evaluateSafety(const point& position) {
    // ��ʼ���÷�
    float score = 0.0f;

    // ����Ƿ񿿽��ӵ��������ݾ��뽵�͵÷�
    for (Bullet& bullet : bullets) {
        float distance = fastDistance(position, bullet.evaluatePosition());
        if (distance < AIThinkRange) {
            score -= (AIThinkRange - distance);
        }
    }

    // ������С��ȫ����
    constexpr float min_safe_distance = 80.0f;

    // ����Ƿ񿿽���ң������ݾ��뽵�͵÷�
    {
        float distance = fastDistance(position, player.getPosition());
        if (distance < min_safe_distance) {
            score -= (min_safe_distance - distance) * 10;
        }
    }

    // ����Ƿ񿿽��������ˣ������ݾ��뽵�͵÷�
    for (AIPlayer& enemy : enemies) {
        if (&enemy != this) {
            float distance = fastDistance(position, enemy.getPosition());
            if (distance < min_safe_distance) {
                score -= (min_safe_distance - distance) * 15;
            }
        }
    }

    // ����Ƿ񿿽���Դ���󣬲����ݾ��뽵�͵÷�
    for (Object& resource : resources) {
        float distance = fastDistance(position, resource.getPosition());
        if (distance < min_safe_distance) {
            score -= (min_safe_distance - distance) * 10;
        }
    }

    return score;
}

point AIPlayer::computeNewPosition(float x, float y) {
    float move_dist = getMaxSpeed();

    if (x && y) // �����б���ƶ�
        move_dist /= std::sqrt(2.0f);

    return getPosition() + point(x * move_dist, y * move_dist);
}

void AIPlayer::think() {
    // ��������ҵľ���
    point distanceVec = getPosition() - player.getPosition();
    float distance = std::sqrt(distanceVec.x * distanceVec.x + distanceVec.y * distanceVec.y);

    // �����˼����Χ�ڣ���׼��Ҳ�����
    if (distance < AIThinkRange) {
        calcTurretRotation(player.getPosition());
        fire(player.getPosition());
    }

    // �����ƶ�������б�
    const std::vector<point> directions = {
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
    point best_direction;

    // Ѱ��ʹ����������ߵ��ƶ�����
    for (const auto& dir : directions) {
        point new_position = computeNewPosition(dir.x, dir.y);
        float score = evaluateSafety(new_position);

        if (score > best_score) {
            best_score = score;
            best_direction = dir;
        }
    }

    // ʵ���ƶ�
    setAcceleration(best_direction * 400.0f);
}

ObjectType AIPlayer::WhatAmI() {
    return ObjectType::AIPlayer;
}