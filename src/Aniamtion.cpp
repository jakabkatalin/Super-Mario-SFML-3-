#include "Animation.hpp"
#include <algorithm>
#include <SFML/Graphics.hpp>
//frame by frame mapping: sincronizarea temporala(contor intern decide exact cadn trebuie schimbat cadrul), gestionarea atlasului de texturi, modularitatea
//tot ce se misca vizual, decupeaza cadre dintr-o textură mare
// Constructorul clasei Animation: inițializează animația cu lățimea cadrului, locația texturii și viteza animației
Animation::Animation(const unsigned short i_frame_width, const std::string& i_texture_location, const unsigned short i_animation_speed) :
    flipped(false),
    animation_iterator(0),
	//ne asigurăm că viteza animației este cel puțin 1
    animation_speed(std::max<unsigned short>(1, i_animation_speed)),
    current_frame(0),
    frame_width(i_frame_width),
    sprite(texture)
{
	//încarcăm textura și calculăm numărul total de cadre
    if (texture.loadFromFile(i_texture_location)) {
		total_frames = static_cast<unsigned short>(texture.getSize().x / frame_width);//!!! constructor total_frames initializarea valida a objectului 
    }
}
//am abstractizat procesul de miscare a imaginii intr-o singura entitate usor de folosit
void Animation::draw(sf::RenderWindow& i_window) {
    if (!flipped) {
		//setăm dreptunghiul texturii pentru cadrul curent
        sprite.setTextureRect(sf::IntRect(
			{ static_cast<int>(current_frame * frame_width), 0 },//x-ul se muta la fiecare cadru
            { static_cast<int>(frame_width), static_cast<int>(texture.getSize().y) }//dim cadru
        ));
    }
    else {
		//setăm dreptunghiul texturii pentru cadrul curent, inversat pe orizontală
        sprite.setTextureRect(sf::IntRect(
            { static_cast<int>(frame_width * (1 + current_frame)), 0 },
            { -static_cast<int>(frame_width), static_cast<int>(texture.getSize().y) }
        ));
    }

    i_window.draw(sprite);
}

void Animation::update() {
	//actualizăm cadrul curent în funcție de viteza animației
    while (animation_iterator >= animation_speed) {
        animation_iterator -= animation_speed;
		//modulo asigură repetarea cadrelor (0, 1, 2, 0, 1, 2...)
        current_frame = (1 + current_frame) % total_frames;
    }
    animation_iterator++;
}
// setează dacă animația este oglindită pe orizontală
void Animation::set_flipped(const bool i_value) { flipped = i_value; }

// setează poziția sprite-ului în fereastră
void Animation::set_position(const short i_x, const short i_y) {
    sprite.setPosition({ static_cast<float>(i_x), static_cast<float>(i_y) });
}
// setează locația texturii și recalculăm numărul total de cadre
void Animation::set_texture_location(const std::string& i_texture_location) {
    if (texture.loadFromFile(i_texture_location)) {
        total_frames = static_cast<unsigned short>(texture.getSize().x / frame_width);
    }
}
// setează viteza animației, asigurându-se că este cel puțin 1
void Animation::set_animation_speed(const unsigned short i_animation_speed) {
    animation_speed = std::max<unsigned short>(1, i_animation_speed);
};