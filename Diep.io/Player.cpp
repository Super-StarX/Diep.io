#include <iostream>
#include "Player.h"
#include "Global.h"
#include "Data.h"
#include "Network.h"

Player::Player(float radius, const sf::Color& color, const point& position, int maxHealth, int level) :
	Object(radius, color, position, maxHealth), color(color) {
	changeTurret(TurretTypes::Overseer);
	setTeam(color == blueTeamColor ? 1 : 2);
	helper::add(players, this);
	healthBorder.setSize(point(radius * 2.2f + 2, 5));
	healthBorder.setFillColor(sf::Color{80,80,80});
	healthBorder.setPosition(getPosition());
	healthBar.setSize(point(radius * 2.2f, 3));
	healthBar.setFillColor(sf::Color::Red);
	healthBar.setPosition(getPosition());
	levelText.setFont(Global::font);
	levelText.setFillColor(sf::Color::White);
	levelText.setPosition(getPosition());
	setLevel(level);
}

Player::Player(Player* owner) :
	Object(owner->getColor(), owner->getPosition(), owner->getBulletDamage(), owner->getBulletSpeed())
	, owner(owner)
	, color(owner->getColor()) {
	helper::add(players, this);
	setTeam(color == blueTeamColor ? 1 : 2);
}

Player::~Player() {
	helper::erase(players, this);
	turrets.clear();
	for (auto spawn : spawns) {
		spawn->owner = nullptr;
		spawn->reduceHealth(spawn->getHealth());
	}
	if (owner)
		helper::erase(owner->spawns, this);
}

void Player::AddExp(int amount) {
	if (owner)
		return owner->AddExp(amount);
	if (level > 45 || level < 1)
		return;
	int need = nextLevelExp[level];
	int exp = getExp();
	exp += amount;
	setExp(exp);
	if (exp >= need && need) {
		int count = exp / need;
		++remainUpgradeSkills;
		setLevel(level + 1);
		exp -= need;
		setExp(exp);
		if (exp >= need)
			this->AddExp(amount - need);
	}
}

void Player::setLevel(int amount) {
	level = amount;
	/*坦克的大小變大、子彈的大小會增加。
	你的視野將會增加。
	你的移動速度將會降低。*/
	//1级15 45级约25
	float radius = 0.22f * amount + 14.78f;
	setRadius(radius);
	changeTurret(Type);
	setOrigin(body);
	setOrigin(polygonBody);
	setOrigin(healthBorder, point(0, radius * 1.4f));
	setOrigin(healthBar, point(0, radius * 1.4f));
	setOrigin(healthText, point(0, radius * 2.f));
	healthBar.setPosition(healthBar.getPosition().x - ((healthBorder.getSize().x - 2) - healthBar.getSize().x) / 2, healthBar.getPosition().y);
	levelText.setCharacterSize(static_cast<int>(radius));
	levelText.setString("Level:" + std::to_string(amount));
	setOrigin(levelText, point(0, -radius * 1.75f));
}

bool Player::upgradeCount() const {
	return remainUpgradeSkills > 0 || Global::debugMode;
}

