#pragma once

#include <SFML/Graphics.hpp>
#include "Global.hpp"

class MapManager;

class Mushroom {
	//incapsulare, nu pot fi accesate direct din exterior
	bool dead; // Starea entității (dacă mai există sau nu)
	char horizontal_direction; // 1 pentru dreapta, -1 pentru stânga

	float vertical_speed;
	float x;
	float y; 
	float start_y; // Poziția inițială folosită pentru animația de ieșire

	sf::Sprite sprite;

public:
	Mushroom(float i_x, float i_y, const sf::Texture& i_texture);

	bool get_dead() const;
	void set_dead(bool i_value);

	void update(unsigned i_view_x, const MapManager& i_map_manager);
	void draw(unsigned i_view_x, sf::RenderWindow& i_window);

	sf::FloatRect get_hit_box() const;
};