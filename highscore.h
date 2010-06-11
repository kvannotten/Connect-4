/********************************
* Vier op een rij
* highscore header file
* highscore.h
********************************/

#ifndef __HIGHSCORE_H__
#define __HIGHSCORE_H__

typedef struct {
	char Naam[30];
	int Score;
} ScorePair;

void writescore(ScorePair*, int);
ScorePair* readscore(int);
ScorePair* OrderScores(ScorePair*, int);
void print_scores(ScorePair*, int);
void writeSingleScore(ScorePair*);

#endif
