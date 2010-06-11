/*******************************
* Vier op een rij
* playfield header
* playfield.h
*******************************/

#ifndef __PLAYFIELD_H__
#define __PLAYFIELD_H__

#define ROWS 6
#define COLUMNS 7
#define PLAYER_1 1
#define PLAYER_2 2

#include "global.h"

#ifdef _WIN32
	#include <SDL/SDL.h>
#else
	#include <SDL/SDL.h>
#endif

typedef struct {
    char values[ROWS];
    int top;
} column;

typedef struct {
	int winner;
	int hwon; //0 = horiz; 1=vertic; 2=diag /; 3=diag \

}WinnerV;

void init_playfield(column*);
int count_direction(column*, int, int, int);
int is_game_over(column*, int);
void print_playfield(column*);
char pop(column*, int);
int push(column*, int, int, SDL_Surface*, SDL_Surface*);
int push_and_check(column*, int, int, SDL_Surface*, SDL_Surface* );
WinnerV found_winner(column*, int);
void clear_playfield(column*);

#endif
