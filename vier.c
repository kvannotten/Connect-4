/**********************************
* Vier op een rij
* hier staat de main van het programma
* en ook de grafische interface
* vier.c
**********************************/
#include <stdio.h>
#include <stdlib.h>
#include "playfield.h"
#include "ai/ai.h"
#include "highscore.h"
#include "vier.h"

int main(int argc, char** args)
{

	SDL_Event event;	//de event variabele houdt alle events bij, KEYDOWN, KEYUP, muisclicks, etc
	bool quit = false;	//onze boolean die onze hoofd loop laat draaien, als deze true wordt gaan we uit de loop en wordt het programma afgesloten

	Uint32 tStart, tTicks; 	//nog eens 2 tijd variabelen maar deze worden gebruikt om het aantal uitvoeringen van de loop per seconde te regelen	(zie #define MAXFPS)
	Uint8 *keystates;	//deze variabele kan voor alle toetsen op het toetsenbord de status bijhouden (ingedrukt of niet ingedrukt)
	
	bool sw=false;
	column playfield[COLUMNS];	//ons 'onzichtbaar' speelveld waar de status van het spel 
	
	int playerTurn = PLAYER_1;
	int Score =0;
	int CountMoves=0;		//het aantal zetten wordt in deze variabele getelt
	WinnerV champ;
	MoveValue CompMove={-1,-1};
	
	
	int Difficulty = 1, DifTeller = rand() % 2; //difficulty is standaard Medium AI
	
	

	bool cheat_sw= false;	//een boolean die aangeeft of de AI gecheat heeft of niet
	//als cheat_sw true is krijgt deze een waarde, als cheatMessageTime een waarde heeft wordt het bericht 'computer is cheating' getoont
	//en cheatMessageTime verminderd, als cheatMessageTime 0 berijkt wordt cheat_sw op false gezet
	int cheatMessageTime = 0;	

	int pickCol;		//geselecteerde kollom (0 - 6)
	int pijltjePos;	//de X positie van het column select pijltje
	
	char cScore[5] = "";	//een string om de score in te zetten
	int iTime;		//een integer om de verlopen seconden in een spel op te slagen
	
	char nameInput[11] = "";	//string om de naam in op te slagen die wordt ingevoerd als er een highscore verbroken is (max 10 letter)
	int nameCount = 0;		//Teller om de characters in de ingegeve naam bij te houden
	int i;				//gebruikt om in een loopje heel de nameInput array leeg te maken
	ScorePair newScore;		//struct ScorePair om de nieuwe score en naam in te zetten en deze daarna weg te schrijven
		
	int itemp1, itemp2;		//2 temporary integers om verschillende dinge mee te doen


	if(initSDL()){ //De SDL Module initialiseren, als dit mislukt kan het spel niet werken dus stoppen we meteen

		SDL_WM_SetCaption("Connect Four", NULL); //De titel van het scherm instellen
		
		/* andere load functies */		
		font = TTF_OpenFont("img/cih.ttf", 40);		//we laden onze font cih.ttf in (met de SDL_ttf functie TTF_OpenFont) met grote 40pt
		fontBig = TTF_OpenFont("img/cih.ttf", 70);	//we laden opnieuw cih.ttf in maar met grote 70pt
		
		/*
		 in de volgende reeks regels gaan we onze afbeeldingen inladen en klaarmaken voor het tekenen.
		hierbij maken we gebruik van een SDL functie, namelijk SDL_Surface* IMG_Load(char*), hierin geef je het path van de afbeeldingen en de functie geeft deze terug in de vorm van een SDL_Surface*
		daarna gebruiken we de functie SDL_Surface* SDL_DisplayFormat(SDL_Surface*), deze functie zet onze surface in het juiste formaat met de ingestelde bit-diepte van het scherm.
		vervolgens gaan we gebruik maken van onze zelfgemaakte functie void makeOpa(SDL_Surface*, int, int, int); (voor meer uitleg, zie makeOpa functie)	
		*/

		cel = IMG_Load("img/cel.png");
		cel = SDL_DisplayFormat(cel);
		makeOpa(cel, 255, 0 , 255);
		
		///onze speelveld surface is eigenlijk een soort layer we maken hier dus een lege Surface van met de SDL_CreateRGBSurface functie. 
		//Hierop gaan we dan ROWS * COLUMNS cel surfaces op tekenen
		speelveld = SDL_CreateRGBSurface(SDL_SWSURFACE, 80 * 7, 80 * 6, 32, 0, 0, 0, 0);
		speelveld = SDL_DisplayFormat(speelveld);
		makeOpa(speelveld, 0, 0, 0);
		
		setupSpeelveld(speelveld, cel); //Met deze functie gaan we onze speelveld 'layer' vullen met cellen, om zo het speelbord te creeren
		
		highscoresMenu = SDL_CreateRGBSurface(SDL_SWSURFACE, SW, SH, 32, 0, 0, 0, 0); //highScoreMenu 'layer' maken
		highscoresMenu = SDL_DisplayFormat(highscoresMenu);
		makeOpa(highscoresMenu, 0, 0, 0);

		celveld = SDL_CreateRGBSurface(SDL_SWSURFACE, 80 * 7, 80 * 6, 32, 0, 0, 0, 0); //celveld 'layer' maken
		celveld = SDL_DisplayFormat(celveld);
		makeOpa(celveld, 0, 0, 0);

		jetonRood = IMG_Load("img/jetonRood.png");
		jetonRood = SDL_DisplayFormat(jetonRood);
		makeOpa(jetonRood, 255, 0 , 255);

		jetonGeel = IMG_Load("img/jetonGeel.png");
		jetonGeel = SDL_DisplayFormat(jetonGeel);
		makeOpa(jetonGeel, 255, 0, 255);
		
		jetonGeelSmall = IMG_Load("img/jetonGeelSmall.png");
		jetonGeelSmall = SDL_DisplayFormat(jetonGeelSmall);
		makeOpa(jetonGeelSmall, 255, 0, 255);
		
		jetonRoodSmall = IMG_Load("img/jetonRoodSmall.png");
		jetonRoodSmall = SDL_DisplayFormat(jetonRoodSmall);
		makeOpa(jetonRoodSmall, 255, 0, 255);
		
		pijltje = IMG_Load("img/pijltje.png");
		pijltje = SDL_DisplayFormat(pijltje);
		makeOpa(pijltje, 255, 0, 255);
		
		grayAlpha = SDL_CreateRGBSurface(SDL_SWSURFACE, SW, SH, 32, 0, 0, 0, 0); //grayAlpha 'layer'
		SDL_FillRect(grayAlpha, &grayAlpha->clip_rect, SDL_MapRGB(grayAlpha->format, 70, 70, 70)); //SDL_FillRect tekent een vierkant over de hele surface met de opgegeven RGB kleur (in dit geval grijs)
		
		//met deze functie veranderen we de 'Alpha' waarde van onze surface, de doorzichtigheid (doorzichtig = 0, niet doorzichtig = 255)
		SDL_SetAlpha(grayAlpha, SDL_SRCALPHA, 160);	

	
		pijltjePos = SW / 2 - (pijltje->w / 2) - 80 * 3; //de x waarde van het column select pijltje, deze berekening zet het pijltje op de eerst kollom
		pickCol = 0;	//de geselecteerde kollom (0 = de eerste kollom)
		
		initMenus(); //voor uitleg, zie initMenus functie
		
		menuStatus = mainmenu;	//we zetten de menuStatus op mainmenu zodat het spel start op het mainmenu
		menuSelection = 0;		//selectie is 0 zodat het geslecteerde menuItem het eerste zal zijn

		//Hier begint onze main loop, het programma zal deze blijven uitvoeren tot de gebruiker het spel afsluit.
		while(!quit){
			tStart = SDL_GetTicks();	
			
			//we maken het scherm helemaal zwart, dit zodat alles wat we in de vorige loop getekend hebbe terug weg is
			//anders blijft alles staan en overlappen ze elkaar
			SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen -> format, 0, 0, 0));
			
			if(!sw && menuStatus != pause){
				iTime = (SDL_GetTicks() - gameStart - pauseTime) / 1000;	//we berekenen de tijd die al  voorbij is gegaan
				
				//we berekenen de score adhv de moeilijkheidsgraad
				Score = ((COLUMNS*ROWS) - (CountMoves - (CountMoves / 2)) + (100 - iTime)) * 5;	
				if(Difficulty == 0)Score /= 5;
				if(Difficulty == 2)Score += 500;
				
				//de score in een string stoppen en in een surface zetten zodat we deze kunne tekenen
				// dankzij de SDL_ttf library is dit heel simpel, we hoeven enkel de functie TTF_RenderTextShaded op te roepen,
				// een font, tekst, kleur en achtergrondkleur mee te geven en vervolgens returned de functie een SDL_Surface pointer terug met de juiste tekst en kleur
				sprintf(cScore, "%s - %d", "Score", Score);
				sdlScore = TTF_RenderText_Shaded(font, cScore, (SDL_Color){255, 255, 255}, (SDL_Color){0, 0, 0});
				makeOpa(sdlScore, 0, 0, 0);
			}

			//de event loop. Deze loop vangt alle events op, bv: er wordt op een toets gedrukt, er wordt geklikt, ...
			
			//wij gebruiken vooral de event SDL_KEYDOWN (een toets werd ingedrukt)
			//als dit gebeurt gaan we ook kijken naar de menuStatus, een enkele toets doet niet hetzelfde op alle menus
			//daarna wordt er de juist functie opgeroepen, iets berekent, ...
			while(SDL_PollEvent(&event)){
				if(event.type == SDL_QUIT)quit = true; //Er is op het kruisje geklikt, dus we moetten uit de while lus gaan

				if(event.type == SDL_KEYDOWN){
					//het entername menu (als er een highscore verbroken is) krijgt een speciaal appart stukje
					//dit omdat er chars moetten opgevangen worden en in de nameInput array worden gestopt
					if(menuStatus == entername){
						
						//de array is 11 groot dus er mogen maar 10 letters in de array staan
						if(nameCount < 10){
							//als er een letter tussen a-z of A-Z wordt gedrukt voegen we deze toe, dus we accepteren enkel kleine en grote letter meer niet
							if((event.key.keysym.unicode >= (Uint16)'a' && event.key.keysym.unicode <= (Uint16)'z') || (event.key.keysym.unicode >= (Uint16)'A' && event.key.keysym.unicode <= (Uint16)'Z')){
								nameInput[nameCount] = (char)event.key.keysym.unicode;
								nameCount++;	
							}

						}
						//als er op backspace wordt gedrukt
						if(event.key.keysym.sym == SDLK_BACKSPACE && nameCount > 0){
							nameCount--;
							nameInput[nameCount] = '\0';

						}

					}
				
					switch(event.key.keysym.sym){ //event.key.keysym.sym houdt de gedrukte toets bij in de vorm van SDLK_<toets>
				
						case SDLK_RIGHT:
							switch(menuStatus){
							
							case playing:
								pijltjePos = SW / 2 - (pijltje->w / 2) - 80 * 3;


								pickCol++;
								if(pickCol > 6)pickCol = 0;
								pijltjePos += pickCol * 80;
								break;
							
							}
							break;

						case SDLK_LEFT:
						
							switch(menuStatus){
							
							case playing:
								pijltjePos = SW / 2 - (pijltje->w / 2) - 80 * 3;


								pickCol--;
								if(pickCol < 0)pickCol = 6;
								pijltjePos += pickCol * 80;
								break;
							}
							break;
							
						case SDLK_UP:
							switch(menuStatus){
								case mainmenu:
									menuSelection--;
									if(menuSelection < 0)menuSelection = 3;
									break;
							
								case again:
									menuSelection--;
									if(menuSelection < 0)menuSelection = 1;
									break;
								case pause:
									menuSelection--;
									if(menuSelection < 0) menuSelection = 2;
									break;
									
								case diffi:
									menuSelection--;
									if(menuSelection < 0)menuSelection = 2;
									break;
							
							}
						
							break;
						case SDLK_ESCAPE:
							if(menuStatus == playing){
								pauseStart = SDL_GetTicks();
								menuStatus = pause;
								
							}
					
							break;
	
						case SDLK_DOWN: 
							if(menuStatus == again){
							
								menuSelection++;
								if(menuSelection > 1)menuSelection = 0;
								break;
							}
							else if(menuStatus == mainmenu){
							
								menuSelection++;
								if(menuSelection > 3)menuSelection = 0;
							
								break;
							}
							else if(menuStatus == pause){
								
								menuSelection++;
								if(menuSelection > 2)menuSelection = 0;
								break;
							
							}
							else if(menuStatus == diffi){
								menuSelection++;
								if(menuSelection > 2)menuSelection = 0;
								break;
							
							}
							if(menuStatus == highscore) break;
							
						case SDLK_RETURN:
						case SDLK_SPACE:
							
							
							switch(menuStatus){
							
							case playing:
								//als er op spatie, enter of het pijlte naar onder wordt gedrukt als menuStatus playing is en playerTurn PLAYER_1 is proberen we een jeton te laten vallen in pickCol
								if(playerTurn == PLAYER_1 && !sw && !dropping){
									if(push(playfield, pickCol, playerTurn, jetonGeel, celveld)) { //voor meer uitleg zie push (in playfield.c) 
										champ= found_winner(playfield, 4); //voor meer uitleg zie found_winner (in playfield.c)
										
										if( champ.winner == PLAYER_1 ) { //als speler 1 gewonnen heeft gaan we de score berekenen, kijken of hij in de highscore komt en vragen of hij opnieuw wilt spelen
											CountMoves++;
											
											sw=true;	//als sw true is betekent het dat er iemand gewonnen heeft, het spel is dus gedaan
											Score = ((COLUMNS*ROWS) - (CountMoves - (CountMoves / 2)) + (100 - iTime)) * 5;
											if(Difficulty == 0)Score /= 5;
											if(Difficulty == 2)Score += 500;
										
											HighScores = readscore(5);
											
											if(Score > HighScores[4].Score){
												i = 0;
												while(i < nameCount){
													nameInput[i] = '\0';
													i++;
												}
												nameCount = 0;
												printf("Entering entername");
												menuStatus = entername;
											}
											else
											{
												menuStatus = again;
											}
											
											#ifdef DEBUG
											printf("Je score bedraagt: %d\n", Score);
											#endif
											playerTurn = PLAYER_2;
										} else {
											//als er niet gewonnen is gaat het spel verder
											playerTurn = PLAYER_2;
											CountMoves++;
											if(CountMoves == ROWS * COLUMNS)menuStatus = again; //als CountMoves gelijk is aan ROWS * COLUMNS staat het speelveld vol en is er niemand gewonne, dus het spel is gedaan
										}
									}


							

								}
								break;
							case mainmenu:		
								switch(menuSelection){
								
									case 0: //new game
										cheatMessageTime = 0;
										new_game(&sw, playfield, &CountMoves, &Score, &champ);
							
										 break;
									case 1: //difficulty menu
										prevMenuStatus = mainmenu;
										menuSelection = Difficulty;
										menuStatus = diffi;
										break;
									case 2: //highscore menu
										loadHighscores();
										menuStatus = highscore;
										
										break;
									
									case 3: quit = true; break; //exit	
								
								}
									
								break;
								
							case again:
								if(menuSelection == 0){ //yes
									cheatMessageTime = 0;
									new_game(&sw, playfield, &CountMoves, &Score, &champ);
								}
								else{	//no
									menuStatus = mainmenu;
									menuSelection = 0;
								}
								break;
								
							case pause:
								if(menuSelection == 0){	//continue
									pauseTime += SDL_GetTicks() - pauseStart;	
									menuStatus = playing;
								}
								else if(menuSelection == 1){	//difficulty menu
									prevMenuStatus = pause;
									menuSelection = Difficulty;
									menuStatus = diffi;
								
								}
								
								else if(menuSelection == 2){	//leave game
									menuStatus = mainmenu;
									menuSelection = 0;	
								}
							
								break;
								
							case highscore:
								menuSelection = 0;
								menuStatus = mainmenu;
								break;
								
							case diffi:
								Difficulty = menuSelection;
								if(prevMenuStatus == pause && playerTurn == PLAYER_1)playerTurn = PLAYER_2; //als je de difficulty veranderd tijdens een spel verlies je je beurt
								menuStatus = prevMenuStatus;
								menuSelection = 0;
								break;
								
							case entername:
								if(nameCount > 0){
									strcpy(newScore.Naam, nameInput);
									newScore.Score = Score;

									writeSingleScore(&newScore);
									menuStatus = again;
								}
							}
							break;
							
        				}

				}

			}//einde event loop
			
			//als de AI aan de beurt is om een zet te maken
			if(playerTurn == PLAYER_2 && !sw && !dropping && menuStatus == playing) {
				switch(Difficulty) {
					case 0: CompMove.col = RandomAI(playfield); break; 
					case 1:
					case 2: CompMove = MediumAI(playfield, pickCol, CompMove.col); break;//BestMove(playerTurn, 0, 6, playfield);
				}
				if( push(playfield, CompMove.col/*pickCol*/,playerTurn, jetonRood, celveld)) {
					champ= found_winner(playfield, 4);
					if( champ.winner == PLAYER_2 ) {
						print_playfield(playfield);
						CountMoves++;
						//printf("Speler 2 (computer) heeft gewonnen in %d zetten.\n", CountMoves);
						sw=true;
						
						menuStatus = again;
						
						playerTurn=PLAYER_1;
					} else {
						playerTurn=PLAYER_1;
						CountMoves++;
						if(CountMoves == ROWS * COLUMNS)menuStatus = again;
						
							//als de difficulty op unfair staat is er een kans van 1/3 dat de computer valsspeelt en 1 beurt van jou overslaat, dus de computer zet 2 jetons na elkaar
							if( Difficulty == 2 && DifTeller == rand() % 3 ) {
								playerTurn = PLAYER_2;
								DifTeller = -1;
								cheatMessageTime = 200;
								cheat_sw=true;
							} else{
								if( Difficulty == 2 && DifTeller == -1 ){
									playerTurn = PLAYER_1;								
									DifTeller = rand() % 3;
								}
							}
					}
				}
			}
		  /* -------------------- */
		  
		  //de drop 'animatie'
		  if(dropping){
		   	if(dropy <= dropTrow * 80){
		   		dropvel += 10;
		   		dropy += 15 + dropvel;
		   		place(droppingJeton, screen, SW / 2 - (pijltje->w / 2) - 80 * 3 +(dropTcol * 80), dropy);
		   	}
		   	else{
		   		//als de jeton de dropTrow rij behaald heeft moetten we stoppe met de animatie
		   		//anders valt onze jeton gewoon verder naar beneden, droppin wordt dus false
		   		//en we tekenen onze jeton op de juiste plaats op de surface celveld (jeton 'layer')
				#ifdef DEBUG					   		
				printf("dropping stopped\n");
				#endif
		   		dropping = false;
		   		
		   		place(droppingJeton, celveld, dropTcol * 80, dropTrow * 80);
				
				//we resetten de droppping animatie variabelen
		   		dropy = 0;
		   		dropTcol = 0;
		   		dropTrow = 0;
		   		dropvel = 5;
		   	}

		   } //einde drop animatie

				place(celveld, screen, SW / 2 - (celveld->w / 2), 50);		//de layer met alle jeton tekenen
				place(speelveld, screen, SW / 2 - (speelveld->w / 2), 50);	//hierover de layer met het speelbord tekenen (hier zitten 'gaten' in door onze makeOpa functie dus kunnen we de jetons zien)

				switch(menuStatus){	//adhv de menustatus tekenen we iets anders
					case playing:
						place(pijltje,screen, pijltjePos, 10);
						place(sdlScore, screen, 10, SH - TTF_FontLineSkip(font));
						if( Difficulty == 2 && cheat_sw){
							if(cheatMessageTime > 0){
								cheatMessageTime-= 10;
								place(grayAlpha, screen, 0,0);
								place(cheated, screen, SW / 2 - (cheated->w / 2), 130);
							}
							else{
								cheat_sw = false;
							}
						}
						
						
						break;
						
					case mainmenu:
							place(grayAlpha, screen, 0, 0);
							place(mainMenu, screen, SW / 2 - (mainMenu->w / 2), 0);
							
							//omdat we de exacte X waarden van de menu items niet meer kennen moeten we ze anders opzoeken
							//TTF_SizeText geeft de breedte en hoogte terug die nodig is voor een bepaalde tekst met een bepaalde font en stopt deze in 2 integers (hier itemp1 en itemp2, respectievelijk breedte en hoogte)
							switch(menuSelection){
								case 0:
									TTF_SizeText(font, "New Game", &itemp1, &itemp2);
									break;
								case 1:
									TTF_SizeText(font, "Difficulty", &itemp1, &itemp2);
									break;
								case 2:
									TTF_SizeText(font, "Highscores", &itemp1, &itemp2);
									break;
								case 3:
									TTF_SizeText(font, "Exit", &itemp1, &itemp2);
									break;
							
							}
							
							//bij het plaatse van de menu jetons is er even op het zicht gewerkt (de y start waarde 208), dit omdat de juiste Y waarde van de woorden niet beschikbaar zijn
							
							//omdat de tekst in het midden van het scherm is geplaatst en we de breedte van elk item kennen is de X waarde makkelijk te berekenen:
							//het midden van het scherm min de helft van de breedte van het item geeft ons de exacte X waarde
							place(jetonGeelSmall, screen, SW / 2 - (itemp1 / 2) - jetonGeelSmall->w - 10, 208 + (menuSelection * (TTF_FontLineSkip(font) + 10)));
							place(jetonRoodSmall, screen, SW / 2 + (itemp1 / 2) + 10, 208 + (menuSelection * (TTF_FontLineSkip(font) + 10)));
						break;
					case pause:
						place(grayAlpha, screen, 0, 0);
						place(pauseMenu, screen, SW / 2 - (pauseMenu->w / 2), 0);
						switch(menuSelection){
							case 0:
								TTF_SizeText(font, "Continue", &itemp1, &itemp2);
								break;
							case 1:
								TTF_SizeText(font, "Difficulty", &itemp1, &itemp2);
								break;
							case 2:
								TTF_SizeText(font, "Leave game", &itemp1, &itemp2);
								break;
						}
						
						place(jetonGeelSmall, screen, SW / 2 - (itemp1 / 2) - jetonGeelSmall->w - 10, 200 + (menuSelection * (TTF_FontLineSkip(font) + 10)) - 2);
						place(jetonRoodSmall, screen, SW / 2 + (itemp1 / 2) + 10, 200 + (menuSelection * (TTF_FontLineSkip(font) + 10)) - 2);
						
						break;	
					case again:
						place(grayAlpha, screen, 0, 0);
						
							if(champ.winner == PLAYER_1){
								place(youWin, screen, SW / 2 - (youWin->w / 2), 130);
							}
							else if(champ.winner == PLAYER_2){
								place(youLose, screen, SW / 2 - (youLose->w / 2), 130);
							}
							else{
								place(draw, screen, SW / 2 - (youLose->w / 2), 130);
							}

						place(againMenu, screen, SW / 2 - (againMenu->w / 2), SH / 2 - (againMenu->h / 2) + 50);
						
						place(jetonGeelSmall, screen, SW / 2 - 90, 338 + (41 * menuSelection));
						place(jetonRoodSmall, screen, SW / 2 + 50, 338 + (41 * menuSelection));
						break;
						
						
					case highscore:
						place(grayAlpha, screen, 0, 0);
						place(highscoresMenu, screen, 0, 0);
						
						TTF_SizeText(font, "Back", &itemp1, &itemp2);
						
						place(jetonGeelSmall, screen, SW /2 - (itemp1 / 2) - jetonGeelSmall->w - 10, 200 + (6 * (TTF_FontLineSkip(font) + 10)));
						place(jetonRoodSmall, screen, SW /2 + (itemp1 / 2) + 10, 200 + (6 * (TTF_FontLineSkip(font) + 10)));
						break;
						
					case diffi:
						place(grayAlpha, screen, 0, 0);
						place(diffMenu, screen, SW / 2 - (diffMenu->w / 2), 0);
						
						switch(menuSelection){
						
							case 0:
								TTF_SizeText(font, "Random AI", &itemp1, &itemp2);
								break;
								
							case 1:
								TTF_SizeText(font, "Medium AI", &itemp1, &itemp2);
								break;
								
							case 2:
								TTF_SizeText(font, "Unfair AI", &itemp1, &itemp2);
								break;
								
							case 3:
								TTF_SizeText(font, "All of the above", &itemp1, &itemp2);
								break;
						
						}
						place(jetonGeelSmall, screen, SW / 2 - (itemp1 / 2) - jetonGeelSmall->w - 10, 200 + (menuSelection * (TTF_FontLineSkip(font) + 10)));
						place(jetonRoodSmall, screen, SW / 2 + (itemp1 / 2) + 10, 200 + (menuSelection * (TTF_FontLineSkip(font) + 10)));
						
						break;
						
					case entername:
						place(grayAlpha, screen, 0, 0);
						place(enternameMenu, screen, 0, 0);
						
						
						if(nameCount > 0){
						nameEntry = TTF_RenderText_Shaded(font, nameInput, (SDL_Color){255, 255, 255}, (SDL_Color){0, 0, 0});
						nameEntry = SDL_DisplayFormat(nameEntry);
						makeOpa(nameEntry, 0, 0, 0);
						
						place(nameEntry, screen, SW / 2 - (nameEntry->w / 2), 200 + (2 * TTF_FontLineSkip(font)));
						}
						
						TTF_SizeText(font, nameInput, &itemp1, &itemp2);
						place(jetonGeelSmall, screen, SW / 2 - (itemp1 / 2) - jetonGeelSmall->w - 10, 200 + (2 * TTF_FontLineSkip(font)));
						place(jetonRoodSmall, screen, SW / 2 + (itemp1 / 2) + 10, 200 + (2 * TTF_FontLineSkip(font)));
					
						break;
				
				}

			/* ------------------- */
			tTicks = SDL_GetTicks() - tStart; //we berekenen hoeveel milliseconden er verlopen zijn om de loop uit te voeren
			if(tTicks < 1000 / MAXFPS ){
				SDL_Delay((1000 / MAXFPS) - tTicks); //we pauzeren onze loop voor X aantal milliseconden
			}
			
			//omdat we doublebuffered werken hebben we hierboven alles op de buffer getekent
			//dus om dit nu weer te geven horen we deze te 'flippen'
			SDL_Flip(screen); 
			
		}//end while(!quit)


	}
	else{
		printf("Couldn't start, exiting...\n");
	}
	return 0;
}

