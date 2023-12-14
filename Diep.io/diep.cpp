#include <SFML/Graphics.hpp>

#include "Object.h"
#include "AIPlayer.h"
#include "Bullet.h"
#include "Global.h"
#include "Random.h"
#include "UI.h"
#include "Menu.h"
#include "Network.h"

// 会变的数值
bool Global::isGameOver = false;
float Global::deltaTime = 0.0f;
float timeSinceLastSpawn = 0.0f; // 保存上次生成资源的时间

// 创建一个视图并将其设置为窗口大小
sf::RenderWindow window(sf::VideoMode(defaultWindowWidth, defaultWindowHeight), "diep.io!");
sf::View view(sf::FloatRect(0.f, 0.f, static_cast<float>(defaultWindowWidth), static_cast<float>(defaultWindowHeight)));
sf::Font Global::font;
sf::Clock globalClock;
NetworkManager networkManager;
UI Global::ui;
Menu menu;
int Global::resourceCount;

// 创建对象
Player* player;
std::vector<Player*> players;
std::vector<Bullet*> bullets;
std::vector<Object*> objects;
std::vector<AIPlayer*> enemies;
std::vector<sf::VertexArray> Global::lines;

point getWindowToWorldPosition(const sf::View& view) {
	// 获取鼠标在窗口中的位置（窗口坐标系）
	sf::Vector2i windowMousePosition = sf::Mouse::getPosition(window);

	// 将窗口坐标转换为视图坐标系
	point viewMousePosition = window.mapPixelToCoords(windowMousePosition, view);

	return viewMousePosition;
}

void drawGrid(int cellSize) {
	constexpr int borderSize = 20;
	//边界
	sf::RectangleShape border(point(mapWidth + borderSize * 2, mapHeight + borderSize * 2));
	border.setFillColor(sf::Color(150, 150, 150));
	border.setPosition(-borderSize, -borderSize);
	window.draw(border);

	//地图
	sf::RectangleShape map(point(mapWidth, mapHeight));
	map.setFillColor(sf::Color(198, 198, 198));
	window.draw(map);

	//网格
	sf::RectangleShape line(point(mapWidth, 1));
	line.setFillColor(sf::Color(170, 170, 170));

	for (size_t y = 0; y <= mapHeight / cellSize; ++y) {
		line.setPosition(0, static_cast<float>(y * cellSize));
		window.draw(line);
	}

	line.setSize(point(1, mapHeight));

	for (size_t x = 0; x <= mapWidth / cellSize; ++x) {
		line.setPosition(static_cast<float>(x * cellSize), 0);
		window.draw(line);
	}
}

