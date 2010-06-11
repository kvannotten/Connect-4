/*********************************
* Vier op een rij
* Hier worden de eigenlijke spelfuncties
* gedefineerd, zoals een jeton zetten
* in onze array van stacks
* playfield.c
***********************************/

#include <stdio.h>
#include <stdlib.h>
#include "playfield.h"

/*
 * Initialiseer het speelveld.
 * Deze methode kan ook gebruikt worden om het speelveld te herinitialiseren (resetten).
 */
void init_playfield(column* stack)
{
    int i;

    for (i = 0; i < COLUMNS; i++) {
        stack++->top = 0;
    }
}

//zet alle waarden in elke stack op 0, bij het begin van elk spel
void clear_playfield(column* playfield)
{
	int i, j;
	column* stack=NULL;
	
	for(i=0; i<ROWS;i++) {
		for(j=0;j<COLUMNS;j++) {
			stack=&playfield[j];			
			stack->values[i]=0;
		}
	}
}

/*
 * Next-gen graphics visuele representatie van het speelveld. Console weergave van het speelveld ^.^
 */
void print_playfield(column* playfield)
 {
     int i, j;
     for (i = ROWS; i > 0; i--) {
         for (j = 0; j < COLUMNS; j++) {
             if (i > playfield[j].top) {
                 printf(" * ");
             } else {
                 printf(" %d ", playfield[j].values[i-1]);
             }
         }
         printf("\n");
     }
     printf("\n");
 }

/*
 * Verwijder de bovenste jeton van de kolom.
 * Geeft -1 terug wanneer de kolom reeds leeg is.
 */
char pop(column* playfield, int column_nr)
{
    column* stack = &playfield[column_nr];
	if (stack->top == 0) {
		return -1;
	}else stack->values[stack->top-1] =0;
    return stack->values[--(stack->top)];
}

/*
 * Tracht een jeton te laten vallen in de gegeven kolom.
 * Geeft 0 terug indien de kolom reeds vol is (en de zet dus ongeldig),
 * geeft 1 terug wanneer de zet succesvol is.
 */
int push(column* playfield, int column_nr, int value, SDL_Surface* j, SDL_Surface* c)
{
    column* stack = &playfield[column_nr];
	//int winner;
    if (stack->top == ROWS) {
        return 0;
    }
    stack->values[(stack->top)++] = (char)value;
	//print_playfield(playfield);
	/*if( winner=found_winner(playfield) ) {
		printf("Gewonnen: %d\n", winner);
	}*/
    
	pushGUI(j, c, column_nr, stack->top-1);
    
    return 1;
}

/*
 * Een helper methode die de push en is_game_over methodes samenvat.
 * -1 indien zet ongeldig.
 * 0 indien zet geldig en spel niet over.
 * 1 indien zet geldig en spel over.
 */
int push_and_check(column* playfield, int column_nr, int value, SDL_Surface* j, SDL_Surface* c)
{
	if (push(playfield, column_nr, value, j, c) == 0) {
		return -1;
	}
	if ( found_winner(playfield, 4).winner == PLAYER_1) {
		return 1;
	}
	if ( found_winner(playfield, 4).winner == PLAYER_2 ) {
		return 2;
	}
	return 0;
}

/*
 * Controleert of de laatst gevallen jeton vier op een rij vormt
 * Geeft 1 terug indien vier op een rij, 0 indien niet.
 */
int is_game_over(column* playfield, int row)
{
	/* Diagonaal */
	if (count_direction(playfield, row, 1, 1) + count_direction(playfield, row, -1, -1) >= 3) {
		return 1;
	} else if (count_direction(playfield, row, -1, 1) + count_direction(playfield, row, 1, -1) >= 3) {
		return 1;
	}

	/* Horizontaal */
	else if (count_direction(playfield, row, 1, 0) + count_direction(playfield, row, -1, 0) >= 3) {
		return 1;
	}

	/* Verticaal */
	else if (count_direction(playfield, row, 0, -1) >= 3) {
		return 1;
	}
    return 0;
}

/*
 * Tel het aantal gelijke jetons in een gegeven richting.
 * Bijvoorbeeld: hor 1 ver 1 is diagonaal links onder.row
 */
int count_direction(column* playfield, int column_nr, int hor, int ver)
{
	int i = 1;
	column* stack = &playfield[column_nr];
	/* Laatst gedropte jeton van deze kolom zit op stack->top-1 */
	int val = stack->values[stack->top-1];
	stack += hor;
	int next = stack->values[stack->top-1+(ver*i)];
	while(next == val) {
		i++;
		stack += hor;
		next = stack->values[stack->top-1+(ver*i)];
	}
	return i-1;
}