void loadHighscores(void)
{
	SDL_Color cWhite = {255, 255, 255};
	SDL_Color cBlack = {0, 0, 0};
	int i = 0;
	char *tmp = malloc(50 * sizeof(char));
	
	HighScores = readscore(5);
		
	SDL_FillRect(highscoresMenu, &highscoresMenu->clip_rect, SDL_MapRGB(highscoresMenu->format, 0, 0, 0));
	placeMenu(highscoresMenu, "Highscores", 70, cWhite, cBlack, fontBig);
	
	while(i < 5){
		sprintf(tmp, "%s - %d", HighScores[i].Naam, HighScores[i].Score);
		
		placeMenu(highscoresMenu, tmp, 200 + (i * (TTF_FontLineSkip(font) + 10)), cWhite, cBlack, font);
	
	i++;
	}
		
	placeMenu(highscoresMenu, "Back", 200 + (++i * (TTF_FontLineSkip(font) + 10)), cWhite, cBlack, font);
	
	free(tmp);
}


void initMenus(void)
{	
	/*
		deze functie is gemaakt om de menus te initialliseren, alle menu Items te creeren, ...
		we maken hieren ook gebruik van de functie placeMenu
	
	*/
	SDL_Color cWhite = {255, 255, 255};	//witte kleur
	SDL_Color cBlack = {0, 0, 0};		//zwarte kleur
	
	int top;
	
	
	/* main menu */
	mainMenu = SDL_CreateRGBSurface(SDL_SWSURFACE, 500, SH, 32, 0, 0, 0, 0);
	mainMenu = SDL_DisplayFormat(mainMenu);
	makeOpa(mainMenu, 0, 0, 0);
	
	placeMenu(mainMenu, "Connect Four!", 70, cWhite, cBlack, fontBig);
	top = 70 + TTF_FontLineSkip(fontBig);
	
	placeMenu(mainMenu, "New Game", top + 70, cWhite, cBlack, font);
	placeMenu(mainMenu, "Difficulty", top + 70 + TTF_FontLineSkip(font) + 10, cWhite, cBlack, font);
	placeMenu(mainMenu, "Highscores", top + 70 + (TTF_FontLineSkip(font) + 10) * 2, cWhite, cBlack, font);
	placeMenu(mainMenu, "Exit", top + 70 + (TTF_FontLineSkip(font) + 10) * 3, cWhite, cBlack, font);
	/* --------- */
	
	

	/* again Menu */
	againMenu = SDL_CreateRGBSurface(SDL_SWSURFACE, 500, 300, 32, 0, 0, 0, 0);
	againMenu = SDL_DisplayFormat(againMenu);
	makeOpa(againMenu, 0, 0, 0);
	
	
	placeMenu(againMenu, "Play again?", 60, cWhite, cBlack, font);
	placeMenu(againMenu, "Yes", 140, cWhite, cBlack, font);
	placeMenu(againMenu, "No", 140 + TTF_FontLineSkip(font), cWhite, cBlack, font);
	/* --------- */
	
	/* pause menu */
	pauseMenu = SDL_CreateRGBSurface(SDL_SWSURFACE, 500, SH, 32, 0, 0, 0, 0);
	pauseMenu = SDL_DisplayFormat(pauseMenu);
	makeOpa(pauseMenu, 0, 0, 0);
	
	placeMenu(pauseMenu, "Pause", 70, cWhite, cBlack, fontBig);
	placeMenu(pauseMenu, "Continue", 200, cWhite, cBlack, font);
	placeMenu(pauseMenu, "Difficulty", 200 + TTF_FontLineSkip(font) + 10, cWhite, cBlack, font);
	placeMenu(pauseMenu, "Leave game", 200 + 2 * (TTF_FontLineSkip(font) + 10), cWhite, cBlack, font);
	/* ---------- */
	
	
	/* difficulty menu */
	diffMenu = SDL_CreateRGBSurface(SDL_SWSURFACE, SW, SH, 32, 0, 0, 0, 0);
	diffMenu = SDL_DisplayFormat(diffMenu);
	makeOpa(diffMenu, 0, 0, 0);
	
	placeMenu(diffMenu, "Select difficulty", 70, cWhite, cBlack, fontBig);
	placeMenu(diffMenu, "Random AI", 200, cWhite, cBlack, font);
	placeMenu(diffMenu, "Medium AI", 200 + TTF_FontLineSkip(font) + 10, cWhite, cBlack, font);
	placeMenu(diffMenu, "Unfair AI", 200 + 2 * (TTF_FontLineSkip(font) + 10), cWhite, cBlack, font);
	/* --------------- */
	
	
	/* enter name menu */
	enternameMenu = SDL_CreateRGBSurface(SDL_SWSURFACE, SW, SH, 32, 0, 0, 0, 0);
	enternameMenu = SDL_DisplayFormat(enternameMenu);
	makeOpa(enternameMenu, 0, 0, 0);
	
	placeMenu(enternameMenu, "You have a highscore!", 70, cWhite, cBlack, fontBig);
	placeMenu(enternameMenu, "Please enter your name:", 200, cWhite, cBlack, font);
	
	/* --------------- */
	
	
	// you win, you lose, draw en computer cheated bericht op de surface zetten
	youWin = TTF_RenderText_Shaded(fontBig, "You Win!!", (SDL_Color){20, 200, 30}, (SDL_Color){0, 0, 0});
	youWin = SDL_DisplayFormat(youWin);
	makeOpa(youWin, 0, 0, 0);
	
	
	youLose = TTF_RenderText_Shaded(fontBig, "You Lose!!", (SDL_Color){255, 0, 0}, (SDL_Color){0, 0, 0});
	youLose = SDL_DisplayFormat(youLose);
	makeOpa(youLose, 0, 0, 0);
	
	draw = TTF_RenderText_Shaded(fontBig, "Draw!!", (SDL_Color){195, 113, 15}, (SDL_Color){0, 0, 0});
	draw = SDL_DisplayFormat(draw);
	makeOpa(draw, 0, 0, 0);

	cheated = TTF_RenderText_Shaded(fontBig, "Computer is cheating!!", (SDL_Color){255, 0, 0}, (SDL_Color){0, 0, 0});
	cheated = SDL_DisplayFormat(cheated);
	makeOpa(cheated, 0, 0, 0);
}


