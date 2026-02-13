#include <array>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Animation.hpp"
#include "Global.hpp"
#include "MapManager.hpp"
#include "Mushroom.hpp"
#include "Mario.hpp"
// Constructor Mario: Inițializează stările, încarcă sunetele și cache-ul de texturi
Mario::Mario() :
	crouching(false),
	dead(false),
	flipped(false),
	on_ground(false),
	enemy_bounce_speed(0),
	horizontal_speed(0),
	vertical_speed(0),
	x(0),
	y(0),
	respawn_x(0),
	respawn_y(0),
	jump_timer(0),
	powerup_state(0),
	death_timer(MARIO_DEATH_DURATION),
	growth_timer(0),
	invincible_timer(0),
	big_walk_animation(CELL_SIZE, "BigMarioWalk.png", MARIO_WALK_ANIMATION_SPEED),
	walk_animation(CELL_SIZE, "MarioWalk.png", MARIO_WALK_ANIMATION_SPEED)
{
	//sunetele
	if (coin_buffer.loadFromFile("coin.wav")) {
		coin_sound.emplace(coin_buffer);
	}

	if (death_buffer.loadFromFile("death.wav")) {
		death_sound.emplace(death_buffer);
	}

	mushroom_texture.loadFromFile("Mushroom.png");
	//preîncărcarea texturilor
	std::vector<std::string> textures = {
		"MarioIdle.png", "MarioJump.png", "MarioBrake.png", "MarioDeath.png",
		"BigMarioIdle.png", "BigMarioJump.png", "BigMarioBrake.png", "BigMarioCrouch.png", "BigMarioDeath.png"
	};

	for (const std::string& name : textures) {
		sf::Texture temp;
		if (temp.loadFromFile(name)) {
			texture_cache[name] = std::move(temp);
		}
	}

	if (texture_cache.count("MarioIdle.png")) {
		sprite.emplace(texture_cache["MarioIdle.png"]);
	}
}
//schimbă textura sprite-ului 
void Mario::set_texture(std::string i_name) {
	if (sprite.has_value() && texture_cache.count(i_name)) {
		sprite->setTexture(texture_cache[i_name]);
	}
}
//getteri pt mario
bool Mario::get_dead() const { return dead; }
float Mario::get_vertical_speed() const { return vertical_speed; }
float Mario::get_x() const { return x; }
float Mario::get_y() const { return y; }
//verifică dacă Mario poate reporni după moarte
bool Mario::can_restart() const {
	return dead && (death_timer == 0);
}
//gestionează moartea lui Mario sau pierderea unui power-up
void Mario::die(const bool i_instant_death) {
	if (!dead) {
		if (i_instant_death) {
			dead = true;
			set_texture(powerup_state == 0 ? "MarioDeath.png" : "BigMarioDeath.png");
			if (death_sound.has_value()) {
				death_sound->play();
			}
		}
		else if (growth_timer == 0 && invincible_timer == 0) {
			if (powerup_state == 0) {
				dead = true;
				set_texture("MarioDeath.png");
				if (death_sound.has_value()) {
					death_sound->play();
				}
			}
			else {
				//daca are power-up, îl pierde
				powerup_state = 0;
				invincible_timer = MARIO_INVINCIBILITY_DURATION;
				if (!crouching) y += CELL_SIZE;
				else crouching = false;
			}
		}
	}
}
//resetează starea lui Mario
void Mario::reset(bool i_full_reset) {
	if (death_sound.has_value()) {
		death_sound->stop();
	}

	crouching = false; dead = false; flipped = false; on_ground = false;
	enemy_bounce_speed = 0; horizontal_speed = 0; vertical_speed = 0;
	jump_timer = 0; powerup_state = 0; death_timer = MARIO_DEATH_DURATION;
	growth_timer = 0; invincible_timer = 0;
	mushrooms.clear();
	set_texture("MarioIdle.png");
	if (i_full_reset) { x = 0; y = 0; respawn_x = 0; respawn_y = 0; }
	else { x = std::max(0.0f, respawn_x - CELL_SIZE); y = respawn_y; }
}
//desenează sprite-ul lui Mario în fereastră
void Mario::draw(sf::RenderWindow& i_window) {
	if (!sprite.has_value()) return;
	//efect de clipire când este invincibil
	if (0 == (invincible_timer / MARIO_BLINKING) % 2) {
		bool draw_sprite = true;
		bool draw_big = (growth_timer == 0) ? (powerup_state > 0) : (0 == (growth_timer / MARIO_BLINKING) % 2);

		if (!dead) {
			if (draw_big) {
				//logica pentru desenarea lui Big Mario
				if (crouching) set_texture("BigMarioCrouch.png");
				else if (!on_ground) set_texture("BigMarioJump.png");
				else {
					if (horizontal_speed == 0) set_texture("BigMarioIdle.png");
					else if ((horizontal_speed > 0 && (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))) ||
						(horizontal_speed < 0 && (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)))) {
						set_texture("BigMarioBrake.png");
					}
					else {
						draw_sprite = false;
						big_walk_animation.set_flipped(flipped);
						big_walk_animation.set_position(static_cast<short>(std::round(x)), static_cast<short>(std::round(y)));
						big_walk_animation.draw(i_window);
					}
				}
			}
			else {
				//logica pentru desenarea lui Small Mario
				float offset_y = (growth_timer > 0 || powerup_state > 0) ? (float)CELL_SIZE : 0.0f;
				if (!on_ground) set_texture("MarioJump.png");
				else {
					if (horizontal_speed == 0) set_texture("MarioIdle.png");
					else if ((horizontal_speed > 0 && (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))) ||
						(horizontal_speed < 0 && (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)))) {
						set_texture("MarioBrake.png");
					}
					else {
						draw_sprite = false;
						walk_animation.set_flipped(flipped);
						walk_animation.set_position(static_cast<short>(std::round(x)), static_cast<short>(std::round(y + offset_y)));
						walk_animation.draw(i_window);
					}
				}
			}
		}
		else set_texture("MarioDeath.png");

		if (draw_sprite) {
			const sf::Texture& current_tex = sprite->getTexture();
			float offset_y = (!dead && !draw_big && (growth_timer > 0 || powerup_state > 0)) ? (float)CELL_SIZE : 0.0f;
			sprite->setPosition({ (float)std::round(x), (float)std::round(y) + offset_y });
			int w = static_cast<int>(current_tex.getSize().x);
			int h = static_cast<int>(current_tex.getSize().y);
			sprite->setTextureRect(flipped ? sf::IntRect({ w, 0 }, { -w, h }) : sf::IntRect({ 0, 0 }, { w, h }));
			i_window.draw(*sprite);
		}
	}
}
//desenează ciupercile în fereastră
void Mario::draw_mushrooms(const unsigned i_view_x, sf::RenderWindow& i_window) {
	for (Mushroom& mushroom : mushrooms) mushroom.draw(i_view_x, i_window);
}
//setează poziția lui Mario și punctul de respawn
void Mario::set_position(const float i_x, const float i_y) {
	x = i_x; y = i_y; respawn_x = i_x; respawn_y = i_y;
}
//setează viteza verticală a lui Mario (folosită la sărituri peste inamici)
void Mario::set_vertical_speed(const float i_value) { enemy_bounce_speed = i_value; }
//actualizează starea lui Mario în funcție de input și coliziuni
void Mario::update(const unsigned i_view_x, MapManager& i_map_manager, std::vector<sf::Vector2i>& i_collected_items) {
	if (0 != enemy_bounce_speed) { vertical_speed = enemy_bounce_speed; enemy_bounce_speed = 0; }

	for (Mushroom& mushroom : mushrooms) mushroom.update(i_view_x, i_map_manager);

	if (!dead) {
		bool moving = false;
		std::vector<unsigned char> collision;
		std::vector<sf::Vector2i> cells;
		sf::FloatRect hit_box = get_hit_box();

		on_ground = false;

		// Gestionare Monede (deja existent)
		i_map_manager.map_collision({ Cell::Coin }, cells, hit_box);
		for (const auto& cell : cells) {
			i_map_manager.set_map_cell(cell.x, cell.y, Cell::Empty);
			i_collected_items.push_back(cell);
			game_coins++;
			game_score += 100;
			if (coin_sound.has_value()) coin_sound->play();
		}
		cells.clear();
		//input de mișcare orizontală
		if (!crouching) {
			if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) &&
				!(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))) {
				moving = true; horizontal_speed = std::max(horizontal_speed - MARIO_ACCELERATION, -MARIO_WALK_SPEED);
			}
			if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) &&
				!(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))) {
				moving = true; horizontal_speed = std::min(MARIO_ACCELERATION + horizontal_speed, MARIO_WALK_SPEED);
			}
		}
		//frânare când nu se mișcă
		if (!moving) {
			if (horizontal_speed > 0) horizontal_speed = std::max<float>(0, horizontal_speed - MARIO_ACCELERATION);
			else if (horizontal_speed < 0) horizontal_speed = std::min<float>(0, MARIO_ACCELERATION + horizontal_speed);
		}
		//gestionare ghemuit
		if (powerup_state > 0) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
				if (!crouching) { crouching = true; y += CELL_SIZE; }
			}
			else if (crouching) {
				//verificăm dacă poate să se ridice
				sf::FloatRect crouch_box = get_hit_box();
				crouch_box.size.y += (float)CELL_SIZE;
				crouch_box.position.y -= (float)CELL_SIZE;
				collision = i_map_manager.map_collision({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::QuestionBlock, Cell::Wall }, crouch_box, i_collected_items);
				if (std::all_of(collision.begin(), collision.end(), [](unsigned char v) { return v == 0; })) {
					crouching = false; y -= CELL_SIZE;
				}
			}
		}

		hit_box = get_hit_box();
		hit_box.position.x += horizontal_speed;
		collision = i_map_manager.map_collision({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::QuestionBlock, Cell::Wall }, hit_box, i_collected_items);
		//sarituri și coliziuni orizontale
		if (!std::all_of(collision.begin(), collision.end(), [](unsigned char v) { return v == 0; })) {
			if (horizontal_speed > 0) x = (float)CELL_SIZE * (std::ceil((horizontal_speed + x) / CELL_SIZE) - 1);
			else if (horizontal_speed < 0) x = (float)CELL_SIZE * (1 + std::floor((horizontal_speed + x) / CELL_SIZE));
			horizontal_speed = 0;
		}
		else x += horizontal_speed;
		//coliziuni verticale
		hit_box = get_hit_box();
		hit_box.position.y += 1.0f;
		collision = i_map_manager.map_collision({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::QuestionBlock, Cell::Wall }, hit_box, i_collected_items);
		//verificare dacă este pe sol
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
			if (vertical_speed == 0 && !std::all_of(collision.begin(), collision.end(), [](unsigned char v) { return v == 0; })) {
				vertical_speed = MARIO_JUMP_SPEED; jump_timer = MARIO_JUMP_TIMER;
			}
			else if (jump_timer > 0) {
				vertical_speed = MARIO_JUMP_SPEED; jump_timer--;
			}
			else vertical_speed = std::min(GRAVITY + vertical_speed, MAX_VERTICAL_SPEED);
		}
		else {
			vertical_speed = std::min(GRAVITY + vertical_speed, MAX_VERTICAL_SPEED); jump_timer = 0;
		}

		hit_box = get_hit_box();
		hit_box.position.y += vertical_speed;
		collision = i_map_manager.map_collision({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::QuestionBlock, Cell::Wall }, hit_box, i_collected_items);

		if (!std::all_of(collision.begin(), collision.end(), [](unsigned char v) { return v == 0; })) {
			if (vertical_speed < 0) {
				// Eliminare Fizică Brick-uri (Mario Mare)
				if (!crouching && powerup_state > 0) {
					i_map_manager.map_collision({ Cell::Brick }, cells, hit_box);
					for (const auto& cell : cells) {
						i_map_manager.set_map_cell(cell.x, cell.y, Cell::Empty);
						// Îl adăugăm în lista de colectate pentru a NU mai apărea la restart
						i_collected_items.push_back(cell);
						i_map_manager.add_brick_particles((float)CELL_SIZE * cell.x, (float)CELL_SIZE * cell.y);
						game_score += 250;
					}
					cells.clear();
				}

				// Eliminare Fizică Question Blocks
				i_map_manager.map_collision({ Cell::QuestionBlock }, cells, hit_box);
				for (const auto& cell : cells) {
					sf::Color sketch_pixel = i_map_manager.get_map_sketch_pixel(cell.x, cell.y);

					// În loc de ActivatedQuestionBlock, îl facem Empty ca să dispară coliziunea
					i_map_manager.set_map_cell(cell.x, cell.y, Cell::Empty);
					// Îl adăugăm în listă pentru persistență
					i_collected_items.push_back(cell);

					if (sf::Color(255, 73, 85) == sketch_pixel) {
						mushrooms.push_back(Mushroom(static_cast<float>(CELL_SIZE * cell.x), static_cast<float>(CELL_SIZE * cell.y), mushroom_texture));
					}
					else {
						i_map_manager.add_question_block_coin(CELL_SIZE * cell.x, CELL_SIZE * cell.y);
						game_coins++;
						game_score += 100;
						if (coin_sound.has_value()) coin_sound->play();
					}
				}
				cells.clear();
				y = (float)CELL_SIZE * (1 + std::floor((vertical_speed + y) / CELL_SIZE));
			}
			else if (vertical_speed > 0) {
				y = (float)CELL_SIZE * (std::ceil((vertical_speed + y) / CELL_SIZE) - 1);
			}
			jump_timer = 0; vertical_speed = 0;
		}
		else y += vertical_speed;

		if (horizontal_speed != 0) flipped = (horizontal_speed < 0);

		hit_box = get_hit_box();
		hit_box.position.y += 1.0f;
		collision = i_map_manager.map_collision({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::QuestionBlock, Cell::Wall }, hit_box, i_collected_items);

		if (!std::all_of(collision.begin(), collision.end(), [](unsigned char v) { return v == 0; })) {
			on_ground = true;
			if (y < SCREEN_HEIGHT - CELL_SIZE * 2) { respawn_x = x; respawn_y = y; }
		}
		//coliziuni cu ciupercile power-up
		for (Mushroom& mushroom : mushrooms) {
			if (get_hit_box().findIntersection(mushroom.get_hit_box()).has_value()) {
				mushroom.set_dead(true);
				game_score += 1000;
				if (powerup_state == 0) {
					powerup_state = 1;
					growth_timer = MARIO_GROWTH_DURATION;
					y -= (float)CELL_SIZE;
				}
			}
		}

		if (invincible_timer > 0) invincible_timer--;
		if (growth_timer > 0) growth_timer--;

		if (y >= SCREEN_HEIGHT) die(true);

		if (powerup_state == 0) walk_animation.update();
		else big_walk_animation.update();
	}
	else {
		//death animation
		if (death_timer == 0) {
			vertical_speed = std::min(GRAVITY + vertical_speed, MAX_VERTICAL_SPEED);
			y += vertical_speed;
		}
		else if (death_timer == 1) vertical_speed = MARIO_JUMP_SPEED;
		death_timer = std::max(0, (int)death_timer - 1);
	}

	mushrooms.erase(std::remove_if(mushrooms.begin(), mushrooms.end(), [](const Mushroom& m) { return m.get_dead(); }), mushrooms.end());
}
//returnează hit box-ul lui Mario în funcție de starea sa (ghemuit sau mare)
sf::FloatRect Mario::get_hit_box() const {
	if (crouching || (powerup_state == 0 && growth_timer == 0)) return sf::FloatRect({ x, y }, { (float)CELL_SIZE, (float)CELL_SIZE });
	return sf::FloatRect({ x, y }, { (float)CELL_SIZE, (float)(2 * CELL_SIZE) });
}