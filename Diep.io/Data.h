#pragma once
#include <SFML/System/Vector2.hpp>

class point : public sf::Vector2f {
public:
	float length() const {
		return std::sqrt(x * x + y * y);
	}

	point rotation(float rad) const {
		return point { x * cos(rad) - y * sin(rad) ,x * sin(rad) + y * cos(rad) };
	}

	point() : sf::Vector2f() {}
	point(float x, float y) : sf::Vector2f(x, y) {}
	point(const sf::Vector2f& v) : sf::Vector2f(v) {}
};

enum class ResourceType
{
	Square,
	Triangle,
	Pentagon,
	AlphaPentagon,
};

static constexpr int mapWidth = 5000;
static constexpr int mapHeight = 5000;
static constexpr float moveSpeed = 300.0f;
static constexpr float inertia = 0.1f; // 摩擦力
static constexpr int maxResourceCount = 500; // 最大资源数
static constexpr float respawnTime = 0.1f; // 控制新资源生成间隔（秒）
static constexpr int defaultWindowWidth = 800;
static constexpr int defaultWindowHeight = 600;

static constexpr int nextLevelExp[] = { 4,9,15,22,28,35,44,54,64,75,87,101,113,132,151,171,192,215,241,269,299,333,368,407,450,496,546,600,659,723,791,839,889,942,999,1059,1123,1190,1261,1337,1417,1502,1593,1687 };

static constexpr int bulletLife = 300;
static constexpr float AIThinkRange = 350.f;

namespace math
{
	static constexpr float PI = 3.14159265f;
	static float rad(float angle) { return angle * PI / 180.f; }
	static float angle(float rad) { return rad * 180.0f / PI; }
}