void placeMenu(SDL_Surface* t, char* text, int y, SDL_Color color, SDL_Color bgColor, TTF_Font* font)
{
	/* deze functie zet de opgegeven tekst op het opgegeven surface (automatisch gecentreerd) me de opgegeven Y waarde */
	SDL_Surface* temp = NULL;
	temp = TTF_RenderText_Shaded(font, text, color, bgColor);
	temp = SDL_DisplayFormat(temp);
	makeOpa(temp, bgColor.r, bgColor.g, bgColor.b);
	place(temp, t, t->w / 2 - (temp->w / 2), y);
	
	SDL_FreeSurface(temp);
}

void new_game(bool* sw, column* p, int* cm, int* s, WinnerV* c)
{
	/* new game functie
		reset de benodigde variabelen zodat er een nieuw spel kan worden gestart	
	*/
	gameStart = SDL_GetTicks();
	*sw = false;
	clear_playfield(p);		//voor meer info zie clear_playfield	(playfield.c)
	init_playfield(p);		//voor meer info zie init_playfield	(playfield.c)
	
	*cm = 0;
	*s = 0;
	c->winner = 0;
	
	pauseTime = 0;
	
	menuStatus = playing;
	
	
	SDL_FillRect(celveld, &celveld->clip_rect, SDL_MapRGB(celveld -> format, 0, 0, 0));	//de layer met al de getekende jetons moet ook leegemaakt worden
}

