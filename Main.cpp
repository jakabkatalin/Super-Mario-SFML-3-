#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <memory>
#include <algorithm>

#include "Global.hpp"
#include "MapManager.hpp"
#include "Mario.hpp"
#include "Enemy.hpp"
#include "ConvertSketch.hpp"
#include "TextManager.hpp"
// punctul de intrare si bucla principala a jocului
int main()
{
	//initilizez starea jocului 
	unsigned char current_level = 0;
	unsigned short level_finish = 0;
	short lives = 3;
	bool game_finished = false;

	std::vector<sf::Vector2f> killed_enemies;
	std::vector<sf::Vector2i> collected_coins; // Această listă conține acum și Brick/QuestionBlock distruse

	std::chrono::microseconds lag(0);
	std::chrono::steady_clock::time_point previous_time;
	std::vector<std::shared_ptr<Enemy>> enemies;
	sf::Color background_color = sf::Color(0, 219, 255);

	sf::RenderWindow window(sf::VideoMode({ static_cast<unsigned int>(SCREEN_RESIZE * SCREEN_WIDTH),
											static_cast<unsigned int>(SCREEN_RESIZE * SCREEN_HEIGHT) }),
		"Super Mario Bros", sf::Style::Close);

	window.setFramerateLimit(60);// Limităm la 60 FPS pentru a preveni rularea prea rapidă pe sisteme puternice
	sf::View view(sf::FloatRect({ 0.f, 0.f }, { static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT) }));// Setăm vizualizarea inițială
	sf::View ui_view(sf::FloatRect({ 0.f, 0.f }, { static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT) }));

	TextManager text_manager;
	MapManager map_manager;
	Mario mario;

	game_time = 0;// Resetează timpul jocului la începutul fiecărei sesiuni
	convert_sketch(current_level, level_finish, enemies, background_color, map_manager, mario, killed_enemies, collected_coins);
	previous_time = std::chrono::steady_clock::now();
	// bucla principală a jocului
	while (window.isOpen())
	{
		std::chrono::microseconds delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time);
		lag += delta_time;
		previous_time = std::chrono::steady_clock::now();

		while (FRAME_DURATION <= lag)// Actualizăm logica jocului la intervale fixe
		{
			lag -= FRAME_DURATION;

			if (!mario.get_dead() && !game_finished && lives > 0)
			{
				game_time += 0.016f;
			}

			while (const std::optional event = window.pollEvent())
			{
				if (event->is<sf::Event::Closed>()) window.close();

				if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
					if (keyPressed->code == sf::Keyboard::Key::R) {// Apasă R pentru a reporni jocul
						if (lives <= 0 || game_finished) {
							// --- RESTART COMPLET (GAME OVER) ---
							game_score = 0;
							game_coins = 0;
							game_time = 0;
							current_level = 0;
							lives = 3;
							game_finished = false;
							killed_enemies.clear();
							collected_coins.clear(); // Golește lista la Game Over
							enemies.clear();
							mario.reset(true);
							convert_sketch(current_level, level_finish, enemies, background_color, map_manager, mario, killed_enemies, collected_coins);
						}
						else if (mario.can_restart() && lives > 0) {
							// --- RESTART PARȚIAL (RESPAWN) ---
							lives--;
							if (lives > 0) {
								enemies.clear();
								mario.reset(false);
								// Reconstruim nivelul dar menținem blocurile distruse
								convert_sketch(current_level, level_finish, enemies, background_color, map_manager, mario, killed_enemies, collected_coins);
							}
						}
					}
				}
			}
			// Actualizăm logica jocului doar dacă nu este terminat și Mario are vieți rămase
			if (!game_finished && lives > 0) {
				if (mario.get_hit_box().position.y >= SCREEN_HEIGHT) {
					mario.die(true);
					if (lives <= 0) game_finished = false;
				}

				if (current_level == 1) {
					float finish_line_x = static_cast<float>((level_finish - 1) * CELL_SIZE);
					if (mario.get_x() >= finish_line_x) game_finished = true;
				}
				// Trecerea la nivelul următor
				if (!game_finished && current_level == 0 && mario.get_x() >= (level_finish - 1) * CELL_SIZE) {
					current_level++;
					killed_enemies.clear();
					collected_coins.clear(); // La trecerea la nivelul următor, de obicei se golește
					enemies.clear();
					mario.reset(true);
					convert_sketch(current_level, level_finish, enemies, background_color, map_manager, mario, killed_enemies, collected_coins);
				}
				// Actualizăm harta și pozițiile entităților
				map_manager.update();
				int mario_x = static_cast<int>(std::round(mario.get_x()));
				unsigned int view_x = std::clamp<int>(mario_x - 0.5f * (SCREEN_WIDTH - CELL_SIZE), 0, CELL_SIZE * map_manager.get_map_width() - SCREEN_WIDTH);

				mario.update(view_x, map_manager, collected_coins);
				// Actualizăm inamicii si curatăm cei eliminați
				for (auto it = enemies.begin(); it != enemies.end();) {
					(*it)->update(view_x, enemies, map_manager, mario);
					if ((*it)->get_dead(true)) {
						killed_enemies.push_back((*it)->get_start_position());
						it = enemies.erase(it);
					}
					else {
						++it;
					}
				}

				view.setCenter({ static_cast<float>(view_x) + SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f });
			}
		}
		// Desenăm scena
		window.clear(background_color);
		window.setView(view);
		unsigned int current_view_x = static_cast<unsigned int>(view.getCenter().x - SCREEN_WIDTH / 2.f);

		map_manager.draw_map(1, sf::Color(0, 0, 85) == background_color, current_view_x, window);
		mario.draw_mushrooms(current_view_x, window);
		map_manager.draw_map(0, sf::Color(0, 0, 85) == background_color, current_view_x, window);
		for (auto& enemy : enemies) enemy->draw(current_view_x, window);
		mario.draw(window);
		//interfata
		window.setView(ui_view);
		text_manager.draw_text(10, 8, "MARIO", window);
		text_manager.draw_text(10, 16, std::to_string(game_score), window);
		text_manager.draw_text(80, 8, "LIVES", window);
		text_manager.draw_text(95, 16, std::to_string(lives), window);

		std::string coin_text = (game_coins < 10 ? "0" : "") + std::to_string(game_coins);
		text_manager.draw_text(120, 16, "COIN X " + coin_text, window);

		text_manager.draw_text(200, 8, "WORLD", window);
		text_manager.draw_text(205, 16, std::to_string(current_level + 1) + (current_level == 1 ? "-2" : "-2"), window);

		text_manager.draw_text(265, 8, "TIME", window);
		text_manager.draw_text(265, 16, std::to_string(static_cast<int>(game_time)), window);

		if (game_finished) {
			text_manager.draw_text(80, 100, "FINISHED GAME AT", window);
			text_manager.draw_text(140, 115, std::to_string(static_cast<int>(game_time)) + " SECONDS", window);
			text_manager.draw_text(80, 140, "YOUR SCORE " + std::to_string(game_score), window);
			text_manager.draw_text(75, 170, "PRESS R TO RESTART", window);
		}
		else if (mario.get_dead() && lives > 0) {
			text_manager.draw_text(130, 100, "YOU DIED", window);
			text_manager.draw_text(85, 120, "PRESS R TO TRY AGAIN", window);
		}
		else if (lives <= 0) {
			text_manager.draw_text(130, 100, "GAME OVER", window);
			text_manager.draw_text(80, 120, "PRESS R TO RESTART ALL", window);
		}

		window.display();
	}

	return 0;
}