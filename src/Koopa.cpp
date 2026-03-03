#include <array>
#include <cmath>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include "Global.hpp"
#include "Koopa.hpp"
//mai complex decat goomba dar tot la fel merge, are mai multe starii 
Koopa::Koopa(const bool i_underground, const float i_x, const float i_y) :
	Enemy(i_x, i_y),
	check_collision(true),
	flipped(true),
	no_collision_dying(false),
	underground(i_underground),
	state(0),
	get_out_timer(KOOPA_GET_OUT_DURATION),
	get_out_animation(CELL_SIZE, "KoopaGetOut.png", KOOPA_GET_OUT_ANIMATION_SPEED),
	walk_animation(CELL_SIZE, "KoopaWalk.png", KOOPA_WALK_ANIMATION_SPEED),
	sprite(texture)
{
	horizontal_speed = -KOOPA_SPEED;

	if (0 == underground) {
		texture.loadFromFile("KoopaShell.png");
	}
	else {
		texture.loadFromFile("UndergroundKoopaShell.png");
		get_out_animation.set_texture_location("UndergroundKoopaGetOut.png");
		walk_animation.set_texture_location("UndergroundKoopaWalk.png");
	}
}

bool Koopa::get_dead(const bool i_deletion) const {
	return (1 == i_deletion) ? dead : (dead || no_collision_dying);
}

void Koopa::die(const unsigned char i_death_type) {
	if (0 == i_death_type) {
		dead = 1;
	}
	else if (2 == i_death_type) {
		no_collision_dying = 1;
		vertical_speed = 0.5f * MARIO_JUMP_SPEED;
		texture.loadFromFile(underground ? "UndergroundKoopaDeath.png" : "KoopaDeath.png");
	}
}

void Koopa::draw(const unsigned i_view_x, sf::RenderWindow& i_window) {
	if (-CELL_SIZE < std::round(y) && std::round(x) >(int)i_view_x - CELL_SIZE && std::round(x) < SCREEN_WIDTH + i_view_x && std::round(y) < SCREEN_HEIGHT) {
		if (0 < state || 1 == no_collision_dying) {
			if (0 == get_dead(0) && 1 == state && get_out_timer <= 0.25f * KOOPA_GET_OUT_DURATION) {
				get_out_animation.set_position(static_cast<short>(std::round(x)), static_cast<short>(std::round(y)));
				get_out_animation.draw(i_window);
			}
			else {
				sprite.setPosition({ static_cast<float>(std::round(x)), static_cast<float>(std::round(y)) });
				sprite.setTexture(texture);
				i_window.draw(sprite);
			}
		}
		else {
			walk_animation.set_flipped(flipped);
			walk_animation.set_position(static_cast<short>(std::round(x)), static_cast<short>(std::round(y)));
			walk_animation.draw(i_window);
		}
	}
}

void Koopa::update(const unsigned i_view_x, const std::vector<std::shared_ptr<Enemy>>& i_enemies, const MapManager& i_map_manager, Mario& i_mario) {
	if (-CELL_SIZE < y && x >= (int)i_view_x - CELL_SIZE - ENTITY_UPDATE_AREA && x < ENTITY_UPDATE_AREA + SCREEN_WIDTH + i_view_x && y < SCREEN_HEIGHT) {

		vertical_speed = std::min(GRAVITY + vertical_speed, MAX_VERTICAL_SPEED);

		sf::FloatRect hit_box = get_hit_box();
		hit_box.position.y += vertical_speed;

		std::vector<sf::Vector2i> dummy_coins;
		auto collision = i_map_manager.map_collision({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall }, hit_box, dummy_coins);

		if (std::all_of(collision.begin(), collision.end(), [](unsigned char v) { return v == 0; })) {
			bool changed = false;
			for (auto& enemy : i_enemies) {
				if (shared_from_this() != enemy && !enemy->get_dead(0) && hit_box.findIntersection(enemy->get_hit_box())) {
					changed = true;
					y = (vertical_speed < 0) ? (enemy->get_hit_box().position.y + enemy->get_hit_box().size.y) : (enemy->get_hit_box().position.y - CELL_SIZE);
					vertical_speed = 0;
					break;
				}
			}
			if (!changed) y += vertical_speed;
		}
		else {
			y = (vertical_speed < 0) ? (CELL_SIZE * (1 + std::floor((vertical_speed + y) / CELL_SIZE))) : (CELL_SIZE * (std::ceil((vertical_speed + y) / CELL_SIZE) - 1));
			vertical_speed = 0;
		}

		hit_box = get_hit_box();
		hit_box.position.x += horizontal_speed;
		std::vector<sf::Vector2i> dummy_cells;
		collision = i_map_manager.map_collision({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall }, dummy_cells, hit_box);

		if (!no_collision_dying && std::all_of(collision.begin(), collision.end(), [](unsigned char v) { return v == 0; })) {
			bool changed = false;
			for (auto& enemy : i_enemies) {
				if (shared_from_this() != enemy && !enemy->get_dead(0) && hit_box.findIntersection(enemy->get_hit_box())) {
					if (0 == state) {
						changed = true;
						horizontal_speed *= -1;
					}
					else if (2 == state) {
						enemy->die(2);
					}
					break;
				}
			}
			if (!changed) x += horizontal_speed;
		}
		else {
			x = (horizontal_speed > 0) ? (CELL_SIZE * (std::ceil((horizontal_speed + x) / CELL_SIZE) - 1)) : (CELL_SIZE * (1 + std::floor((horizontal_speed + x) / CELL_SIZE)));
			horizontal_speed *= -1;
		}

		if (!i_mario.get_dead() && get_hit_box().findIntersection(i_mario.get_hit_box())) {
			if (check_collision) {
				if (0 == state) {
					if (i_mario.get_vertical_speed() > 0) {
						check_collision = 0; horizontal_speed = 0; state = 1;
						get_out_timer = KOOPA_GET_OUT_DURATION;
						i_mario.set_vertical_speed(0.5f * MARIO_JUMP_SPEED);
						game_score += 200; // PUNCTE KOOPA UCIS
					}
					else i_mario.die(0);
				}
				else if (1 == state) {
					check_collision = 0; state = 2;
					horizontal_speed = (x < i_mario.get_x()) ? -KOOPA_SHELL_SPEED : KOOPA_SHELL_SPEED;
				}
				else if (2 == state) {
					if (i_mario.get_vertical_speed() > 0) {
						check_collision = 0; horizontal_speed = 0; state = 1;
						get_out_timer = KOOPA_GET_OUT_DURATION;
						i_mario.set_vertical_speed(0.5f * MARIO_JUMP_SPEED);
					}
					else if ((horizontal_speed > 0 && x < i_mario.get_x()) || (horizontal_speed < 0 && x > i_mario.get_x())) {
						i_mario.die(0);
					}
				}
			}
		}
		else check_collision = 1;

		if (horizontal_speed > 0) flipped = 0; else if (horizontal_speed < 0) flipped = 1;

		if (1 == state) {
			if (get_out_timer > 0) {
				get_out_timer--;
				get_out_animation.update();
			}
			else {
				state = 0;
				horizontal_speed = flipped ? -KOOPA_SPEED : KOOPA_SPEED;
			}
		}
		walk_animation.update();
	}
	if (SCREEN_HEIGHT <= y) die(0);
}