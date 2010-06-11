#ifndef _AI_H_
#define _AI_H_
#include "../playfield.h"
#include "../global.h"

int value(column*);

typedef struct {
	int Score;
	int col;
}MoveValue;
int RandomAI(column*);
MoveValue BestMove(char, int, int, column* );
MoveValue MediumAI(column*, int, int);

#endif