void Player::changeTurret(TurretTypes type) {
	turrets.clear();
	float radius = getRadius();//15.f
	Type = type;
	switch (type)
	{
	case TurretTypes::Default:
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, point{ 0,20 }, color, 0, 1));
		break;
		//初级
	case TurretTypes::Twin:
		turrets.emplace_back(Turret(this, radius * 0.65f, radius * 2.2f, point{ -10,20 }, color, 0, 1));
		turrets.emplace_back(Turret(this, radius * 0.65f, radius * 2.2f, point{ 10,20 }, color, 0, 1));
		break;
	case TurretTypes::Sniper:
		turrets.emplace_back(Turret(this, radius * 0.65f, radius * 2.8f, point{ 0,40 }, color, 0, 1));
		break;
	case TurretTypes::MachineGun:
		turrets.emplace_back(Turret(this, radius * 0.95f, radius * 2.2f, point{ 0,20 }, color, 0, 1));
		break;
	case TurretTypes::FlankGuard:
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, point{ 0,20 }, color, 0, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 1.8f, point{ 0,20 }, color, 180, 1));
		break;
		//中级
	case TurretTypes::TripleShot:
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, point{ 0,20 }, color, -45, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, point{ 0,20 }, color, 0, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, point{ 0,20 }, color, 45, 1));
		break;
	case TurretTypes::GuadTank:
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, point{ 0,20 }, color, 0, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, point{ 0,20 }, color, 90, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, point{ 0,20 }, color, 180, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, point{ 0,20 }, color, 270, 1));
		break;
	case TurretTypes::Assassin:
		break;
	case TurretTypes::Overseer:
		turrets.emplace_back(Turret(this, radius * 1.35f, radius * 1.5f, point{ 0,20 }, color, 90, 1));
		turrets.emplace_back(Turret(this, radius * 1.35f, radius * 1.5f, point{ 0,20 }, color, 270, 1));
		bulletDamage = 50;
		break;
	case TurretTypes::Destoryer:
		break;
	case TurretTypes::Gunner:
		break;
	case TurretTypes::TriAngle:
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, point{ 0,20 }, color, 0, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 1.8f, point{ 0,20 }, color, -160, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 1.8f, point{ 0,20 }, color, 160, 1));
		break;
	case TurretTypes::Smasher:
		break;
		//高级
	case TurretTypes::Triplet:
		turrets.emplace_back(Turret(this, radius * 0.65f, radius * 1.8f, point{ -11,20 }, color, 0, 1));
		turrets.emplace_back(Turret(this, radius * 0.65f, radius * 1.8f, point{ 11,20 }, color, 0, 1));
		turrets.emplace_back(Turret(this, radius * 0.75f, radius * 2.2f, point{ 0,20 }, color, 0, 1));
		break;
	case TurretTypes::OctoTank:
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, point{ 0,20 }, color, 0, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, point{ 0,20 }, color, 45, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, point{ 0,20 }, color, 90, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, point{ 0,20 }, color, 135, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, point{ 0,20 }, color, 180, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, point{ 0,20 }, color, -45, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, point{ 0,20 }, color, -90, 1));
		turrets.emplace_back(Turret(this, radius * 0.8f, radius * 2.2f, point{ 0,20 }, color, -135, 1));
		break;
	case TurretTypes::Stalker:
		break;
	case TurretTypes::Overlord:
		break;
	case TurretTypes::Annihilator:
		break;
	case TurretTypes::Streamliner:
		break;
	case TurretTypes::Booster:
		break;
	case TurretTypes::Landmine:
		break;
	case TurretTypes::Spike:
		break;
	default:
		break;
	}
	for (auto& turret : turrets)
		turret.setRotation(turretRotating);
}

void Player::setTurretRotation(float degress) {
	turretRotating = degress;
	for (auto& turret : turrets)
		turret.setRotation(degress);
}

void Player::calcTurretRotation(const point& mousePos) {
	point turretPos = getPosition();
	point direction = mousePos - turretPos;

	// 计算夹角弧度
	float radians = std::atan2(direction.y, direction.x);

	// 转换为度数并设置炮台旋转角度
	float degrees = radians * (180.0f / 3.14159265f) + 90.0f; // 注意 +90 表示朝着鼠标
	setTurretRotation(degrees);
}

void Player::setPosition(float x, float y) {
	Object::setPosition(x, y);

	for (auto& turret : turrets)
		turret.setPosition(x, y);
	levelText.setPosition(x, y);
	healthBorder.setPosition(x, y);
	healthBar.setPosition(x - ((healthBorder.getSize().x - 2) - healthBar.getSize().x) / 2, y);
}

void Player::reduceHealth(int amount) {
	Object::reduceHealth(amount);

	// 重新计算血条
	int currentHealth = getHealth();
	float percentage = static_cast<float>(currentHealth) / getMaxHealth();
	healthBar.setSize(point((healthBorder.getSize().x - 2) * percentage, healthBar.getSize().y));
	healthBar.setPosition(healthBar.getPosition().x - ((healthBorder.getSize().x - 2) - healthBar.getSize().x) / 2, healthBar.getPosition().y);
}

// 根据用户输入(wsad)移动炮台
void Player::checkMove(float moveSpeed) {
	bool noXAcceleration = true;
	bool noYAcceleration = true;
	float MaxAcceleration = getMaxAcceleration();
	point Acceleration;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		Acceleration.x = -MaxAcceleration;
		noXAcceleration = false;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		Acceleration.x = MaxAcceleration;
		noXAcceleration = false;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		Acceleration.y = -MaxAcceleration;
		noYAcceleration = false;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		Acceleration.y = MaxAcceleration;
		noYAcceleration = false;
	}
	if (Acceleration.x && Acceleration.y)
		Acceleration = Acceleration.normalization() * MaxAcceleration;
	if (noXAcceleration)
		Acceleration.x = 0.0f;
	if (noYAcceleration)
		Acceleration.y = 0.0f;
	setAcceleration(Acceleration);
}

void Player::checkCollision() {
	// 检查与资源的碰撞
	for (auto resource : objects) {
		if(resource->getTeam()!=getTeam()&&resource!=this)
		if (isCollideWith(resource)) {
			int health = getHealth();
			reduceHealth(resource->getHealth());
			resource->reduceHealth(health);
			if (resource->getHealth() <= 0)
				AddExp(resource->getExp());
		}
	}
}