void place(SDL_Surface* s, SDL_Surface* t, int x, int y)
{
	//omdat SDL werkt met een struct SDL_Rect (waarin de x en y waarden worden bijgehouden) gebruikt om surfaces te tekenen hebben we deze functie gemaakt
	//zoniet moesten we voor elke surface een apparte SDL_Rect maken, dit zou overbodig werk geweest zijn
	SDL_Rect r;

	r.x = x;
	r.y = y;

	SDL_BlitSurface(s, NULL, t, &r); //we tekenen Surface 's' op Surface 't' met de x en y waarde in strut 'r'
}

void pushGUI(SDL_Surface* j, SDL_Surface* v, int col, int row)
{
	//deze functie stelt de variabelen dropTrow, dropTcol en droppingJeton in zodat de 'drop animatie' kan worden gebruik
	//we zetten de boolean dropping dus true, want er wordt een jeton gedropt
	row = 5 - row;
	dropping = true;

	dropTrow = row;
	dropTcol = col;

	droppingJeton = j;
}

void setupSpeelveld(SDL_Surface* v, SDL_Surface* c)
{
	int i = 0, j;
	int x, y = 0;


	while(i < 6){
		j = 0;
		x = 0;
		while(j < 7){
			place(c, v, x, y);
			x+= 80;
		j++;
		}
		y+= 80;
	i++;
	}

}