void update() {
	networkManager.update();
	//networkManager.sendGameData(/* 要发送的游戏数据 */);
	bool hasFocus = true;
	bool isClickButton = false;
	sf::Event event;

	Global::deltaTime = globalClock.restart().asSeconds();
	while (window.pollEvent(event)) {
		menu.updateMenu(event);

		if (event.type == sf::Event::Closed) {
			window.close();
		}
		else if (event.type == sf::Event::Resized) {
			// 获取新的窗口尺寸
			float windowWidth = static_cast<float>(event.size.width);
			float windowHeight = static_cast<float>(event.size.height);

			// 计算新的视图尺寸，选择更窄的边作为基础
			float viewWidth, viewHeight;
			if (windowWidth / mapWidth < windowHeight / mapHeight) {
				viewWidth = windowWidth;
				viewHeight = windowWidth * mapHeight / mapWidth;
			}
			else {
				viewWidth = windowHeight * mapWidth / mapHeight;
				viewHeight = windowHeight;
			}

			// 更新视图尺寸
			view.setSize(viewWidth, viewHeight);

			// 计算并设置新的视口，使内容居中
			sf::FloatRect viewport(
				(windowWidth - viewWidth) / 2 / windowWidth,
				(windowHeight - viewHeight) / 2 / windowHeight,
				viewWidth / windowWidth,
				viewHeight / windowHeight
			);
			view.setViewport(viewport);
		}

		if (!menu.inMenu()) {
			if (event.type == sf::Event::LostFocus)
				hasFocus = false;
			else if (event.type == sf::Event::GainedFocus)
				hasFocus = true;
			// 处理UI点击
			if (event.type == sf::Event::MouseButtonPressed
				&& event.mouseButton.button == sf::Mouse::Left
				&& player->upgradeCount()) {
				sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
				if (mousePosition.x <= 100 && mousePosition.y >= defaultWindowHeight - 370)
				{
					isClickButton = true;
					Global::ui.handleMouseClick(event.mouseButton.x, event.mouseButton.y);
				}
			}
		}
	}

	if (!menu.inMenu()) {
		// 如果游戏未结束并且用户聚焦于当前应用
		if (hasFocus && !isClickButton && !Global::isGameOver) {
			// 更新炮塔方向
			point mousePosition = getWindowToWorldPosition(view);
			player->calcTurretRotation(mousePosition);

			// 根据用户输入(wsad)移动炮台
			player->checkMove(moveSpeed);

			// 如果按下鼠标左键,则进行开火
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
				if (player->fire(mousePosition))
					networkManager.sendPlayerFireEvent(mousePosition);
		}

		for (const auto& obj : objects)
			obj->update();

		// 网格遍历碰撞检测
		std::map<std::pair<int, int>, std::vector<Object*>> grid;
		for (auto& obj : objects) {
			int x = static_cast<int>(obj->getPosition().x / 250);
			int y = static_cast<int>(obj->getPosition().y / 250);
			grid[std::make_pair(x, y)].push_back(obj);
		}

		for (auto& cell : grid) {
			auto& objs = cell.second;
			for (size_t i = 0; i < objs.size(); ++i) {
				for (size_t j = i + 1; j < objs.size(); ++j) {
					auto obj1 = objs[i], obj2 = objs[j];
					sf::VertexArray line(sf::Lines, 2);
					line[0].position = obj1->getPosition();
					line[1].position = obj2->getPosition();
					Global::lines.push_back(line);
					
					if (obj1->isCollideWith(obj2) && obj1 != obj2
						&& obj1->getHealth() && obj2->getHealth()) {
						if (obj1->WhatAmI() == ObjectType::Bullet && obj2->WhatAmI() == ObjectType::Bullet) { //子弹相撞
							while (obj1->getTeam() != obj2->getTeam() && obj1->getHealth() > 0 && obj2->getHealth() > 0) { // 直到有一方扣完血
								obj1->reduceHealth(static_cast<Bullet*>(obj2)->getMaxHealth());
								obj2->reduceHealth(static_cast<Bullet*>(obj1)->getMaxHealth());
							}
						}
						else if (obj1->WhatAmI() != ObjectType::Bullet && obj2->WhatAmI() != ObjectType::Bullet) { //物物相撞
							if (obj1->getTeam() != obj2->getTeam()) {
								obj1->reduceHealth(obj2->getBodyDamage());
								obj2->reduceHealth(obj1->getBodyDamage());
								if (!obj1->getHealth() || !obj2->getHealth()) {
									auto winner = static_cast<Player*>(obj1->getHealth() ? obj1 : obj2);
									winner->AddExp(obj2->getExp());
									continue;
								}
							}
							auto vel = math::momentum(obj1->getVelocity(), obj1->getMass(), obj2->getVelocity(), obj2->getMass());
							obj2->setVelocity(-vel.first);
							obj1->setVelocity(-vel.second);
						}
						else { // 弹物相撞
							auto bullet = obj1->WhatAmI() == ObjectType::Bullet ? obj1 : obj2;
							auto obj = obj1->WhatAmI() == ObjectType::Bullet ? obj2 : obj1;
							if (obj1->getTeam() != obj2->getTeam()) {
								int health = obj->getHealth();
								obj->reduceHealth(bullet->getMaxHealth());
								bullet->reduceHealth(obj->getHealth() ? bullet->getHealth() : health);
								if (obj->getHealth() <= 0) {
									if (auto pOwner = static_cast<Bullet*>(bullet)->getOwner())
										pOwner->AddExp(obj->getExp());
									auto vel = math::combined(obj1->getVelocity(), obj1->getMass(), obj2->getVelocity(), obj2->getMass());
									bullet->setVelocity(vel);
									continue;
								}
							}
							auto vel = math::collision(bullet->getVelocity(), obj->getVelocity(), bullet->getPosition(), obj->getPosition(), bullet->getMass() / 2, obj->getMass());
							bullet->setVelocity(vel.first);
							obj->setVelocity(vel.second);
						}
					}
				}
			}
		}

		// 清理尸体
		if (networkManager.isHost()) {
			for (auto it = objects.begin(); it != objects.end(); ) {
				if ((*it)->isDie()) {
					delete* it;
					it = objects.erase(it);
				}
				else
					++it;
			}
		}

		//生成新的资源
		if (networkManager.isHost() && Global::resourceCount < maxResourceCount) {
			// 累积自上次生成资源以来的时间
			timeSinceLastSpawn += Global::deltaTime;

			// 如果累积时间大于生成资源所需的时间，尝试在地图的随机位置生成新资源
			if (timeSinceLastSpawn >= respawnTime) {
				timeSinceLastSpawn = 0; // 重置时间
				auto resource = new Object(Object::randomResourceType(), point(200, 200));
				resource->randomAddToMap();
				networkManager.sendPlayerResource(resource->getPosition());
			}
		}
	}
}

void render() {
	window.clear(sf::Color(100, 100, 100));

	if (menu.inMenu()) {
		drawGrid(15);
		menu.draw();
	}
	else {
		// 更新视图使其聚焦于圆
		view.setCenter(player->getPosition());
		window.setView(view);
		drawGrid(15);

		for (const auto& obj : objects)
			window.draw(*obj);
		
		for (const auto& line : Global::lines)
			window.draw(line);
		Global::lines.clear();

		// 在绘制UI之前，将视图重置为默认视图
		window.setView(window.getDefaultView());
		Global::ui.draw();
		Global::ui.drawMinimap();
		if (Global::isGameOver) {
			Global::ui.drawGameOver();
		}
	}
	window.display();
}

int main() {

	window.clear(sf::Color(100, 100, 100));

	player = new Player(15.f, blueTeamColor, point(400, 300), 1000);
	// 添加资源
	for (int i = 0; i < 250; i++) {
		auto resource = new Object(Object::randomResourceType(), point(200, 200));
		resource->randomAddToMap();
	}

	// 创建其他敌人AI
	for (int i = 0; i < 50; i++) {
		auto enemy = new AIPlayer(15.f, redTeamColor, point(200, 200), 100);
		enemy->randomAddToMap();
		enemy->setTurretRotation(random::randomFloat(0, 314));
	}

	// 创建一个文本对象来显示圆的位置
	Global::font.loadFromFile("fonts/chinese.ttf");

	// 初始化小地图的 RenderTexture
	Global::ui.initialiseMinimapTexture();

	while (window.isOpen()) {
		update();
		render();
	}

	return 0;
}
