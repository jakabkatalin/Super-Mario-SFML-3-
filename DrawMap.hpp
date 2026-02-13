#pragma once
#include <SFML/Graphics.hpp>
#include "Global.hpp"
void draw_map(unsigned i_view_x, const sf::Image& i_map_sketch, sf::RenderWindow& i_window, const sf::Texture& i_map_texture, const Map& i_map);