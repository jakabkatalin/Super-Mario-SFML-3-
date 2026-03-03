#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include <cmath>
#include "Global.hpp"

class TextManager {
	// Stocăm reprezentarea binară a fiecărui caracter (matrice 5x5)
	std::map<char, std::vector<std::vector<int>>> font_data;

public:
	TextManager() {
		// Definim pixelii pentru cifre
		font_data['0'] = { {1,1,1,1,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,1,1,1,1} };
		font_data['1'] = { {0,0,1,0,0}, {0,1,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {1,1,1,1,1} };
		font_data['2'] = { {1,1,1,1,1}, {0,0,0,0,1}, {1,1,1,1,1}, {1,0,0,0,0}, {1,1,1,1,1} };
		font_data['3'] = { {1,1,1,1,1}, {0,0,0,0,1}, {0,1,1,1,1}, {0,0,0,0,1}, {1,1,1,1,1} };
		font_data['4'] = { {1,0,0,0,1}, {1,0,0,0,1}, {1,1,1,1,1}, {0,0,0,0,1}, {0,0,0,0,1} };
		font_data['5'] = { {1,1,1,1,1}, {1,0,0,0,0}, {1,1,1,1,1}, {0,0,0,0,1}, {1,1,1,1,1} };
		font_data['6'] = { {1,1,1,1,1}, {1,0,0,0,0}, {1,1,1,1,1}, {1,0,0,0,1}, {1,1,1,1,1} };
		font_data['7'] = { {1,1,1,1,1}, {0,0,0,0,1}, {0,0,0,1,0}, {0,0,1,0,0}, {0,1,0,0,0} };
		font_data['8'] = { {1,1,1,1,1}, {1,0,0,0,1}, {1,1,1,1,1}, {1,0,0,0,1}, {1,1,1,1,1} };
		font_data['9'] = { {1,1,1,1,1}, {1,0,0,0,1}, {1,1,1,1,1}, {0,0,0,0,1}, {1,1,1,1,1} };

		// Definim pixelii pentru litere
		font_data['M'] = { {1,0,0,0,1}, {1,1,0,1,1}, {1,0,1,0,1}, {1,0,0,0,1}, {1,0,0,0,1} };
		font_data['A'] = { {0,1,1,1,0}, {1,0,0,0,1}, {1,1,1,1,1}, {1,0,0,0,1}, {1,0,0,0,1} };
		font_data['R'] = { {1,1,1,1,0}, {1,0,0,0,1}, {1,1,1,1,0}, {1,0,1,0,0}, {1,0,0,1,0} };
		font_data['I'] = { {1,1,1,1,1}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {1,1,1,1,1} };
		font_data['O'] = { {0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,0} };
		font_data['L'] = { {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,1,1,1,1} };
		font_data['V'] = { {1,0,0,0,1}, {1,0,0,0,1}, {0,1,0,1,0}, {0,1,0,1,0}, {0,0,1,0,0} };
		font_data['E'] = { {1,1,1,1,1}, {1,0,0,0,0}, {1,1,1,1,1}, {1,0,0,0,0}, {1,1,1,1,1} };
		font_data['S'] = { {0,1,1,1,1}, {1,0,0,0,0}, {0,1,1,1,0}, {0,0,0,0,1}, {1,1,1,1,0} };
		font_data['W'] = { {1,0,0,0,1}, {1,0,0,0,1}, {1,0,1,0,1}, {1,1,0,1,1}, {1,0,0,0,1} };
		font_data['D'] = { {1,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,1,1,1,0} };
		font_data['T'] = { {1,1,1,1,1}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0} };
		font_data['Y'] = { {1,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,0}, {0,0,1,0,0}, {0,0,1,0,0} };
		font_data['U'] = { {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,0} };
		font_data['X'] = { {1,0,0,0,1}, {0,1,0,1,0}, {0,0,1,0,0}, {0,1,0,1,0}, {1,0,0,0,1} };
		font_data['C'] = { {0,1,1,1,1}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {0,1,1,1,1} };
		font_data['N'] = { {1,0,0,0,1}, {1,1,0,0,1}, {1,0,1,0,1}, {1,0,0,1,1}, {1,0,0,0,1} };
		font_data['F'] = { {1,1,1,1,1}, {1,0,0,0,0}, {1,1,1,1,0}, {1,0,0,0,0}, {1,0,0,0,0} };
		font_data['H'] = { {1,0,0,0,1}, {1,0,0,0,1}, {1,1,1,1,1}, {1,0,0,0,1}, {1,0,0,0,1} };
		font_data['G'] = { {0,1,1,1,1}, {1,0,0,0,0}, {1,0,1,1,1}, {1,0,0,0,1}, {0,1,1,1,1} };

		// Simboluri speciale
		font_data['*'] = { {0,0,1,0,0}, {0,1,1,1,0}, {1,1,1,1,1}, {0,1,1,1,0}, {0,0,1,0,0} };
		font_data['-'] = { {0,0,0,0,0}, {0,0,0,0,0}, {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} };
	}

	// Metoda care desenează textul pe ecran caracter cu caracter
	void draw_text(float i_x, float i_y, std::string i_string, sf::RenderWindow& i_window) {
		float current_x = i_x;
		float current_y = i_y;

		for (char c : i_string) {
			// Convertim automat în majuscule deoarece am definit doar litere mari
			c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));

			if (c == ' ') {
				current_x += 6; // Lățimea unui spațiu
				continue;
			}

			if (font_data.count(c)) {
				auto& grid = font_data[c];
				// Folosim un Shape de 1x1 pixel pentru a desena fontul manual
				sf::RectangleShape pixel(sf::Vector2f(1.0f, 1.0f));
				pixel.setFillColor(sf::Color::White);

				for (int row = 0; row < 5; ++row) {
					for (int col = 0; col < 5; ++col) {
						if (grid[row][col] == 1) {
							pixel.setPosition({ current_x + static_cast<float>(col), current_y + static_cast<float>(row) });
							i_window.draw(pixel);
						}
					}
				}
			}
			// Trecem la următorul caracter (5 pixeli lățime + 2 pixeli spațiu)
			current_x += 7;
		}
	}
};