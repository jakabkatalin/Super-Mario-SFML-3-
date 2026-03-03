#include <array>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <SFML/Graphics.hpp>

#include "Animation.hpp"
#include "Global.hpp"
#include "MapManager.hpp"
#include "Enemy.hpp"
#include "Mario.hpp"
//creierul nivelului, citeste harta , plaseza blocurile, gestioneaza si calculeaza coliziunile 
//Constructorul: inițializează animațiile pentru monede și blocuri și încarcă textura hărții
MapManager::MapManager() :
	coin_animation(CELL_SIZE, "Coin.png", COIN_ANIMATION_SPEED),
	question_block_animation(CELL_SIZE, "QuestionBlock.png", QUESTION_BLOCK_ANIMATION_SPEED),
	cell_sprite(map_texture)
{
	map_texture.loadFromFile("Map.png");
}
// Returnează înălțimea schiței hărții
unsigned short MapManager::get_map_sketch_height() const
{
	return static_cast<unsigned short>(map_sketch.getSize().y);
}
// Returnează lățimea imaginii sursa
unsigned short MapManager::get_map_sketch_width() const
{
	return static_cast<unsigned short>(map_sketch.getSize().x);
}
// Returnează lățimea hărții
unsigned short MapManager::get_map_width() const
{
	return static_cast<unsigned short>(map.size());
}
//creeaza cele  4 particule care apar cand un bloc de caramida este spart
void MapManager::add_brick_particles(const unsigned short i_x, const unsigned short i_y)
{
	brick_particles.push_back(Object(i_x, i_y, -0.25f * BRICK_PARTICLE_SPEED, -1.5f * BRICK_PARTICLE_SPEED));
	brick_particles.push_back(Object(i_x + 0.5f * CELL_SIZE, i_y, 0.25f * BRICK_PARTICLE_SPEED, -1.5f * BRICK_PARTICLE_SPEED));
	brick_particles.push_back(Object(i_x, i_y + 0.5f * CELL_SIZE, -0.5f * BRICK_PARTICLE_SPEED, -BRICK_PARTICLE_SPEED));
	brick_particles.push_back(Object(i_x + 0.5f * CELL_SIZE, i_y + 0.5f * CELL_SIZE, 0.5f * BRICK_PARTICLE_SPEED, -BRICK_PARTICLE_SPEED));
}
// Adaugă o monedă care sare dintr-un bloc cu semn de întrebare
void MapManager::add_question_block_coin(const unsigned short i_x, const unsigned short i_y)
{
	question_block_coins.push_back(Object(i_x, i_y, 0, COIN_JUMP_SPEED));
}
//transformă schița hărții într-o hartă de joc utilizabilă
void MapManager::convert_sketch(std::vector<std::shared_ptr<Enemy>>& i_enemies, sf::Color& i_color, MapManager& i_map_manager, Mario& i_mario, const std::vector<sf::Vector2i>& i_collected_items)
{ //std::vector<std::shared_ptr<Enemy>>& i_enemies polimoffism pentru inamici
	unsigned short map_width = get_map_sketch_width();
	unsigned short map_height = get_map_sketch_height() / 3; //impartim la 3 deoarece harta conține 3 straturi: nivelul de sus, nivelul subteran și fundalul

	set_map_size(map_width);

	for (unsigned short a = 0; a < map_width; a++)
	{
		for (unsigned short b = 0; b < map_height; b++)
		{
			set_map_cell(a, b, Cell::Empty);

			sf::Color pixel = get_map_sketch_pixel(a, b);
			
			if (sf::Color(182, 73, 0) == pixel) // Maro : bloc de cărămidă
			{
				set_map_cell(a, b, Cell::Brick);
			}
			//moneda sau bloc cu semn de întrebare
			else if (sf::Color(255, 255, 0) == pixel || sf::Color(255, 73, 85) == pixel)
			{
				if (sf::Color(255, 255, 0) == pixel)
				{
					set_map_cell(a, b, Cell::Coin);
				}
				else
				{
					set_map_cell(a, b, Cell::QuestionBlock);
				}
			}
			// podea/perete
			else if (sf::Color(0, 0, 0) == pixel) // Negru
			{
				set_map_cell(a, b, Cell::Wall);
			}
			// poz start Mario
			else if (sf::Color(0, 255, 0) == pixel)
			{
				i_mario.set_position(static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b));
			}
		}
	}

	// Salvăm harta curată (fără eliminări) ca referință
	map_initial = map;

	// Aplicăm eliminările persistente
	reset(i_collected_items);
}
// Resetează harta la starea inițială, eliminând permanent obiectele colectate/distruse
void MapManager::reset(const std::vector<sf::Vector2i>& i_collected_items)
{
	// Revenim la starea inițială
	map = map_initial;

	// Eliminăm permanent blocurile și monedele din listă
	for (const sf::Vector2i& item : i_collected_items)
	{
		if (item.x < map.size() && item.y < map[0].size())
		{
			// Dacă a fost un QuestionBlock, în Mario::update el devine ActivatedQuestionBlock.
			// Dar conform cerinței tale de a elibera spațiul, îl facem Empty.
			map[item.x][item.y] = Cell::Empty;
		}
	}
}
// Desenează harta pe fereastra dată, fie fundalul, fie elementele de prim-plan
void MapManager::draw_map(const bool i_draw_background, const bool i_underground, const unsigned i_view_x, sf::RenderWindow& i_window)
{
	unsigned short map_end = static_cast<unsigned short>(ceil((SCREEN_WIDTH + i_view_x) / static_cast<float>(CELL_SIZE)));
	unsigned short map_height = static_cast<unsigned short>(floor(map_sketch.getSize().y / 3.f));
	unsigned short map_start = static_cast<unsigned short>(floor(i_view_x / static_cast<float>(CELL_SIZE)));
	
	if (0 == i_draw_background)// Desenăm monedele care sar din blocurile cu semn de întrebare
	{
		for (const Object& question_block_coin : question_block_coins)
		{
			coin_animation.set_position(question_block_coin.x, question_block_coin.y);
			coin_animation.draw(i_window);//abstractizare animație monedă
		}
	}

	for (unsigned short a = map_start; a < map_end; a++)
	{
		if (a >= map.size()) continue;

		for (unsigned short b = 0; b < map_height; b++)
		{
			if (Cell::Pipe == map[a][b]) continue;

			unsigned short sprite_x = 0;
			unsigned short sprite_y = 0;

			cell_sprite.setPosition({ static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b) });

			if (1 == i_draw_background)
			{
				// Desenăm fundalul
				sf::Color pixel = map_sketch.getPixel({ static_cast<unsigned int>(a), static_cast<unsigned int>(b + 2 * map_height) });

				if (255 == pixel.a)
				{
					if (sf::Color(255, 255, 255) == pixel) { sprite_x = 0; sprite_y = 0; }
					cell_sprite.setTextureRect(sf::IntRect({ CELL_SIZE * sprite_x, CELL_SIZE * sprite_y }, { CELL_SIZE, CELL_SIZE }));
					i_window.draw(cell_sprite);
				}
			}
			else if (Cell::Empty != map[a][b])
			{
				// Desenăm celulele de prim-plan
				if (Cell::Coin == map[a][b])
				{
					coin_animation.set_position(static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b));
					coin_animation.draw(i_window);
				}
				else if (Cell::QuestionBlock == map[a][b])
				{
					question_block_animation.set_position(static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b));
					question_block_animation.draw(i_window);
				}
				else
				{
					//alegem sprite-ul potrivit pentru celulă
					sprite_y = 2 * i_underground;

					if (Cell::ActivatedQuestionBlock == map[a][b]) {
						sprite_x = 1;
					}
					else if (Cell::Wall == map[a][b]) {
						sprite_x = 0;
					}
					else if (Cell::Brick == map[a][b]) {
						sprite_x = 0;
					}

					cell_sprite.setTextureRect(sf::IntRect({ CELL_SIZE * sprite_x, CELL_SIZE * sprite_y }, { CELL_SIZE, CELL_SIZE }));
					i_window.draw(cell_sprite);
				}
			}
		}
	}
	// Desenăm particulele de cărămidă distruse
	if (0 == i_draw_background)
	{
		for (const Object& brick_particle : brick_particles)
		{
			cell_sprite.setPosition({ brick_particle.x, brick_particle.y });
			cell_sprite.setTextureRect(sf::IntRect({ static_cast<int>(0.25f * CELL_SIZE), static_cast<int>(CELL_SIZE * (0.25f + 2 * i_underground)) },
				{ static_cast<int>(0.5f * CELL_SIZE), static_cast<int>(0.5f * CELL_SIZE) }));
			i_window.draw(cell_sprite);
		}
	}
}
//setez o celulă a hărții la o anumită valoare
void MapManager::set_map_cell(const unsigned short i_x, const unsigned short i_y, const Cell& i_cell)
{
	if (i_x < map.size() && i_y < map[0].size())
	{
		map[i_x][i_y] = i_cell;
	}
}
// Setează dimensiunea hărții
void MapManager::set_map_size(const unsigned short i_new_size)
{
	map.clear();
	map.resize(i_new_size);
}
// Actualizează schița hărții pentru nivelul curent
void MapManager::update_map_sketch(const unsigned char i_current_level)
{
	// in loc sa scriu manual coordonatele fiecarui bloc, creez un sistem care citeste un fisier de tip imagine 
	map_sketch.loadFromFile("LevelSketch" + std::to_string(static_cast<unsigned short>(i_current_level)) + ".png");
}
// Actualizează pozițiile monedelor și particulelor
void MapManager::update()
{
	for (Object& question_block_coin : question_block_coins)
	{
		question_block_coin.vertical_speed += GRAVITY;
		question_block_coin.y += question_block_coin.vertical_speed;
	}

	for (Object& brick_particle : brick_particles)
	{
		brick_particle.vertical_speed += GRAVITY;
		brick_particle.x += brick_particle.horizontal_speed;
		brick_particle.y += brick_particle.vertical_speed;
	}
	// Eliminăm particulele care au ieșit în afara ecranului
	brick_particles.erase(std::remove_if(brick_particles.begin(), brick_particles.end(), [](const Object& i_brick_particle)
		{
			return SCREEN_HEIGHT <= i_brick_particle.y;
		}), brick_particles.end());

	question_block_coins.erase(std::remove_if(question_block_coins.begin(), question_block_coins.end(), [](const Object& i_question_block_coin)
		{
			return 0 <= i_question_block_coin.vertical_speed;
		}), question_block_coins.end());
	
	coin_animation.update();//abstractizare animație monedă
	question_block_animation.update();
}
//Funcție principală de coliziune: detectează dacă Mario atinge anumite celule
std::vector<unsigned char> MapManager::map_collision(const std::vector<Cell>& i_check_cells, const sf::FloatRect& i_hitbox, std::vector<sf::Vector2i>& i_collected_items) const
{
	std::vector<unsigned char> output;

	for (short a = static_cast<short>(floor(i_hitbox.position.y / CELL_SIZE)); a <= floor((ceil(i_hitbox.size.y + i_hitbox.position.y) - 1) / CELL_SIZE); a++)
	{
		output.push_back(0);

		for (short b = static_cast<short>(floor(i_hitbox.position.x / CELL_SIZE)); b <= floor((ceil(i_hitbox.position.x + i_hitbox.size.x) - 1) / CELL_SIZE); b++)
		{
			if (0 <= b && b < map.size())
			{
				if (0 <= a && a < map[0].size())
				{
					// Colectare monedă la atingere
					if (Cell::Coin == map[b][a]) {
						const_cast<MapManager*>(this)->set_map_cell(b, a, Cell::Empty);
						i_collected_items.push_back(sf::Vector2i(b, a));
						game_coins++;
						game_score += 200;
					}
					//BITMASKING: inloc sa intrebam fiecare obiect daca sa lovit de Mario, folosim puterile lui 2 pentru a reprezenta coliziunile, daca M se afla la coordonatele x,y intregam matricea hartii
					if (i_check_cells.end() != std::find(i_check_cells.begin(), i_check_cells.end(), map[b][a]))
					{
						//calculam bitul corespunzător coloanei lovite folosind puteri ale lui 2, sistemul de coliziuni fiind bazat pe bitmasking
						output[a - floor(i_hitbox.position.y / CELL_SIZE)] += static_cast<unsigned char>(pow(2, floor((ceil(i_hitbox.position.x + i_hitbox.size.x) - 1) / CELL_SIZE) - b));
					}
				}
			}
			else if (i_check_cells.end() != std::find(i_check_cells.begin(), i_check_cells.end(), Cell::Wall))
			{
				output[a - floor(i_hitbox.position.y / CELL_SIZE)] += static_cast<unsigned char>(pow(2, floor((ceil(i_hitbox.position.x + i_hitbox.size.x) - 1) / CELL_SIZE) - b));
			}
		}
	}

	return output;
}
//supraincarcă funcția de coliziune pentru a returna și celulele de coliziune
std::vector<unsigned char> MapManager::map_collision(const std::vector<Cell>& i_check_cells, std::vector<sf::Vector2i>& i_collision_cells, const sf::FloatRect& i_hitbox) const
{
	std::vector<unsigned char> output;
	i_collision_cells.clear();

	for (short a = static_cast<short>(floor(i_hitbox.position.y / CELL_SIZE)); a <= floor((ceil(i_hitbox.size.y + i_hitbox.position.y) - 1) / CELL_SIZE); a++)
	{
		output.push_back(0);

		for (short b = static_cast<short>(floor(i_hitbox.position.x / CELL_SIZE)); b <= floor((ceil(i_hitbox.position.x + i_hitbox.size.x) - 1) / CELL_SIZE); b++)
		{
			if (0 <= b && b < map.size())
			{
				if (0 <= a && a < map[0].size())
				{
					if (i_check_cells.end() != std::find(i_check_cells.begin(), i_check_cells.end(), map[b][a]))
					{
						i_collision_cells.push_back(sf::Vector2i(b, a));
						output[a - floor(i_hitbox.position.y / CELL_SIZE)] += static_cast<unsigned char>(pow(2, floor((ceil(i_hitbox.position.x + i_hitbox.size.x) - 1) / CELL_SIZE) - b));
					}
				}
			}
			else if (i_check_cells.end() != std::find(i_check_cells.begin(), i_check_cells.end(), Cell::Wall))
			{
				output[a - floor(i_hitbox.position.y / CELL_SIZE)] += static_cast<unsigned char>(pow(2, floor((ceil(i_hitbox.position.x + i_hitbox.size.x) - 1) / CELL_SIZE) - b));
			}
		}
	}

	return output;
}
//
sf::Color MapManager::get_map_sketch_pixel(const unsigned short i_x, const unsigned short i_y) const
{
	return map_sketch.getPixel({ static_cast<unsigned int>(i_x), static_cast<unsigned int>(i_y) });
}