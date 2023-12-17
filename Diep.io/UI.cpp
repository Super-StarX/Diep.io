
#include <iostream>
#include "UI.h"
#include "AIPlayer.h"

Button::Button(std::string text, float width, float height, sf::Color color, void (*callbackFunction)())
	: buttonShape(point(width, height)), action(callbackFunction) {
	buttonShape.setFillColor(color);
	buttonShape.setOutlineThickness(2);
	buttonShape.setOutlineColor(sf::Color{ color.r,color.g,color.b });

	buttonText.setFont(Global::font);
	buttonText.setString(text);
	buttonText.setFillColor(sf::Color::White);
	buttonText.setCharacterSize(15);
}

void Button::upgradeBulletSpeed() {
	player->upgradeBulletSpeed(15);
	std::cout << "upgradeBulletSpeed" << std::endl;
}
void Button::upgradeBulletPenetration() {
	player->upgradeBulletPenetration(1.f);
	std::cout << "upgradeBulletPenetration" << std::endl;
}
void Button::upgradeBulletDamage() {
	player->upgradeBulletDamage(5);
	std::cout << "upgradeBulletDamage" << std::endl;
}
void Button::upgradeHealthRegen() {
	//player->HealthRegen += 1;
	std::cout << "upgradeHealthRegen" << std::endl;
}
void Button::upgradeReloadInterval() {
	player->upgradeReloadInterval(0.02f);
	std::cout << "upgradeReloadInterval" << std::endl;
}
void Button::upgradeMovementSpeed() {
	player->upgradeMovementSpeed(30.f);
	std::cout << "upgradeMovementSpeed" << std::endl;
}

UI::UI() {
	// 创建按钮列表
	float buttonWidth = 125;
	float buttonHeight = 20;
	buttons = {
		Button("MovementSpeed", buttonWidth, buttonHeight, sf::Color{0,108,192,70}, Button::upgradeMovementSpeed),
		Button("Reload", buttonWidth, buttonHeight, sf::Color{0,108,192,70}, Button::upgradeReloadInterval),
		Button("HealthRegen", buttonWidth, buttonHeight, sf::Color{0,108,192,70}, Button::upgradeHealthRegen),
		Button("BulletDamage", buttonWidth, buttonHeight, sf::Color{0,108,192,70}, Button::upgradeBulletDamage),
		Button("BulletPenetration", buttonWidth, buttonHeight, sf::Color{0,108,192,70}, Button::upgradeBulletPenetration),
		Button("BulletSpeed", buttonWidth, buttonHeight, sf::Color{0,108,192,70}, Button::upgradeBulletSpeed)
	};

	// 把按钮放置在相应的位置
	float yOffset = 2;
	for (size_t i = 0; i < buttons.size(); ++i) {
		buttons[i].setPosition(4, window.getSize().y - (i + 1) * (buttonHeight + yOffset));
	}

	playerPosText.setFont(Global::font);
	playerPosText.setCharacterSize(14);
	playerPosText.setFillColor(sf::Color::White);
	playerPosText.setPosition(10, 10);
}

void UI::handleMouseClick(int x, int y) {
	for (auto& button : buttons) {
		if (button.onClick(x, y)) {
			button.getAction()(); // 如果检测到点击，则执行回调函数
			break;
		}
	}
}

void UI::initialiseMinimapTexture() {
	point(mapWidth, mapHeight);
    minimap.setSize(point(100,100));
	minimap.setPosition(point(window.getSize().x - minimap.getSize().x - 2, window.getSize().y - minimap.getSize().y - 2));
    minimap.setFillColor(sf::Color{ 201, 201, 210 });
    minimap.setOutlineThickness(2);
    minimap.setOutlineColor(sf::Color{ 150,150,150 });
}

point UI::getPositionOnMinimap(point position) {
	return minimap.getPosition() + 
		point{ position.x / mapWidth * minimap.getSize().x ,
					  position.y / mapHeight * minimap.getSize().y };
}

void UI::drawGrid() {
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

void UI::drawMinimap() {
	window.draw(minimap); // 设置右下角位置

	for (auto& obj : players) {
		sf::CircleShape shape(2, 30);
		shape.setFillColor(obj->getColor());
		shape.setPosition(getPositionOnMinimap(obj->getPosition()));
		window.draw(shape);
	}
}

void UI::drawGameOver() {
	// 在画面上显示游戏结束，并暂停游戏
	gameOverText.setFont(Global::font);
	gameOverText.setString("Game Over");
	gameOverText.setPosition(350, 250);
	gameOverText.setFillColor(sf::Color::Red);
	gameOverText.setCharacterSize(50);
	window.draw(gameOverText);
}

void UI::draw() {
	if (player->upgradeCount())
		for (auto& button : buttons)
			button.draw();

	// 更新坐标信息
	std::ostringstream ss;
	ss << "x: " << player->getPosition().x << ", y: " << player->getPosition().y <<
		"\nfps:" << 1 / Global::deltaTime <<
		"\nenemy:" << enemies.size() <<
		"\nresource:" << objects.size() <<
		"\ncurSpeed:" << player->getSpeed() <<
		"\nExp:" << player->getExp()<< ", Level:" << player->getLevel();
	playerPosText.setString(ss.str());
	window.draw(playerPosText);

}

void UI::resize(sf::Event& event){
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
	viewer.setSize(viewWidth, viewHeight);

	// 计算并设置新的视口，使内容居中
	sf::FloatRect viewport(
		(windowWidth - viewWidth) / 2 / windowWidth,
		(windowHeight - viewHeight) / 2 / windowHeight,
		viewWidth / windowWidth,
		viewHeight / windowHeight
	);
	viewer.setViewport(viewport);
}