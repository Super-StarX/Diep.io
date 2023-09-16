#include <SFML/Graphics.hpp>

#include "Object.h"
#include "AIPlayer.h"
#include "Bullet.h"
#include "Global.h"
#include "Random.h"
#include "UI.h"
#include "Menu.h"
#include "Network.h"

// 固定数值
constexpr float moveSpeed = 300.0f;
float Global::inertia = 0.1f; // 摩擦力
const int maxResourceCount = 500; // 最大资源数
float respawnTime = 0.1f; // 控制新资源生成间隔（秒）
int defaultWindowWidth = 800;
int defaultWindowHeight = 600;

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

// 创建对象
Player Global::currentPlayer(15.f, sf::Color::Blue, sf::Vector2f(400, 300), 1000);
std::vector<Player> players;
std::vector<Bullet> bullets;
std::vector<Object> resources{ 250, Object(10.f, sf::Color::Green, sf::Vector2f(200, 200), 100), };
std::vector<AIPlayer> enemies{ 50, AIPlayer(15.f, sf::Color::Red, sf::Vector2f(200, 200), 100), };

sf::Vector2f getWindowToWorldPosition(const sf::View& view) {
	// 获取鼠标在窗口中的位置（窗口坐标系）
	sf::Vector2i windowMousePosition = sf::Mouse::getPosition(window);

	// 将窗口坐标转换为视图坐标系
	sf::Vector2f viewMousePosition = window.mapPixelToCoords(windowMousePosition, view);

	return viewMousePosition;
}

void drawGrid(int cellSize) {
	constexpr int borderSize = 20;
	//边界
	sf::RectangleShape border(sf::Vector2f(mapWidth + borderSize * 2, mapHeight + borderSize * 2));
	border.setFillColor(sf::Color(150, 150, 150));
	border.setPosition(-borderSize, -borderSize);
	window.draw(border);

	//地图
	sf::RectangleShape map(sf::Vector2f(mapWidth, mapHeight));
	map.setFillColor(sf::Color(201, 201, 210));
	window.draw(map);

	//网格
	sf::RectangleShape line(sf::Vector2f(mapWidth, 1));
	line.setFillColor(sf::Color(170, 170, 170));

	for (size_t y = 0; y <= mapHeight / cellSize; ++y) {
		line.setPosition(0, static_cast<float>(y * cellSize));
		window.draw(line);
	}

	line.setSize(sf::Vector2f(1, mapHeight));

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
			if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					// 获取点击的世界坐标
					sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

					// 如果用户点击在屏幕左下角（按钮区域）
					if (mousePosition.x <= 100 && mousePosition.y >= defaultWindowHeight - 370)
					{
						isClickButton = true;
						Global::ui.handleMouseClick(event.mouseButton.x, event.mouseButton.y);
					}
				}
			}
		}
	}

	if (!menu.inMenu()) {
		// 如果游戏未结束并且用户聚焦于当前应用
		if (hasFocus && !isClickButton && !Global::isGameOver) {
			// 更新炮塔方向
			sf::Vector2f mousePosition = getWindowToWorldPosition(view);
			player.calcTurretRotation(mousePosition);

			// 根据用户输入(wsad)移动炮台
			player.checkMove(moveSpeed);

			// 由于存在惯性,每帧更新炮台的位置
			player.update();
			player.checkCollision();
			networkManager.sendPlayerPositionUpdate();

			// 如果按下鼠标左键,则进行开火
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
				if (player.fire(mousePosition, sf::Color::Blue))
					networkManager.sendPlayerFireEvent(mousePosition);
		}

		for (auto& enemy : players) {
			enemy.update();
			enemy.checkCollision();
		}

		// 资源的惯性
		for (auto& resource : resources) {
			resource.update();
		}

		// 子弹的惯性、碰撞
		for (auto& bullet : bullets) {
			bullet.update();
			bullet.checkCollision();
		}

		// AI的寻敌、惯性、碰撞
		for (auto& enemy : enemies) {
			enemy.think();
			enemy.update();
			enemy.checkCollision();
		}

		// 从内存中真正删除死去的资源
		resources.erase(std::remove_if(resources.begin(), resources.end(),
			[](Object& resource) {
				return !resource.getHealth();
			}), resources.end());

		bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
			[](Bullet& bullet) {
				return bullet.isVivid();
			}), bullets.end());

		enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
			[](AIPlayer& enemy) {
				return !enemy.getHealth();
			}), enemies.end());

		//生成新的资源
		if (networkManager.isHost() && resources.size() < maxResourceCount) {
			// 累积自上次生成资源以来的时间
			timeSinceLastSpawn += Global::deltaTime;

			// 如果累积时间大于生成资源所需的时间，尝试在地图的随机位置生成新资源
			if (timeSinceLastSpawn >= respawnTime) {
				timeSinceLastSpawn = 0; // 重置时间
				Object resource(10.f, sf::Color::Green, sf::Vector2f(200, 200), 100);
				resource.randomAddToMap();
				resources.emplace_back(std::move(resource));
				networkManager.sendPlayerResource(resource.getPosition());
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
		view.setCenter(player.getPosition());
		window.setView(view);
		drawGrid(15);

		// 绘制玩家和其他目标
		window.draw(player);
		for (const auto& other : resources)
			window.draw(other);
		for (const auto& bullet : bullets)
			window.draw(bullet);
		for (const auto& enemy : enemies)
			window.draw(enemy);
		for (const auto& otherPlayer : players)
			window.draw(otherPlayer);

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

	// 添加资源
	for (auto& resource : resources) {
		resource.randomAddToMap();
	}

	// 创建其他敌人AI
	for (auto& enemy : enemies) {
		enemy.randomAddToMap();
		enemy.setTurretRotation(random::randomFloat(0, 314));
	}

	// 创建一个文本对象来显示圆的位置
	sf::Font initfont;
	initfont.loadFromFile("fonts/chinese.ttf");
	Global::font = initfont;

	// 初始化小地图的 RenderTexture
	Global::ui.initialiseMinimapTexture();

	while (window.isOpen()) {
		update();
		render();
	}

	return 0;
}
