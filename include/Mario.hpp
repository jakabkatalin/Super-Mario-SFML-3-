#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <optional>
#include <vector>
#include <string>

#include "Animation.hpp"
#include "MapManager.hpp"
#include "Mushroom.hpp"

class Mario {
	bool crouching;
	bool dead;
	bool flipped;
	bool on_ground;

	float enemy_bounce_speed;
	float horizontal_speed;
	float vertical_speed;
	float x;
	float y;
	float respawn_x;
	float respawn_y;

	short jump_timer;
	unsigned char powerup_state;
	unsigned short death_timer;
	unsigned short growth_timer;
	unsigned short invincible_timer;

	std::optional<sf::Sprite> sprite;

	sf::SoundBuffer coin_buffer;
	std::optional<sf::Sound> coin_sound;

	sf::SoundBuffer death_buffer;
	std::optional<sf::Sound> death_sound;

	std::map<std::string, sf::Texture> texture_cache;

	// TEXTURA PENTRU CIUPERCI (Păstrată aici pentru stabilitate)
	sf::Texture mushroom_texture;

	Animation big_walk_animation;
	Animation walk_animation;

	std::vector<Mushroom> mushrooms;

public:
	Mario();

	bool can_restart() const;
	bool get_dead() const;

	float get_vertical_speed() const;
	float get_x() const;
	float get_y() const;

	void die(const bool i_instant_death);
	void draw(sf::RenderWindow& i_window);
	void draw_mushrooms(const unsigned i_view_x, sf::RenderWindow& i_window);
	void reset(bool i_full_reset);
	void set_position(const float i_x, const float i_y);
	void set_texture(std::string i_name);
	void set_vertical_speed(const float i_value);
	void update(const unsigned i_view_x, MapManager& i_map_manager, std::vector<sf::Vector2i>& i_collected_coins);

	sf::FloatRect get_hit_box() const;
};