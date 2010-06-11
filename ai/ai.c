/********************************************************************************
* Vier op een rij								*
* De AI zal bestaat uit 3 moeilijkheidsgraden,					*
* 1. Random AI: zet gewoon random zetten					* 
* 1. Medium: maakt gebruik van een bewerking op het MINIMAX algorithme		*
* 2. Unfair: de "AI" zal vals spelen, en meerdere tokens per keer laten		*
*    vallen, anders is deze moeilijkheid dezelfde als normaal			*
* ai.c										*
********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "ai.h"
#include <time.h>

int RandomAI(column* playfield)
{
	MoveValue Zet;	
	do{
		Zet.col = rand()%7;
	}while ( (&playfield[Zet.col])->top == ROWS );

	return Zet.col;

}
MoveValue MediumAI(column* playfield, int HumanColPick, int LastColPick)
{
	MoveValue Zet;	
	srand(time(NULL));
	int i,j;
	int k;
	WinnerV champ;

	//Zet initiële score voor een zet (voor later eventueel random te zetten	
	Zet.Score = 50;

	for(i=0;i<COLUMNS;i++) {
		if( (&playfield[i])->top == ROWS )
			continue;

		if( push(playfield, i, PLAYER_2, NULL, NULL) ) {	
			champ=found_winner(playfield, 3);
			//pop(playfield, i);
			if( champ.winner == PLAYER_2) {
				for(j=0;j<COLUMNS;j++) {
					if( push(playfield, j, PLAYER_2, NULL, NULL) ) {
						champ = found_winner(playfield, 4);
						pop(playfield,j);
						if( champ.winner == PLAYER_2 && Zet.Score <= 60) {
							Zet.col = i;
							Zet.Score = 60;
							#ifdef DEBUG
							printf("Ik zet in %d, want daarna kan ik in %d zetten om te winnen\n", i, j);
							#endif
							break;
						}
					}
				}	
			}
			pop(playfield,i);
		}

		if( push(playfield, i, PLAYER_1, NULL, NULL) ) { 	//duw in elke kolom een jetonneke met de eigenschap dat die van de speler is
			champ = found_winner(playfield, 3);		//heeft de speler er 3 op een rij mee?
			//pop(playfield, i);
			if(champ.winner == PLAYER_1) {			//zo ja: doe nog ff een zet, en controleer of het 4 op een rij geeft
				for(j=0;j<COLUMNS;j++) {
					if( push(playfield, j, PLAYER_1, NULL, NULL) ) {
						champ = found_winner(playfield, 4);
						pop(playfield,j); 	//verwijder ineens die zet
						if( champ.winner == PLAYER_1 ) {
							if( push(playfield, i, PLAYER_2, NULL, NULL) ) {	//eerst ff zien wat er nu juist gebeurd als ik die zet doe
								for(k=0; k < COLUMNS; k++) {
									if( push(playfield, k, PLAYER_1, NULL, NULL) ) {
										champ = found_winner(playfield,4);
										pop(playfield, k);
										if( champ.winner != PLAYER_1 && Zet.Score <= 70) {
											Zet.col = k;	//geef zet mee in Zet.col, wordt toch sebiet gereturned
											Zet.Score = 70;
											if(i == j)
												Zet.col=rand()%7;
											#ifdef DEBUG
											printf("Ik zet in kolom %d, want als jij in %d en daarna in %d zet, win je.\n", i,i,j);	
											#endif
											break;
										}
									}
								}
								pop(playfield, i); //verwijder de zet die ik zou doen							
							}						
						}
					}
				}
			}
			pop(playfield, i);				//verwijder de zet die 3 op een rij geeft
		}
	}	

	for(i=0;i<COLUMNS;i++) {
		if( push(playfield, i, PLAYER_1, NULL, NULL) ) { //hier wordt er al effectief de mogelijk gechecked dat de speler kan winnen
			champ = found_winner(playfield, 4);	// blokkeer die zet altijd, ongeacht wat de vorige for's opleveren...
			pop(playfield, i);
			if(champ.winner == PLAYER_1 && Zet.Score <= 90) {
				#ifdef DEBUG				
				printf("Oei, ik zet in kolom: %d, anders had je gewonnen.\n", i);
				#endif
				Zet.col = i;
				Zet.Score = 90;
			}
		}

		if( push(playfield, i, PLAYER_2, NULL, NULL) ) { //...tenzij de computer uiteraard zelf kan winnen, doe die zet dan onvoorwaardelijk
			champ=found_winner(playfield, 4);
			pop(playfield, i);
			if( champ.winner == PLAYER_2 && Zet.Score <= 100) {
				#ifdef DEBUG
				printf("Aha, ik win dankzij m'n zet in kolom: %d.\n", i);
				#endif
				Zet.col = i;
				Zet.Score = 100;
				break; //prune, deze zet wint het spel
			}
		}
	}

	if(Zet.Score == 50) { //als niet van dit alles hierboven dient te gebeuren, doe dan een random zet
		do {
			Zet.col = rand()%7;
		}while ( (&playfield[Zet.col])->top == ROWS );	
		#ifdef DEBUG	
		printf("De zet is random gebeurd in: %d\n", Zet.col);
		#endif
	}
	#ifdef DEBUG
	printf("De score van de zet bedraagt: %d\n", Zet.Score);
	#endif
	
	/*if( push(playfield, Zet.col, PLAYER_1, NULL, NULL ) ) {
		if( push(playfield, Zet.col, PLAYER_1, NULL, NULL ) ) {
			champ = found_winner(playfield, 4);
			pop(playfield, Zet.col);
			if( champ.winner == PLAYER_1 ) {
				Zet.Score = 50;
				Zet.col=rand()%7;
			}
		}
		pop(playfield, Zet.col);
	}*/
	
	
	return Zet;
}
