#include <array>
#include <chrono>
#include <SFML/Graphics.hpp>

#include "Animation.hpp"
#include "Global.hpp"
#include "MapManager.hpp"
#include "Mushroom.hpp"
#include "Mario.hpp"
#include "Enemy.hpp"


Enemy::Enemy(const float i_x, const float i_y) :
	dead(0),
	horizontal_speed(0),
	vertical_speed(0),
	x(i_x),
	y(i_y),
	start_x(i_x), //  salvarea poziției inițiale X
	start_y(i_y)  //  salvarea poziției inițiale Y
{

}

bool Enemy::get_dead(const bool i_deletion) const
{
	return dead;
}
//schimbă starea inamicului la "mort"
void Enemy::die(const unsigned char i_death_type)
{
	dead = 1;
}
//returnează poziția de start a inamicului
sf::Vector2f Enemy::get_start_position() const {
	return sf::Vector2f(start_x, start_y);
}
//calculează și returnează hit box-ul inamicului
sf::FloatRect Enemy::get_hit_box() const {
	return sf::FloatRect({ x, y }, { static_cast<float>(CELL_SIZE), static_cast<float>(CELL_SIZE) });
}
//cum se misca tot inamici clasa parinte 