WinnerV found_winner(column* playfield, int AantOpEenRij)
{
	int teller[2]={0}; //0 == speler 1; 1 == speler 2
	int i,j, k;
	int l, x=0;
	WinnerV champ;
	column* stack = NULL;
	column* stack2 = NULL;

	//verticaal checken
	for(i=0; i<COLUMNS;i++) {
		stack=&playfield[i];
		teller[0]=0;
		teller[1]=0;
		for(j=0;j<ROWS-1;j++) {
			if( stack->values[j] == PLAYER_1 && stack->values[j+1] == PLAYER_1) {
				if(teller[0] == 0)
					teller[0]++;
				teller[0]++;
			} else {
			if( stack->values[j] == PLAYER_2 && stack->values[j+1] == PLAYER_2) {
				if(teller[1] == 0)
					teller[1]++;
				teller[1]++;
			} else {
				teller[0]=0;
				teller[1]=0;
			}
			}
			if(teller[0] == AantOpEenRij) {
				#ifdef DEBUG				
				printf("Verticaal gewonnen\n");	
				#endif
				champ.winner= PLAYER_1;
				champ.hwon=0;		
				return champ;
			}

			if(teller[1] == AantOpEenRij) {
				#ifdef DEBUG				
				printf("Verticaal gewonnen\n");				
				#endif
				champ.winner= PLAYER_2;
				champ.hwon=0;		
				return champ;
			}
		}
	}
	teller[0]=0;
	teller[1]=0;
	//horizontaal checken
	for(i=0;i<ROWS;i++) {
		teller[0]=0;
		teller[1]=0;
		for(j=0;j<COLUMNS-1;j++) {
			stack=&playfield[j];
			stack2=&playfield[j+1];
			if( stack->values[i] == PLAYER_1 && stack2->values[i] == PLAYER_1 ) {
				if(teller[0] == 0)
					teller[0]++;
				teller[0]++;
			} else {
			if( stack->values[i] == PLAYER_2 && stack2->values[i] == PLAYER_2 ) {
				if(teller[1] == 0)
					teller[1]++;
				teller[1]++;
			} else {
				teller[0]=0;
				teller[1]=0;
			}
			}
			if(teller[0] == AantOpEenRij) {
				#ifdef DEBUG
				printf("Horizontaal gewonnen\n");				
				#endif
				champ.winner= PLAYER_1;
				champ.hwon=1;		
				return champ;
			}
			if(teller[1] == AantOpEenRij) {
				#ifdef DEBUG
				printf("Horizontaal gewonnen\n");			
				#endif
				champ.winner= PLAYER_2;
				champ.hwon=1;		
				return champ;
			}
		}
	}
	teller[0]=0;
	teller[1]=0;
	x=0;
	//diagonaal checken == wenen :p
	//diagonaal checken zo: '/'	
	for(i=0;i<ROWS;i++) {
		teller[0]=0;
		teller[1]=0;
		for(l=0;l<COLUMNS;l++) {
			teller[0]=0;
			teller[1]=0;
			k=i;
			for(j=x;j<COLUMNS;j++) {
				stack=&playfield[j];
				stack2=&playfield[j+1];

				if( stack->values[k] == PLAYER_1 && stack2->values[k+1] == PLAYER_1) {
					if(teller[0] == 0)
						teller[0]++;
					teller[0]++;
				} else {
				if( stack->values[k] == PLAYER_2 && stack2->values[k+1] == PLAYER_2) {
					if(teller[1] == 0)
						teller[1]++;
					teller[1]++;
				}else {
					teller[0]=0;
					teller[1]=0;
				}
				}
				if(teller[0] == AantOpEenRij) {
					#ifdef DEBUG
					printf("Diagonaal / gewonnen\n");
					#endif
					champ.winner= PLAYER_1;
					champ.hwon=2;		
					return champ;
				}
				if(teller[1] == AantOpEenRij) {
					#ifdef DEBUG
					printf("Diagonaal / gewonnen\n");
					#endif
					champ.winner= PLAYER_2;
					champ.hwon=2;		
					return champ;
				}				
				k++;				
			}
		x++;
		}
		x=0;
	}
	teller[0]=0;
	teller[1]=0;
	x=0;
	//diagonaal checken zo: '\'
	for(i=ROWS;i>0;i--) {
		teller[0]=0;
		teller[1]=0;
		for(l=0;l<COLUMNS;l++) {
			teller[0]=0;
			teller[1]=0;		
			k=i;
			for(j=x;j<COLUMNS;j++) {
				stack=&playfield[j];
				stack2=&playfield[j+1];
				
				if( stack->values[k] == PLAYER_1 && stack2->values[k-1] == PLAYER_1) {
					if(teller[0] == 0)
						teller[0]++;
					teller[0]++;
				} else {
				if( stack->values[k] == PLAYER_2 && stack2->values[k-1] == PLAYER_2) {
					if(teller[1] == 0)
						teller[1]++;
					teller[1]++;
				}else {
					teller[0]=0;
					teller[1]=0;
				}
				}
				if(teller[0] == AantOpEenRij) {
					#ifdef DEBUG
					printf("Diagonaal \\ gewonnen\n");	
					#endif
					champ.winner= PLAYER_1;
					champ.hwon=3;		
					return champ;
				}
				if(teller[1] == AantOpEenRij) {
					#ifdef DEBUG
					printf("Diagonaal \\ gewonnen\n");
					#endif
					champ.winner= PLAYER_2;
					champ.hwon=3;		
					return champ;
				}

				k--;
			}
		x++;
		}
		x=0;
	}
	champ.winner= 0;
	champ.hwon=-1;		
	return champ;
}
