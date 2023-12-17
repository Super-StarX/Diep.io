#pragma once
#include <SFML/System/Vector2.hpp>

// map
static constexpr int defaultWindowWidth = 800;
static constexpr int defaultWindowHeight = 600;
static constexpr int mapWidth = 5000;
static constexpr int mapHeight = 5000;
static constexpr int cellSize = 15;
static constexpr int maxResourceCount = 500; // 最大资源数
static constexpr float respawnTime = 0.1f; // 控制新资源生成间隔（秒）

// tank
static constexpr float moveSpeed = 300.0f;
static constexpr float inertia = 0.1f; // 摩擦力
static constexpr int bulletLife = 300;
static sf::Color blueTeamColor{ 0,179,225 };
static sf::Color redTeamColor{ 241,78,84 };

static std::vector<int> nextLevelExp = { 4,9,15,22,28,35,44,54,64,75,87,101,113,132,151,171,192,215,241,269,299,333,368,407,450,496,546,600,659,723,791,839,889,942,999,1059,1123,1190,1261,1337,1417,1502,1593,1687 };

static constexpr float AIThinkRange = 350.f;
static constexpr float min_safe_distance = 80.0f;

class point : public sf::Vector2f {
public:
	inline float length() const {
		return std::sqrtf(x * x + y * y);
	}

	inline float distance(point pos) const {
		float X = pos.x - x;
		float Y = pos.y - y;
		return std::sqrtf(X * X + Y * Y);
	}

	inline point rotation(float rad) const {
		return point{ x * cos(rad) - y * sin(rad) ,x * sin(rad) + y * cos(rad) };
	}
	
	inline point normalization() const {
		float l = length();
		return point{ x / l ,y / l };
	}

	inline float dot(const point& rhs) const {
		return x * rhs.x + y * rhs.y;
	}

	inline bool inMap() const {
		return x > 0 && x < mapWidth && y>0 && y < mapHeight;
	}
	point() : sf::Vector2f() {}
	point(float x, float y) : sf::Vector2f(x, y) {}
	point(const sf::Vector2f& v) : sf::Vector2f(v) {}
};

namespace math
{
	static constexpr float PI = 3.14159265f;
	inline static float rad(float angle) { return angle * PI / 180.f; }
	inline static float angle(float rad) { return rad * 180.0f / PI; }
	// 完全非弹性碰撞
	inline static point combined(const point& v1, float m1, const point& v2, float m2) {
		return point{ v1.x * m1 + v2.x * m2, v1.y * m1 + v2.y * m2 } / (m1 + m2);
	}
	// 完全弹性碰撞
	static std::pair<point, point> momentum(const point& v1, float m1, const point& v2, float m2) {
		point u1 = { (v1.x * (m1 - m2) + 2 * m2 * v2.x) / (m1 + m2), (v1.y * (m1 - m2) + 2 * m2 * v2.y) / (m1 + m2) };
		point u2 = { (v2.x * (m2 - m1) + 2 * m1 * v1.x) / (m1 + m2), (v2.y * (m2 - m1) + 2 * m1 * v1.y) / (m1 + m2) };
		return std::make_pair(u1, u2);
	}
	// 考虑碰撞点的完全弹性碰撞
	static std::pair<point, point> collision(const point v1, const point v2, const point& pos1, const point& pos2, float m1, float m2) {
		point normal = pos2 - pos1;
		point unit_normal = normal * (1.0f / sqrtf(normal.dot(normal)));
		point unit_tangent{ -unit_normal.y, unit_normal.x };

		float v1n = unit_normal.dot(v1);
		float v1t = unit_tangent.dot(v1);
		float v2n = unit_normal.dot(v2);
		float v2t = unit_tangent.dot(v2);

		// Velocity components in normal direction after collision, tangent components remain same
		float v1n_final = (v1n * (m1 - m2) + 2 * m2 * v2n) / (m1 + m2);
		float v2n_final = (v2n * (m2 - m1) + 2 * m1 * v1n) / (m1 + m2);

		// Convert scalar normal and tangential speeds into vectors
		point v1n_final_vec = unit_normal * v1n_final;
		point v1t_final_vec = unit_tangent * v1t;
		point v2n_final_vec = unit_normal * v2n_final;
		point v2t_final_vec = unit_tangent * v2t;

		// Update velocities with final velocities after collision
		return std::make_pair(v1n_final_vec + v1t_final_vec, v2n_final_vec + v2t_final_vec);
	}
}

namespace helper
{
	template<typename T>
	inline static void erase(std::vector<T>& vec, T my) {
		vec.erase(std::remove(vec.begin(), vec.end(), my), vec.end());
	}
	template<typename T>
	inline static void add(std::vector<T>& vec, T my) {
		vec.push_back(my);
	}
}

template <class T>
inline static void setOrigin(T& shape, point offest = {0,0}) {
	sf::FloatRect Bounds = shape.getLocalBounds();
	shape.setOrigin(Bounds.left + Bounds.width / 2 + offest.x, Bounds.top + Bounds.height / 2 + offest.y);
}

#include <cstdarg>
static void initConvexShape(sf::ConvexShape& polygonBody,int count, point points...) {
	va_list args;
	va_start(args, points);
	//int count = sizeof(points) / 8;
	polygonBody.setPointCount(count);
	for (int i = 0; i < count; ++i)
		polygonBody.setPoint(i, va_arg(args, point));
	va_end(args);
}

static void initConvexShape2(sf::ConvexShape& polygonBody,std::vector<point>vec) {
	polygonBody.setPointCount(vec.size());
	for (int i = 0; i < vec.size(); ++i)
		polygonBody.setPoint(i, vec[i]);
}

template <typename... Args>
static void initConvexShape(sf::ConvexShape& polygonBody, Args... points) {
	static_assert(sizeof...(points) % 2 == 0, "The number of points should be even!");
	std::vector<point> vec = { points... };
	size_t count = vec.size();
	polygonBody.setPointCount(count);
	for (size_t i = 0; i < count; ++i) {
		polygonBody.setPoint(i, vec[i]);
	}
}

enum class ObjectType {
	Object,
	Player,
	AIPlayer,
	Bullet,
};

enum class ResourceType
{
	Square,
	Triangle,
	Pentagon,
	AlphaPentagon,
};

enum class TurretTypes
{
	//---------0-5级
	Default,

	//---------5-15级
	Twin,//左右双发
	Sniper,//一发2倍伤害但是rof/2
	MachineGun,//开枪更快
	FlankGuard,//前后开火

	//---------15-40级
	//Double to
	TripleShot,//散发3发
	GuadTank,//4个方向

	//Sniper to
	Assassin,//一发4倍伤害但是rof/2
	Overseer,//母舰

	//MachineGun to
	Destoryer,//一发巨大的炮弹,这炮弹能顶更多发子弹
	Gunner,//小机枪

	//FlankGuard to
	TriAngle,//屁股方向2发
	Smasher,//创人

	//终极
	Triplet,//一次三发
	OctoTank,//8个方向开火

	Stalker,//一次6倍伤害加超级穿透,速度很快
	Overlord,//

	Annihilator,//
	Streamliner,//射速超快

	Booster,//移速超快
	Landmine,//静止隐形
	Spike//反弹子弹
};