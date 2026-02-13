#include <array>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <SFML/Graphics.hpp>

#include "Animation.hpp"
#include "Global.hpp"
#include "MapManager.hpp"
#include "Mushroom.hpp"
#include "Mario.hpp"
#include "Enemy.hpp"
#include "Goomba.hpp"

Goomba::Goomba(const bool i_underground, const float i_x, const float i_y) :
	Enemy(i_x, i_y),
	no_collision_dying(0),
	underground(i_underground),
	death_timer(GOOMBA_DEATH_DURATION),
	walk_animation(CELL_SIZE, "GoombaWalk.png", GOOMBA_WALK_ANIMATION_SPEED),
	sprite(texture)
{
	horizontal_speed = -GOOMBA_SPEED; //merge stanga la început

	if (0 == underground)
	{
		texture.loadFromFile("GoombaDeath0.png");
	}
	else
	{
		texture.loadFromFile("UndergroundGoombaDeath0.png");
		walk_animation.set_texture_location("UndergroundGoombaWalk.png");
	}
}
//verifică dacă Goomba este mort (dacă i_deletion este adevărat, verifică doar dacă este marcat ca mort)
bool Goomba::get_dead(const bool i_deletion) const
{
	if (1 == i_deletion)
	{
		return dead;
	}
	else
	{
		return dead || no_collision_dying || GOOMBA_DEATH_DURATION > death_timer;
	}
}
//gestionează moartea Goomba în funcție de tipul de moarte
void Goomba::die(const unsigned char i_death_type)
{
	switch (i_death_type)
	{
	case 0:
		dead = 1;
		break;//moarte normală
	case 1:
		if (0 == no_collision_dying)
		{
			death_timer--;
		}
		break;
	case 2: //lovește de Mario
		if (GOOMBA_DEATH_DURATION == death_timer)
		{
			no_collision_dying = 1;
			vertical_speed = 0.5f * MARIO_JUMP_SPEED;
			if (0 == underground)
				texture.loadFromFile("GoombaDeath1.png");
			else
				texture.loadFromFile("UndergroundGoombaDeath1.png");
		}
		break;
	}
}

void Goomba::draw(const unsigned i_view_x, sf::RenderWindow& i_window)
{
	//desenează Goomba doar dacă este în zona vizibilă a ecranului
	if (-CELL_SIZE < std::round(y) && std::round(x) > static_cast<int>(i_view_x) - CELL_SIZE && std::round(x) < SCREEN_WIDTH + i_view_x && std::round(y) < SCREEN_HEIGHT)
	{
		if (1 == no_collision_dying || GOOMBA_DEATH_DURATION > death_timer)
		{
			sprite.setPosition({ static_cast<float>(std::round(x)), static_cast<float>(std::round(y)) });
			sprite.setTexture(texture);
			i_window.draw(sprite);
		}
		else
		{
			walk_animation.set_position(static_cast<short>(std::round(x)), static_cast<short>(std::round(y)));
			walk_animation.draw(i_window);
		}
	}
}

void Goomba::update(const unsigned i_view_x, const std::vector<std::shared_ptr<Enemy>>& i_enemies, const MapManager& i_map_manager, Mario& i_mario)
{
	//logica de actualizare doar dacă Goomba este în zona vizibilă a ecranului
	if (-CELL_SIZE < y && x >= static_cast<int>(i_view_x) - CELL_SIZE - ENTITY_UPDATE_AREA && x < ENTITY_UPDATE_AREA + SCREEN_WIDTH + i_view_x && y < SCREEN_HEIGHT)
	{
		std::vector<unsigned char> collision;
		sf::FloatRect hit_box = get_hit_box();

		vertical_speed = std::min(GRAVITY + vertical_speed, MAX_VERTICAL_SPEED);
		hit_box.position.y += vertical_speed;

		std::vector<sf::Vector2i> dummy_cells;
		collision = i_map_manager.map_collision({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall }, hit_box, dummy_cells);

		if (0 == no_collision_dying)
		{
			//coliziuni verticale
			if (std::all_of(collision.begin(), collision.end(), [](unsigned char v) { return v == 0; }))
			{
				bool changed = 0;
				if (0 == get_dead(0))
				{
					for (unsigned short a = 0; a < i_enemies.size(); a++)
					{
						if (shared_from_this() != i_enemies[a] && !i_enemies[a]->get_dead(0) && hit_box.findIntersection(i_enemies[a]->get_hit_box()))
						{
							changed = 1;
							if (0 > vertical_speed)
								y = i_enemies[a]->get_hit_box().size.y + i_enemies[a]->get_hit_box().position.y;
							else
								y = i_enemies[a]->get_hit_box().position.y - CELL_SIZE;

							vertical_speed = 0;
							break;
						}
					}
				}
				if (!changed) y += vertical_speed;
			}
			else
			{
				if (0 > vertical_speed)
					y = CELL_SIZE * (1 + std::floor((vertical_speed + y) / CELL_SIZE));
				else
					y = CELL_SIZE * (std::ceil((vertical_speed + y) / CELL_SIZE) - 1);
				vertical_speed = 0;
			}
			//mişcare orizontală și coliziuni cu alte entități
			if (0 == get_dead(0))
			{
				hit_box = get_hit_box();
				hit_box.position.x += horizontal_speed;

				collision = i_map_manager.map_collision({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall }, hit_box, dummy_cells);

				if (std::all_of(collision.begin(), collision.end(), [](unsigned char v) { return v == 0; }))
				{
					bool changed = 0;
					for (unsigned short a = 0; a < i_enemies.size(); a++)
					{
						if (shared_from_this() != i_enemies[a] && !i_enemies[a]->get_dead(0) && hit_box.findIntersection(i_enemies[a]->get_hit_box()))
						{
							changed = 1;
							horizontal_speed *= -1;//schimbare direcție la coliziune
							break;
						}
					}
					if (!changed) x += horizontal_speed;
				}
				else
				{
					//coliziune orizontală
					if (0 < horizontal_speed)
						x = CELL_SIZE * (std::ceil((horizontal_speed + x) / CELL_SIZE) - 1);
					else
						x = CELL_SIZE * (1 + std::floor((horizontal_speed + x) / CELL_SIZE));
					horizontal_speed *= -1;
				}
				//verificare coliziune cu Mario
				if (!i_mario.get_dead() && get_hit_box().findIntersection(i_mario.get_hit_box()))
				{
					if (0 < i_mario.get_vertical_speed())//mario sare pe goomba
					{
						die(1);
						game_score += 100; // PUNCTE GOOMBA UCIS
						i_mario.set_vertical_speed(0.5f * MARIO_JUMP_SPEED);
					}
					else
					{
						i_mario.die(0); // mario moare
					}
				}
				walk_animation.update();
			}
			else if (GOOMBA_DEATH_DURATION > death_timer)
			{
				if (0 < death_timer) death_timer--;
				else die(0);
			}
		}
		else
		{
			y += vertical_speed;
		}
	}

	if (SCREEN_HEIGHT <= y) die(0);//died bc it fell down 
}