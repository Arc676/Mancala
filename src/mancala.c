//Copyright (C) 2015-2018  Arc676/Alessandro Vinciguerra <alesvinciguerra@gmail.com>

//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation (version 3).

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int p1First = 1;
int _2player = 0;
int fastMode = 0;
int startingPebbles = 4;
int alwaysPrintBoard = 0;

void printBoard(){
	printf("         6     5     4     3     2     1\n");
	printf("-------------------------------------------------\n");
	printf("|%5d|%5d|%5d|%5d|%5d|%5d|%5d|%5s|\n", board[13], board[12], board[11], board[10], board[9], board[8], board[7], "");
	printf("       -----------------------------------\n");
	printf("|%5s|%5d|%5d|%5d|%5d|%5d|%5d|%5d|\n", "", board[0], board[1], board[2], board[3], board[4], board[5], board[6]);
	printf("-------------------------------------------------\n");
	printf("         1     2     3     4     5     6\n");
}

void help(){
	printf("Help\nshow - show board\nhelp/? - show this list\n(pocket number) - select a pocket\nquit - quit game\n");
}

void playGame(){
	int currentPlayer = 2;
	if (p1First || !_2player){
		currentPlayer = 1;
	}
	if (!p1First && !_2player){
		computerMove();
	}
	char * cmd = malloc(255);
	while (!gameIsOver()){
		printf("%d> ", currentPlayer);
		fgets(cmd, 255, stdin);
		if (!strncmp(cmd, "show", 4)){
			printBoard();
		}else if (!strncmp(cmd, "quit", 4)){
			return;
		}else if (!strncmp(cmd, "help", 4) || cmd[0] == '?'){
			help();
		}else{
			if (cmd[0] == '\n'){
				continue;
			}
			int position = (int)strtol(cmd, (char**)NULL, 10);
			if (position < 1 || position > 6){
				printf("Invalid input\n");
				continue;
			}
			position--;
			if (currentPlayer == 2){
				position += 7;
			}
			if (board[position] == 0){
				printf("No pebbles in this pocket\nUse 'show' to see the board\n");
				continue;
			}
			int result = move(position, goals[currentPlayer - 1]);
			if (result == MOVE_EXTRA_TURN){
				printf("Extra turn!\n");
				continue;
			}else if (result == MOVE_CAPTURE){
				printf("Capture!\n");
			}
			if (gameIsOver()){
				break;
			}
			if (currentPlayer == 1 && _2player){
				currentPlayer = 2;
			}else if (currentPlayer == 2){
				currentPlayer = 1;
			}else{
				computerMove();
			}
		}
		memset(cmd, 0, 255);
	}
	free(cmd);
	if (!alwaysPrintBoard){
		printBoard();
	}
}

int main(int argc, char * argv[]){
	printf("Mancala  Copyright (C) 2015-2018  Arc676/Alessandro Vinciguerra\n\
This program comes with ABSOLUTELY NO WARRANTY.\n\
This is free software, and you are welcome to redistribute it\n\
under the conditions of GPLv3; see LICENSE.txt for details\n");
	int pebblesChanged = 0;
	printf("Setting up game...\n");
	for (int i = 1; i < argc;){
		if (argv[i][0] != '-'){
			fprintf(stderr, "Invalid flag: %s\n" , argv[i]);
			return FLAG_ERROR;
		}
		if (argv[i][1] == 't'){
			_2player = 1;
			printf("Two player mode enabled\n");
		}else if (argv[i][1] == 's'){
			p1First = 0;
			printf("Second player starts\n");
		}else if (argv[i][1] == 'b'){
			alwaysPrintBoard = 1;
			printf("Will always print board after each move\n");
		}else if (argv[i][1] == 'f'){
			fastMode = 1;
			printf("Fast mode enabled\n");
		}else if (argv[i][1] == 'p'){
			if (i + 1 >= argc){
				fprintf(stderr, "Invalid flag count\n");
				return FLAG_ERROR;
			}
			startingPebbles = abs((int)strtol(argv[i + 1], (char**)NULL, 10));
			if (startingPebbles > 1000){
				fprintf(stderr, "Starting pebble count too large\n");
				return LIMITATION_ERROR;
			}
			printf("Starting pebbles: %d\n", startingPebbles);
			pebblesChanged = 1;
			i++;
		}else{
			fprintf(stderr, "Invalid option: %s\n", argv[i]);
			return FLAG_ERROR;
		}
		i++;
	}
	if (!pebblesChanged){
		printf("Starting pebbles: %d\n", startingPebbles);
	}
	setupBoard();
	printBoard();
	playGame();
	return NO_ERROR;
}
