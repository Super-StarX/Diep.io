#pragma once
#include <SFML/Graphics.hpp>

class AIPlayer;
class Player;
class Bullet;
class Object;
class UI;
class Menu;
class NetworkManager;

class Global {
public:
	static sf::RenderWindow window;
	static UI ui;
	static bool isGameOver;
	static sf::Font font;
	static Menu menu;
	static NetworkManager networkManager;

	static constexpr int mapWidth = 5000;
	static constexpr int mapHeight = 5000;

	static float deltaTime;
	static float inertia;
	static Player currentPlayer;
	static std::vector<Player> playersVector;
	static std::vector<Bullet> bulletVector;
	static std::vector<Object> resourceVector;
	static std::vector<AIPlayer> enemiesVector;
};

#define window Global::window
#define menu Global::menu
#define networkManager Global::networkManager
#define mapWidth Global::mapWidth
#define mapHeight Global::mapHeight

#define players Global::playersVector
#define player Global::currentPlayer
#define bullets Global::bulletVector
#define resources Global::resourceVector
#define enemies Global::enemiesVector