void makeOpa(SDL_Surface* s, int r, int g, int b)
{
	//SDL kan met het gebruik van een 'colorKey' een bepaalde kleur wegfilteren uit een Surface, dit zodat een bepaald gebied helemaal doorzichtig wordt
	//we hebben hiervoor een handige functie geschreven zodat we dit bij eender welke Surface kunnen toepassen
	Uint32 colorkey = SDL_MapRGB(s->format, r, g, b);
	SDL_SetColorKey(s, SDL_SRCCOLORKEY, colorkey);
}

int initSDL(void)
{
	//SDL initialiseren
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
		//printf("failed to initialise SDL...\nError: %s\n", SDL_GetError());
		return 0;
	}
	atexit(SDL_Quit);

	//het scherm zelf maken met 32-bit diepte en DOUBLEBUFFERD
	screen = SDL_SetVideoMode(SW, SH, 32, SDL_DOUBLEBUF);
	if(screen == NULL){
		//printf("failed to set up screen...\nError: %s\n", SDL_GetError());
		return 0;
	}
	
	SDL_EnableUNICODE(SDL_ENABLE); //het gebruik van unicode voor het indentificeren van keyboardstrokes aanzetten

	//de SDL_ttf module initialiseren
	if(TTF_Init() == -1){
		//printf("failed to load TTF support\n");
		return 0;
	}

	return 1;
}
