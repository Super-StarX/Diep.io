#include <SFML/Graphics.hpp>
#include "Global.h"
#include "Data.h"

class Button {
public:
	Button(std::string text, float width, float height, sf::Color color, void (*callbackFunction)())
		: buttonShape(point(width, height)), action(callbackFunction) {
		buttonShape.setFillColor(color);
		buttonShape.setOutlineThickness(2);
		buttonShape.setOutlineColor(sf::Color{ color.r,color.g,color.b });

		buttonText.setFont(Global::font);
		buttonText.setString(text);
		buttonText.setFillColor(sf::Color::White);
		buttonText.setCharacterSize(15);
	}

	void setPosition(float x, float y) {
		buttonShape.setPosition(x, y);

		auto bounds = buttonText.getGlobalBounds();
		buttonText.setPosition(x - bounds.width / 2, y);
	}

	// ���������(x, y)���ĵ��
	bool onClick(int x, int y) const {
		return buttonShape.getGlobalBounds().contains(static_cast<float>(x), static_cast<float>(y));
	}

	void (*getAction())() {
		return action;
	}

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
	void (*action)(); // ����һ���ص������������Ǿ�̬��Ա��������Ϊ��ť�����¼�
};

class UI {
public:
	UI() {
		// ������ť�б�
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

		// �Ѱ�ť��������Ӧ��λ��
		float yOffset = 2;
		for (size_t i = 0; i < buttons.size(); ++i) {
			buttons[i].setPosition(4, window.getSize().y - (i + 1) * (buttonHeight + yOffset));
		}

		playerPosText.setFont(Global::font);
		playerPosText.setCharacterSize(14);
		playerPosText.setFillColor(sf::Color::White);
		playerPosText.setPosition(10, 10);
	}

	void addButton(const Button& button) {
		buttons.push_back(button);
	}

	void handleMouseClick(int x, int y) {
		for (auto& button : buttons) {
			if (button.onClick(x, y)) {
				button.getAction()(); // �����⵽�������ִ�лص�����
				break;
			}
		}
	}

	void initialiseMinimapTexture();
	point getPositionOnMinimap(point position);
	void drawMinimap();
	void drawGameOver();
	void draw();

private:
	sf::Text playerPosText;
	sf::Text gameOverText;
	std::vector<Button> buttons;
	sf::RectangleShape minimap;
	float minimapResize = 0.1f;
};
