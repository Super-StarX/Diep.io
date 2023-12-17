#pragma once
#include <SFML/Graphics.hpp>
#include "Global.h"
#include "Data.h"

class Button {
public:
	Button(std::string text, float width, float height, sf::Color color, void (*callbackFunction)());

	void setPosition(float x, float y) {
		buttonShape.setPosition(x, y);
		buttonText.setPosition(x - buttonText.getGlobalBounds().width / 2, y);
	}

	// 检查在坐标(x, y)处的点击
	bool onClick(int x, int y) const {
		return buttonShape.getGlobalBounds().contains(static_cast<float>(x), static_cast<float>(y));
	}

	void (*getAction())() { return action; }
	static void upgradeBulletSpeed();
	static void upgradeBulletPenetration();
	static void upgradeBulletDamage();
	static void upgradeHealthRegen();
	static void upgradeReloadInterval();
	static void upgradeMovementSpeed();

	void draw() {
		window.draw(buttonShape);
		window.draw(buttonText);
	}

private:
	sf::RectangleShape buttonShape;
	sf::Text buttonText;
	void (*action)(); // 定义一个回调函数（可以是静态成员函数）作为按钮单击事件
};

class UI {
public:
	UI();

	void addButton(const Button& button) { buttons.push_back(button); }
	void handleMouseClick(int x, int y);
	void initialiseMinimapTexture();
	point getPositionOnMinimap(point position);
	void drawGrid();
	void drawMinimap();
	void drawGameOver();
	void draw();

	static void resize(sf::Event& event);

private:
	sf::Text playerPosText;
	sf::Text gameOverText;
	std::vector<Button> buttons;
	sf::RectangleShape minimap;
	float minimapResize = 0.1f;
};
