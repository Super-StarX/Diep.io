
#include <iostream>
#include "UI.h"
#include "AIPlayer.h"
void Button::upgradeBulletSpeed() {
	player.upgradeBulletSpeed(1.3f);
	std::cout << "upgradeBulletSpeed" << std::endl;
}
void Button::upgradeBulletPenetration() {
	player.upgradeBulletPenetration(1.f);
	std::cout << "upgradeBulletPenetration" << std::endl;
}
void Button::upgradeBulletDamage() {
	player.upgradeBulletDamage(5);
	std::cout << "upgradeBulletDamage" << std::endl;
}
void Button::upgradeHealthRegen() {
	//player.HealthRegen += 1;
	std::cout << "upgradeHealthRegen" << std::endl;
}
void Button::upgradeReloadInterval() {
	player.upgradeReloadInterval(0.02f);
	std::cout << "upgradeReloadInterval" << std::endl;
}
void Button::upgradeMovementSpeed() {
	player.upgradeMovementSpeed(30.f);
	std::cout << "upgradeMovementSpeed" << std::endl;
}

void UI::initialiseMinimapTexture() {
	sf::Vector2f(mapWidth, mapHeight);
    minimap.setSize(sf::Vector2f(100,100));
	minimap.setPosition(sf::Vector2f(window.getSize().x - minimap.getSize().x - 2, window.getSize().y - minimap.getSize().y - 2));
    minimap.setFillColor(sf::Color{ 201, 201, 210 });
    minimap.setOutlineThickness(2);
    minimap.setOutlineColor(sf::Color{ 150,150,150 });
}

sf::Vector2f UI::getPositionOnMinimap(sf::Vector2f position) {
	return minimap.getPosition() + 
		sf::Vector2f{ position.x / Global::mapWidth * minimap.getSize().x ,
					  position.y / Global::mapHeight * minimap.getSize().y };
}

void UI::drawMinimap() {
	window.draw(minimap); // 设置右下角位置
    sf::CircleShape playerShape(3, 30);
    playerShape.setFillColor(sf::Color::Green);
    //playerShape.setOrigin(player.getRadius(), player.getRadius());
    playerShape.setPosition(getPositionOnMinimap(player.getPosition()));
    window.draw(playerShape);

    for (auto& enemy : enemies) {
        sf::CircleShape enemyShape(2, 30);
        enemyShape.setFillColor(sf::Color::Red);
        //enemyShape.setOrigin(enemy.getRadius(), enemy.getRadius());
        enemyShape.setPosition(getPositionOnMinimap(enemy.getPosition()));
        window.draw(enemyShape);
    }

	for (auto& otherPlayer : players) {
		sf::CircleShape enemyShape(2, 30);
		enemyShape.setFillColor(sf::Color::Blue);
		//enemyShape.setOrigin(enemy.getRadius(), enemy.getRadius());
		enemyShape.setPosition(getPositionOnMinimap(otherPlayer.getPosition()));
		window.draw(enemyShape);
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
	if (player.upgradeCount())
		for (auto& button : buttons)
			button.draw();

	// 更新坐标信息
	std::ostringstream ss;
	ss << "x: " << player.getPosition().x << ", y: " << player.getPosition().y <<
		"\nfps:" << 1 / Global::deltaTime <<
		"\nenemy:" << enemies.size() <<
		"\nresource:" << resources.size() <<
		"\ncurSpeed:" << player.getSpeed() <<
		"\nExp:" << player.getExp()<< ", Level:" << player.getLevel();
	playerPosText.setString(ss.str());
	window.draw(playerPosText);

}