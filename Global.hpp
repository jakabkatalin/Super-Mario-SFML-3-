#pragma once
#include <array>
#include <chrono>
#include <vector>
//DEFINIM REGULI UNIVERSALE DIN PROIECT, CONSTEXPR CALCULATE DIRECT LA COMPILARE
//dictionarul de reguli al jocului , contine toate nr fixe (viteza mario, gravitatia, dim celule) folosite in tot proiectul
//VARIABILE ȘI CONSTANTE GLOBALE PENTRU STAREA JOCULUI	
inline unsigned int game_score = 0;
inline unsigned short game_coins = 0;
inline float game_time = 400.0f;
//constante pentru vitezele diferitelor obiecte din joc
constexpr float BRICK_PARTICLE_SPEED = 4;
//A coin speed jumping out of the question block.
constexpr float COIN_JUMP_SPEED = -6;
constexpr float GOOMBA_SPEED = 1;
constexpr float GRAVITY = 0.25f; //forta care trage toate obiectele în jos
constexpr float KOOPA_SHELL_SPEED = 4;
constexpr float KOOPA_SPEED = 1;
constexpr float MARIO_ACCELERATION = 0.25f;
constexpr float MARIO_JUMP_SPEED = -4;
constexpr float MARIO_WALK_SPEED = 2;
constexpr float MAX_VERTICAL_SPEED = 8; //limita vitezei de cadere
constexpr float MUSHROOM_SPEED = 1;
//SETEZ LATIMEA FIECARUI CADRU DIN HARTA
constexpr unsigned char CELL_SIZE = 16;
//We use this constant when Mario is growing or when he's invincible.
constexpr unsigned char MARIO_BLINKING = 4;
//1) Mario gets hit by an enemy.
//2) Mario looks at you like "Oh no! Me lost! Mama mia!" and pauses for this amount.
//3) Mario bounces off the screen.
constexpr unsigned char MARIO_DEATH_DURATION = 32;
constexpr unsigned char MARIO_GROWTH_DURATION = 64;
constexpr unsigned char MARIO_INVINCIBILITY_DURATION = 128;
//The longer we press the jump button, the higher Mario jumps.
constexpr unsigned char MARIO_JUMP_TIMER = 8;
constexpr unsigned char SCREEN_RESIZE = 2;

constexpr unsigned short COIN_ANIMATION_SPEED = 4;
//Since we deactivate everything outside the view, we need to have a safe area around the view where objects start updating.
constexpr unsigned short ENTITY_UPDATE_AREA = 64;
constexpr unsigned short GOOMBA_DEATH_DURATION = 32;
//Why do I have so many animation speed constants? I use them only once.
constexpr unsigned short GOOMBA_WALK_ANIMATION_SPEED = 32;
constexpr unsigned short KOOPA_GET_OUT_ANIMATION_SPEED = 4;
constexpr unsigned short KOOPA_GET_OUT_DURATION = 512;
constexpr unsigned short KOOPA_WALK_ANIMATION_SPEED = 32;
constexpr unsigned short MARIO_WALK_ANIMATION_SPEED = 4;
constexpr unsigned short QUESTION_BLOCK_ANIMATION_SPEED = 8;
constexpr unsigned short SCREEN_HEIGHT = 240;
constexpr unsigned short SCREEN_WIDTH = 320;
//durata fixa a unui cadru pt 60 fps
constexpr std::chrono::microseconds FRAME_DURATION(16667);
//tip celule din hartă
enum Cell
{
	ActivatedQuestionBlock,
	Brick,
	Coin,
	Empty,
	Pipe,
	QuestionBlock,
	Wall
};

//structură pentru particulele de cărămidă și monedele care sar din blocurile cu semn de întrebare
struct Object
{
	float horizontal_speed;
	float vertical_speed;
	float x;
	float y;

	Object(const float i_x, const float i_y, const float i_horizontal_speed = 0, const float i_vertical_speed = 0) :
		horizontal_speed(i_horizontal_speed),
		vertical_speed(i_vertical_speed),
		x(i_x),
		y(i_y)
	{

	}
};
//definiția hărții ca un vector de array-uri de celule
typedef std::vector<std::array<Cell, SCREEN_HEIGHT / CELL_SIZE>> Map;