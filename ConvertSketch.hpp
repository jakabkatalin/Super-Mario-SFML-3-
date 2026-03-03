#pragma once

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

class Enemy;
class MapManager;
class Mario;

void convert_sketch(const unsigned char i_current_level, unsigned short& i_level_finish, std::vector<std::shared_ptr<Enemy>>& i_enemies, sf::Color& i_background_color, MapManager& i_map_manager, Mario& i_mario, const std::vector<sf::Vector2f>& i_killed_enemies, const std::vector<sf::Vector2i>& i_collected_coins);