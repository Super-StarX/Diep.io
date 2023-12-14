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

	static float deltaTime;
	static Player* currentPlayer;
	static int resourceCount;
	static std::vector<Player*> playersVector;
	static std::vector<Bullet*> bulletVector;
	static std::vector<Object*> objectVector;
	static std::vector<AIPlayer*> enemiesVector;
	static std::vector<sf::VertexArray> lines;
};

#define window Global::window
#define menu Global::menu
#define networkManager Global::networkManager

#define players Global::playersVector
#define player Global::currentPlayer
#define bullets Global::bulletVector
#define objects Global::objectVector
#define enemies Global::enemiesVector