float Player::getBulletRadius(){
	int damage = this->getBulletDamage();
	if (damage > 15)
		return 6.f;
	else if (damage > 25)
		return 7.f;
	else if (damage > 40)
		return 8.f;
	else if (damage > 60)
		return 9.f;
	else if (damage > 100)
		return 10.f;
	return 5.f;
}

bool Player::fire(point target) {
	if (Type == TurretTypes::Overseer) {
		for (auto spawn : spawns) {
			point direction = target - spawn->getPosition();
			point acc = direction.normalization() * getMaxAcceleration();
			spawn->setAcceleration(acc);
		}
	}
	else if (fireFrame > rof) {
		for (auto& turret : turrets) {
			auto backVel = recoil * turret.fire(this, target);
			setVelocity(getVelocity() - backVel);
		}
		fireFrame = 0.f;
		return true;
	}
	return false;
}

void Player::checkFirendlyCollide(point& vel) {
	// 碰撞检测和处理
	if (owner && vel.x && vel.y) {
		Player* pClosest = nullptr;
		float minDis = std::numeric_limits<float>::max();

		for (auto& other : owner->spawns) {
			if (other != this) {
				float distance = this->getPosition().distance(other->getPosition());//getDistance(*this, other); // 获取两个炮台之间的距离
				if (distance < this->getRadius() * 2) { // 发生碰撞
					if (distance < minDis) {
						pClosest = other;
						minDis = distance;
					}
				}
			}
		}

		if (pClosest != nullptr) {
			float moveDistance = this->getRadius() * 2 - minDis;

			// 根据边缘位置方向调整速度，使得炮台停在碰撞球的边缘
			vel += (getPosition() - pClosest->getPosition()) * moveDistance;
		}
	}
}

void Player::updateDying() {
	Object::updateDying();
	for (auto& turret : turrets) {
		auto& body = turret.getShape();
		sf::Color color = body.getFillColor();
		auto alpha = std::max(0, static_cast<int>(color.a - 1024 * Global::deltaTime));
		color.a = alpha;
		body.setFillColor(color);
		color = body.getOutlineColor();
		color.a = alpha;
		body.setOutlineColor(color);
	}
}

void Player::updateSpawns() {
	// 产生新子机
	if (spawns.size() < 8 && fireFrame > rof) {
		auto spawn = new Player(this);
		spawns.push_back(spawn);
		fireFrame = 0.f;
	}
	// 
}

void Player::update() {
	if (owner) {
		auto& vel = getAcceleration();
		if (!vel.x && !vel.y) { // 回到母舰身边
			point direction = owner->getPosition() - getPosition();
			if (direction.length() > owner->getRadius()) {
				point acc = direction.normalization() * getMaxAcceleration();
				setAcceleration(acc);
			}
		}
	}
	Object::update();
	float delta= Global::deltaTime;

	if (owner) {
		auto& vel = getAcceleration();
		if (vel.x || vel.y) {
			float rad = atan2(vel.y, vel.x);
			setRotation(math::angle(rad) + 30);
		}
		setAcceleration(point(0, 0));
	}

	if (Type == TurretTypes::Overseer)
		updateSpawns();

	//rof更新
	fireFrame += delta;
	if (this == player)
		networkManager.sendPlayerPositionUpdate();
}

void Player::upgradeBulletSpeed(float amount) {
	bulletSpeed += amount;
	remainUpgradeSkills--;
	for (auto spawn : spawns)
		spawn->setMaxSpeed(bulletSpeed);
}

void Player::upgradeBulletPenetration(float amount) {
	bulletPenetration += amount;
	remainUpgradeSkills--;
}

void Player::upgradeBulletDamage(int amount) {
	bulletDamage += amount;
	remainUpgradeSkills--;
	for (auto spawn : spawns) {
		float percent = static_cast<float>(spawn->getHealth()) / getMaxHealth();
		spawn->setMaxHealth(bulletDamage);
		spawn->setHealth(bulletDamage * percent);
	}
}

void Player::upgradeReloadInterval(float amount) {
	rof -= amount;
	remainUpgradeSkills--;
}

void Player::upgradeMovementSpeed(float amount) {
	setMaxSpeed(getMaxSpeed() + amount);
	remainUpgradeSkills--;
}

void Player::move(float x, float y) {
	Object::move(x, y);
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for (auto& turret : turrets)
		turret.draw(target, states);
	for (auto spawn : spawns)
		spawn->draw(target, states);
	Object::draw(target, states);
	target.draw(healthBorder, states);
	target.draw(healthBar, states);
	target.draw(levelText, states);
}
