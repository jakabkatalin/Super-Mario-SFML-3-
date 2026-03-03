#include <array>
#include <cmath>
#include <chrono>
#include <SFML/Graphics.hpp>

#include "Animation.hpp"
#include "Global.hpp"
#include "MapManager.hpp"
#include "Mushroom.hpp"
#include "Mario.hpp"
#include "Enemy.hpp"
#include "Goomba.hpp"
#include "Koopa.hpp"
#include "ConvertSketch.hpp"
//transforma desenul nivelului in obiecte reale de joc
void convert_sketch(const unsigned char i_current_level, unsigned short& i_level_finish, std::vector<std::shared_ptr<Enemy>>& i_enemies, sf::Color& i_background_color, MapManager& i_map_manager, Mario& i_mario, const std::vector<sf::Vector2f>& i_killed_enemies, const std::vector<sf::Vector2i>& i_collected_coins)
{
	unsigned short map_height;

	i_enemies.clear();// Curățăm lista inamicilor înainte de a reconstrui harta

	i_map_manager.update_map_sketch(i_current_level);
	i_map_manager.set_map_size(i_map_manager.get_map_sketch_width());

	i_level_finish = i_map_manager.get_map_sketch_width();// Valoare implicită, în caz că nu găsim linia de finish
	map_height = floor(i_map_manager.get_map_sketch_height() / 3.f);// Harta conține 3 straturi: nivelul de sus, nivelul subteran și fundalul
    // Setăm culoarea de fundal pe baza pixelului din colțul din stânga jos
	i_background_color = i_map_manager.get_map_sketch_pixel(0, i_map_manager.get_map_sketch_height() - 1);

	for (unsigned short a = 0; a < i_map_manager.get_map_sketch_width(); a++)
	{
		for (unsigned short b = 0; b < 2 * map_height; b++)
		{
			sf::Color pixel = i_map_manager.get_map_sketch_pixel(a, b);

			if (b < map_height)
			{
				if (sf::Color(182, 73, 0) == pixel)
				{
					i_map_manager.set_map_cell(a, b, Cell::Brick);
				}
				else if (sf::Color(255, 255, 0) == pixel)
				{
					// LOGICA RESTART MONEDA:
					bool already_collected = false;
					for (const auto& coin_pos : i_collected_coins)
					{
						if (coin_pos.x == a && coin_pos.y == b)
						{
							already_collected = true;
							break;
						}
					}

					if (already_collected)
					{
						i_map_manager.set_map_cell(a, b, Cell::Empty);
					}
					else
					{
						i_map_manager.set_map_cell(a, b, Cell::Coin);
					}
				}
				else if (sf::Color(0, 146, 0) == pixel || sf::Color(0, 182, 0) == pixel || sf::Color(0, 219, 0) == pixel)
				{
					i_map_manager.set_map_cell(a, b, Cell::Pipe);//tevi care nu se vad fiindca nu doresc a ramas aici :3
				}
				else if (sf::Color(255, 73, 85) == pixel || sf::Color(255, 146, 85) == pixel)
				{
					// LOGICA RESTART BLOCURI:
					// Verificăm dacă blocul a fost deja folosit (moneda sau ciuperca a fost luată)
					bool block_used = false;
					for (const auto& item_pos : i_collected_coins)
					{
						if (item_pos.x == a && item_pos.y == b)
						{
							block_used = true;
							break;
						}
					}

					if (block_used)
					{
						i_map_manager.set_map_cell(a, b, Cell::ActivatedQuestionBlock);
					}
					else
					{
						i_map_manager.set_map_cell(a, b, Cell::QuestionBlock);
					}
				}
				else if (sf::Color(0, 0, 0) == pixel || sf::Color(146, 73, 0) == pixel)
				{
					i_map_manager.set_map_cell(a, b, Cell::Wall);
				}
				else
				{
					i_map_manager.set_map_cell(a, b, Cell::Empty);

					if (sf::Color(0, 255, 255) == pixel)
					{
						i_level_finish = a;
					}
				}
			}
			else
			{
				float entity_x = static_cast<float>(CELL_SIZE * a);
				float entity_y = static_cast<float>(CELL_SIZE * (b - map_height));

				if (sf::Color(255, 0, 0) == pixel)
				{
					if (i_mario.get_x() == 0)
					{
						i_mario.set_position(entity_x, entity_y);
					}
				}
				else if (sf::Color(182, 73, 0) == pixel)
				{
					bool is_dead = false;
					for (const sf::Vector2f& killed_pos : i_killed_enemies)
					{
						if (killed_pos.x == entity_x && killed_pos.y == entity_y)
						{
							is_dead = true;
							break;
						}
					}

					if (!is_dead)
					{
						i_enemies.push_back(std::make_shared<Goomba>(sf::Color(0, 0, 85) == i_background_color, entity_x, entity_y));//polimorfism pentru inamici ii adauga in vectorul principal
					}
				}
				else if (sf::Color(0, 219, 0) == pixel)
				{
					bool is_dead = false;
					for (const sf::Vector2f& killed_pos : i_killed_enemies)
					{
						if (killed_pos.x == entity_x && killed_pos.y == entity_y)
						{
							is_dead = true;
							break;
						}
					}

					if (!is_dead)
					{
						i_enemies.push_back(std::make_shared<Koopa>(sf::Color(0, 0, 85) == i_background_color, entity_x, entity_y));
					}
				}
			}
		}
	}
}