#include <cmath>
#include <algorithm>
#include <SFML/Graphics.hpp>

#include "Global.hpp"
#include "MapManager.hpp"
#include "Mushroom.hpp"
//starii initaile si textura ciupercii
Mushroom::Mushroom(float i_x, float i_y, const sf::Texture& i_texture) :
	dead(false),
	horizontal_direction(1),
	vertical_speed(0),
	x(i_x),
	y(i_y),
	start_y(i_y),
	sprite(i_texture)
{
	// Inițializare sprite -definim porțiunea din textură corespunzătoare ciupercii
	sprite.setTextureRect(sf::IntRect({ 0, 0 }, { static_cast<int>(CELL_SIZE), static_cast<int>(CELL_SIZE) }));
}
// reurunește dacă ciuperca este moartă
bool Mushroom::get_dead() const {
	return dead;
}                                             //Pentru încapsulare. Protejez starea internă a obiectului. 
//setează starea de moarte a ciupercii
void Mushroom::set_dead(bool i_value) {
	dead = i_value;
}
//desenează ciuperca pe ecran dacă este în zona vizibilă
void Mushroom::draw(unsigned i_view_x, sf::RenderWindow& i_window) {
	if (x >= static_cast<float>(i_view_x) - CELL_SIZE && x <= static_cast<float>(i_view_x + SCREEN_WIDTH)) {
		// round() este vital pentru a elimina lag-ul vizual ("jitter")
		sprite.setPosition({ std::round(x), std::round(y) });
		i_window.draw(sprite);
	}
}
// Actualizează poziția și starea ciupercii
void Mushroom::update(unsigned i_view_x, const MapManager& i_map_manager) {
	if (dead) return;

	// Zona de activare a entității (doar când este aproape de marginea ecran)
	if (x >= static_cast<int>(i_view_x) - CELL_SIZE - ENTITY_UPDATE_AREA && x < static_cast<int>(i_view_x) + SCREEN_WIDTH + ENTITY_UPDATE_AREA) {

		// FAZA DE IEȘIRE (RISING)
		if (y > start_y - CELL_SIZE) {
			y -= 0.25f; //viteza mica de ridicare
			if (y <= start_y - CELL_SIZE) {
				y = start_y - CELL_SIZE;
			}
		}
		else {
			// LOGICA DE MIȘCARE ȘI CĂDERE 
			std::vector<sf::Vector2i> dummy_cells;
			std::vector<Cell> collision_types = { Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall };

			// --- GRAVITAȚIE ---
			vertical_speed = std::min(GRAVITY + vertical_speed, MAX_VERTICAL_SPEED);

			// --- VERIFICARE VERTICALĂ (CĂDERE) ---
			// Folosim un hitbox "inset" - mai îngust cu 2 pixeli pe fiecare parte
			// Acest padding de 2px previne blocarea pe colțurile blocurilor
			sf::FloatRect vertical_hitbox(
				{ x + 2.0f, y + vertical_speed },
				{ static_cast<float>(CELL_SIZE) - 4.0f, static_cast<float>(CELL_SIZE) }
			);

			auto collision_v = i_map_manager.map_collision(collision_types, vertical_hitbox, dummy_cells);

			if (std::all_of(collision_v.begin(), collision_v.end(), [](unsigned char v) { return v == 0; })) {
				// Dacă e aer sub acest hitbox îngust, CADE IMEDIAT DAR NU PREA CADE :b
				y += vertical_speed;
			}
			else {
				// Dacă e sol, snapping matematic precis (ca la Goomba)
				if (vertical_speed > 0) {
					y = CELL_SIZE * std::floor((y + vertical_speed) / CELL_SIZE);
				}
				else if (vertical_speed < 0) {
					y = CELL_SIZE * (std::floor(y / CELL_SIZE) + 1);
				}
				vertical_speed = 0;
			}

			// --- VERIFICARE ORIZONTALĂ (MERS) ---
			float h_speed = horizontal_direction * MUSHROOM_SPEED;

			// Hitbox orizontal micșorat pe înălțime (cu 1px sus/jos)
			// Previne detectarea podelei ca fiind un perete în față
			sf::FloatRect horizontal_hitbox(
				{ x + h_speed, y + 1.0f },
				{ static_cast<float>(CELL_SIZE), static_cast<float>(CELL_SIZE) - 2.0f }
			);

			auto collision_h = i_map_manager.map_collision(collision_types, horizontal_hitbox, dummy_cells);

			if (std::all_of(collision_h.begin(), collision_h.end(), [](unsigned char v) { return v == 0; })) {
				x += h_speed;
			}
			else {
				// Coliziune perete: inversăm direcția și aliniem la grilă
				if (horizontal_direction > 0) {
					x = CELL_SIZE * std::floor(x / CELL_SIZE);
				}
				else {
					x = CELL_SIZE * std::ceil(x / CELL_SIZE);
				}
				horizontal_direction *= -1;
			}
		}
	}

	// Moartea în prăpastie
	if (y >= SCREEN_HEIGHT) {
		dead = true;
	}
}
// Returnează aria de coliziune a ciupercii
sf::FloatRect Mushroom::get_hit_box() const {
	// Folosim noul format FloatRect din SFML 3
	return sf::FloatRect({ x, y }, { static_cast<float>(CELL_SIZE), static_cast<float>(CELL_SIZE) });
}