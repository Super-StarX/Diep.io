#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Player;
class Turret
{
public:
	Turret(Player* owner, float sizeX, float sizeY, point offest, sf::Color color, float offestDir, float scale);
	~Turret() = default;

	sf::RectangleShape& getShape() { return shape; };
	void setRotation(float dir) { shape.setRotation(dir + offestDir); }
	void setPosition(float x, float y) { shape.setPosition(x, y); }

	point fire(Player* owner, point target) const;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	sf::RectangleShape shape;
	point offest{ 0.f,0.f };
	float offestDir{ 0.f };
};
