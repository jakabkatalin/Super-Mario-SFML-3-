#include <cmath>
#include <SFML/Graphics.hpp>
#include "DrawMap.hpp"
#include "Global.hpp"
// Desenează harta pe fereastra dată, folosind schița hărții și textura hărții
void draw_map(unsigned i_view_x, const sf::Image& i_map_sketch, sf::RenderWindow& i_window, const sf::Texture& i_map_texture, const Map& i_map)
{
	// Calculăm limitele de desenare pe baza poziției camerei
    unsigned short map_end = static_cast<unsigned short>(std::ceil((i_view_x + SCREEN_WIDTH) / static_cast<float>(CELL_SIZE)));
    unsigned short map_height = static_cast<unsigned short>(std::floor(i_map_sketch.getSize().y / 3.f));
    unsigned short map_start = static_cast<unsigned short>(std::floor(i_view_x / static_cast<float>(CELL_SIZE)));

    sf::Sprite cell_sprite(i_map_texture);

    for (unsigned short a = map_start; a < map_end; a++)
    {
        for (unsigned short b = 0; b < map_height; b++)
        {
            unsigned short sprite_x = 0;
            unsigned short sprite_y = 0;

			// sectiunea de fundal din schița hărții: norii
            sf::Color pixel = i_map_sketch.getPixel({ (unsigned int)a, (unsigned int)(b + 2 * map_height) });
            sf::Color pixel_down(0, 0, 0, 0), pixel_left(0, 0, 0, 0), pixel_right(0, 0, 0, 0), pixel_up(0, 0, 0, 0);

            cell_sprite.setPosition({ static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b) });

            if (255 == pixel.a)
            {
                // Vecini pentru tiling logic
                if (a > 0) pixel_left = i_map_sketch.getPixel({ (unsigned int)(a - 1), (unsigned int)(b + 2 * map_height) });
                if (b > 0) pixel_up = i_map_sketch.getPixel({ (unsigned int)a, (unsigned int)(b + 2 * map_height - 1) });
                if (a < i_map_sketch.getSize().x - 1) pixel_right = i_map_sketch.getPixel({ (unsigned int)(a + 1), (unsigned int)(b + 2 * map_height) });
                if (b < map_height - 1) pixel_down = i_map_sketch.getPixel({ (unsigned int)a, (unsigned int)(b + 2 * map_height + 1) });

                // Exemplu logică nori
                if (sf::Color(255, 255, 255) == pixel) {
                    sprite_x = 8;
                    if (sf::Color(255, 255, 255) == pixel_up) sprite_y = 1;
                    if (sf::Color(255, 255, 255) == pixel_left) {
                        if (sf::Color(255, 255, 255) != pixel_right) sprite_x = 9;
                    }
                    else if (sf::Color(255, 255, 255) == pixel_right) {
                        sprite_x = 7;
                    }
                }
               

                cell_sprite.setTextureRect(sf::IntRect(
                    { (int)(CELL_SIZE * sprite_x), (int)(CELL_SIZE * sprite_y) },
                    { (int)CELL_SIZE, (int)CELL_SIZE }
                ));
                i_window.draw(cell_sprite);
            }

            // Desenare Celule din Map
            if (Cell::Empty != i_map[a][b])
            {
				if (Cell::Pipe == i_map[a][b]) {// Logică țevilor
                    sprite_y = (Cell::Pipe == i_map[a][b - 1]) ? 1 : 0;
                    sprite_x = (Cell::Pipe == i_map[a - 1][b]) ? 11 : 10;
                }
                else if (Cell::QuestionBlock == i_map[a][b]) { sprite_x = 1; sprite_y = 0; }
                else if (Cell::Wall == i_map[a][b]) {
                    sprite_y = 0;
                    sprite_x = (sf::Color(0, 0, 0) == i_map_sketch.getPixel({ (unsigned int)a, (unsigned int)b })) ? 2 : 3;
                }

                cell_sprite.setTextureRect(sf::IntRect(
                    { (int)(CELL_SIZE * sprite_x), (int)(CELL_SIZE * sprite_y) },
                    { (int)CELL_SIZE, (int)CELL_SIZE }
                ));
                i_window.draw(cell_sprite);
            }
        }
    }
}