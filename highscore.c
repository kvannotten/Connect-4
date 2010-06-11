/*******************************************
Vier op een rij

Hier worden de functies gedefineerd om
highscores weg te schrijven en te lezen, 
maar ook om te sorteren en een groepfunctie
die één enkele score wegschrijft
highscore.c
*********************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "playfield.h"
#include "highscore.h"

void writeSingleScore(ScorePair* NewScore)
{
	ScorePair* HighScore;	
	HighScore = readscore(6); //lees de scores uit de highscore file, maar lees er één meer dan er zijn
	
	strcpy( (*(HighScore + 5)).Naam, NewScore->Naam); //kopieer de nieuwe naam in het lege slot
	(*(HighScore + 5)).Score = NewScore->Score; //en de score zelf uiteraard ook

	
	//print_scores(HighScore, 6);
	OrderScores(HighScore, 6); //sorteer deze scores nu op score
	//print_scores(HighScore, 5);
	writescore(HighScore, 5); //en schrijf de beste 5 weg naar de highscore file

}

void print_scores(ScorePair* HighScore, int amount) //debug functie om scores te printen naar console
{
	int i;
	if(HighScore == NULL)
		printf("Empty pointer");

	for(i = 0;i<amount;i++) {
		printf("Naam: %s Score: %d\n", (HighScore+i)->Naam, (HighScore+i)->Score);
	}
}
void writescore(ScorePair* HighScore, int amount)
{
	int i;
	FILE* fp;
	fp=fopen("highscore.dat", "w"); //open highscore.dat voor schrijven

	if(fp == NULL) {
		printf("Kan de scores onmogelijk wegschrijven, er is een probleem met het bestand.\n");
		exit(1);
	}

	for(i=0;i<amount;i++) {
		fprintf(fp, "%s - %03x", (*(HighScore + i)).Naam, (*(HighScore + i)).Score);
	}
	fclose(fp);
}

ScorePair* readscore(int amount)
{
	ScorePair* Highscore;
	FILE* fp;
	int i, Score;
	Highscore = malloc(amount * sizeof(ScorePair)); //alloceer amount keer een ScorePair
	fp=fopen("highscore.dat", "r");

	if(fp == NULL) {
		printf("Error opening file, creating standard file!!!!\n"); 
		for(i=0;i<amount;i++) { //als er geen highscore file wordt gevonden, maak dan een standaard versie
			strcpy( (*(Highscore + i)).Naam, "abc");
			(*(Highscore+i)).Score = 0;
		}
		writescore(Highscore, amount);
		return Highscore;
	}

	for(i=0;i<amount;i++) {
		fscanf(fp, "%s - %3x", (Highscore + i)->Naam, &Score );
		(Highscore+i)->Score = Score; //dit staat op 2 regels voor de duidelijkheid ;-)
	}
	fclose(fp);
	return Highscore;
}

ScorePair* OrderScores(ScorePair* HighScore, int amount) //'standaard' bubblesort functie
{
	int i,j, k;
	int tmp;
	char buffer[30];
	for(k=0;k<amount-1;k++){	
		for(i=0;i<amount-1;i++) {
			for(j=0;j<amount-i-1;j++) {
				if( (*(HighScore + i)).Score < (*(HighScore + i + 1)).Score ) {
					strcpy(buffer, (*(HighScore + i)).Naam );			
					tmp=(*(HighScore + i)).Score;
					(*(HighScore + i)).Score = (*(HighScore + i +1)).Score;
					strcpy( (*(HighScore + i)).Naam, (*(HighScore + i+1)).Naam);
					(*(HighScore + i+1)).Score = tmp;
					strcpy( (*(HighScore + i+1)).Naam, buffer);
				}
			}
		}
	}
	return HighScore;
}
