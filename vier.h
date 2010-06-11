#ifndef __VIER_H__
#define __VIER_H__

#ifdef _WIN32
	#include <SDL/SDL.h>
	#include <SDL/SDL_image.h>
	#include <SDL/SDL_ttf.h>
#else
	#include <SDL/SDL.h>
	#include <SDL/SDL_image.h>
	#include <SDL/SDL_ttf.h>
#endif

#define SW 800		//Scherm breedte
#define SH 600		//Scherm hoogte
#define MAXFPS 35	//Het maximum aantal frames per seconde (hoeveel keer de loop wordt uitgevoerd per seconde)


typedef enum{false, true}bool;
typedef enum{playing ,mainmenu, again, highscore, pause, diffi, entername}menustatus; //duidelijker om te zeggen menuStatus = playing, ipv menuStatus = 0



/* al onze SDL_Surfaces */
SDL_Surface *screen;			//screen gaan we gebruike voor het 'window' te maken
SDL_Surface *speelveld = NULL;		//hier wordt ons speelbord op getekend
SDL_Surface *celveld = NULL;		//hier gaan al onze jetons op getekend worden

SDL_Surface *cel = NULL;		//1 cel (cel.png) hier gaan we ons speelbord met maken
SDL_Surface *jetonRood = NULL;		//rode jeton
SDL_Surface *jetonRoodSmall = NULL;	//kleine rode jeton voor het menu
SDL_Surface *jetonGeel = NULL;		//gele jeton
SDL_Surface *jetonGeelSmall = NULL;	//kleine gele jeton voor het menu

SDL_Surface *pijltje = NULL;		//kolom selectie pijltje

SDL_Surface *againMenu = NULL;		//surface voor het menu als een spelletje afgelopen is, hier komt youwin of youlose op en de vraag of je opnieuw wilt spelen of niet
SDL_Surface *youWin = NULL;		//het You win bericht	
SDL_Surface *youLose = NULL;		//het you lose bericht
SDL_Surface *mainMenu = NULL;		//surface van het main menu
SDL_Surface *pauseMenu = NULL;		//surface voor het pause menu
SDL_Surface *diffMenu = NULL;		//surface voor het difficulty menu
SDL_Surface *highscoresMenu = NULL;	//surface voor het weergeven van de highscores
SDL_Surface *enternameMenu = NULL;		//surface voor het enter name menu
SDL_Surface *nameEntry = NULL;		//weergave van de naam input op het enter name menu

SDL_Surface *draw = NULL;		//gelijkspel message
SDL_Surface *cheated= NULL;		//computer is cheating message

SDL_Surface *grayAlpha = NULL; 	//grijze doorzichtige "overlay" (waarvan de alpha waarde minder is dan normaal, dus doorzichtig) om het scherm 'donkerder' te maken als er een spelletje afgelopen is of bij het weergeven van menus

SDL_Surface *sdlScore = NULL;		//Surface waarop de tijd van een spelletje wordt afgedrukt
SDL_Surface *tempSurface = NULL;	//een temporary surface om verscheidene dinge met te doen


ScorePair* HighScores = NULL;


/*dropping jeton 'animatie' vars*/
bool dropping = false;				//'boolean' of er een jeton aan het vallen is	
SDL_Surface* droppingJeton = NULL;		//surface voor de vallende jeton
int dropTcol, dropTrow, dropy, dropvel = 5;	//de kolom waar de jeton aan het vallen is, rij waar de jeton uiteindelijk moet komen, de y waarde waar de jeton zich bevindt en de snelheid waarmee hij valt

menustatus menuStatus;	//de status van het menu, playing, pause, mainmenu, again, ...
menustatus prevMenuStatus; //vorige menuStatus

int menuSelection;	//een integer die bijhoudt welke optie in het menu geselecteerd is

Uint32 gameStart, pauseStart, pauseTime; //tijd variabelen
/*
	met de SDL library kan je de functie SDL_GetTicks() gebruiken om de 'verlopen' milliseconden op te halen
	deze kunnen we dan gebruiken om de tijd te berekenen.
	we moetten ook bijhouden hoelang het spel gepauzeerd is geweest anders klopt de tijd niet meer
*/
TTF_Font *font = NULL;		//de fonts die gebruikt gaan worden om de menu's te maken
TTF_Font *fontBig = NULL;


int initSDL(void);
void place(SDL_Surface*, SDL_Surface*, int, int);
void makeOpa(SDL_Surface*, int, int, int);
void setupSpeelveld(SDL_Surface*, SDL_Surface*);
void pushGUI(SDL_Surface*,SDL_Surface*, int, int);
void new_game(bool*, column*, int*, int*, WinnerV*);
void initMenus(void);
void placeMenu(SDL_Surface*, char*, int, SDL_Color, SDL_Color, TTF_Font*);
void loadHighscores(void);
//void new_game(void);


#endif
