#############################
# Vier op een Rij makefile
#
#	Connect 4 is written
#		by
#	   Ben & Kristof
##############################

cc= gcc
ldflags=-lSDL -lSDL_image -lSDL_ttf
files=playfield.c vier.c highscore.c ai/ai.c
output=vier

all:
	${cc} ${files} -o ${output} ${ldflags} 
