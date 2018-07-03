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

#include "libmancala.h"

MancalaBoard* board;

int alwaysPrintBoard = 0;
int p1First = 1;
int _2player = 0;

void printBoard() {
	int* b = board->board;
	printf("         6     5     4     3     2     1\n");
	printf("-------------------------------------------------\n");
	printf("|%5d|%5d|%5d|%5d|%5d|%5d|%5d|%5s|\n", b[13], b[12], b[11], b[10], b[9], b[8], b[7], "");
	printf("       -----------------------------------\n");
	printf("|%5s|%5d|%5d|%5d|%5d|%5d|%5d|%5d|\n", "", b[0], b[1], b[2], b[3], b[4], b[5], b[6]);
	printf("-------------------------------------------------\n");
	printf("         1     2     3     4     5     6\n");
}

void help() {
	printf("Help\nshow - show board\nhelp/? - show this list\n(pocket number) - select a pocket\nquit - quit game\n");
}

void performComputerMove() {
	ComputerMoveData* data = (ComputerMoveData*)malloc(sizeof(ComputerMoveData));
	computerMove(board, data);
	if (data->result == MOVE_FAILED) {
		printf("Computer cannot move\n");
		return;
	}
	printf("Computer chose pocket %d\n", data->chosenPocket);
	if (alwaysPrintBoard) {
		printBoard();
	}
	if (data->result == MOVE_EXTRA_TURN) {
		printf("Computer gets an extra turn!\n");
		performComputerMove();
	} else if (data->result == MOVE_CAPTURE) {
		printf("Computer captured your pebble(s)!\n");
	}
	free(data);
}

int gameOver() {
	int result = gameIsOver(board);
	if (result == NOT_OVER) {
		return 0;
	}
	if (result & FAST_WIN) {
		if (result & P1_WINS) {
			if (_2player) {
				printf("Player 1 has more than half of the pebbles\n");
			} else {
				printf("Human has more than half of the pebbles\n");
			}
		} else {
			if (_2player) {
				printf("Player 2 has more than half of the pebbles\n");
			} else {
				printf("Computer has more than half of the pebbles\n");
			}
		}
	} else {
		if (result == (P1_WINS | P2_WINS)) {
			printf("It's a tie!\n");
		} else {
			if (result == P1_WINS) {
				if (_2player) {
					printf("Player 1 wins!\n");
				} else {
					printf("Human wins!\n");
				}
			} else {
				if (_2player) {
					printf("Player 2 wins!\n");
				} else {
					printf("Computer wins!\n");
				}
			}
		}
	}
	return 1;
}

void playGame() {
	int currentPlayer = 2;
	if (p1First || !_2player) {
		currentPlayer = 1;
	}
	if (!p1First && !_2player) {
		performComputerMove();
	}
	char * cmd = malloc(255);
	while (!gameOver()) {
		printf("%d> ", currentPlayer);
		fgets(cmd, 255, stdin);
		if (!strncmp(cmd, "show", 4)) {
			printBoard();
		} else if (!strncmp(cmd, "quit", 4)) {
			return;
		} else if (!strncmp(cmd, "help", 4) || cmd[0] == '?') {
			help();
		} else {
			if (cmd[0] == '\n') {
				continue;
			}
			int position = (int)strtol(cmd, (char**)NULL, 10);
			if (position < 1 || position > 6) {
				printf("Invalid input\n");
				continue;
			}
			position--;
			if (currentPlayer == 2) {
				position += 7;
			}
			if (board->board[position] == 0) {
				printf("No pebbles in this pocket\nUse 'show' to see the board\n");
				continue;
			}
			int result = movePocket(board, position, currentPlayer == 1 ? MANCALA_GOAL1 : MANCALA_GOAL2);
			if (alwaysPrintBoard) {
				printBoard();
			}
			if (result == MOVE_EXTRA_TURN) {
				printf("Extra turn!\n");
				if (gameIsOver(board) == NOT_OVER) {
					continue;
				}
			} else if (result == MOVE_CAPTURE) {
				printf("Capture!\n");
			}
			if (gameOver()) {
				break;
			}
			if (currentPlayer == 1 && _2player) {
				currentPlayer = 2;
			} else if (currentPlayer == 2) {
				currentPlayer = 1;
			} else {
				performComputerMove();
			}
		}
		memset(cmd, 0, 255);
	}
	free(cmd);
	if (!alwaysPrintBoard) {
		printBoard();
	}
}

int main(int argc, char * argv[]) {
	printf("Mancala  Copyright (C) 2015-2018  Arc676/Alessandro Vinciguerra\n\
This program comes with ABSOLUTELY NO WARRANTY.\n\
This is free software, and you are welcome to redistribute it\n\
under the conditions of GPLv3; see LICENSE for details\n");

	int pebblesChanged = 0;

	int fastMode = 0;
	int startingPebbles = 4;

	printf("Setting up game...\n");
	for (int i = 1; i < argc;) {
		if (argv[i][0] != '-') {
			fprintf(stderr, "Invalid flag: %s\n" , argv[i]);
			return FLAG_ERROR;
		}
		if (argv[i][1] == 't') {
			_2player = 1;
			printf("Two player mode enabled\n");
		} else if (argv[i][1] == 's') {
			p1First = 0;
			printf("Second player starts\n");
		} else if (argv[i][1] == 'b') {
			alwaysPrintBoard = 1;
			printf("Will always print board after each move\n");
		} else if (argv[i][1] == 'f') {
			fastMode = 1;
			printf("Fast mode enabled\n");
		} else if (argv[i][1] == 'p') {
			if (i + 1 >= argc) {
				fprintf(stderr, "Invalid flag count\n");
				return FLAG_ERROR;
			}
			startingPebbles = abs((int)strtol(argv[i + 1], (char**)NULL, 10));
			if (startingPebbles > 1000) {
				fprintf(stderr, "Starting pebble count too large\n");
				return LIMITATION_ERROR;
			}
			printf("Starting pebbles: %d\n", startingPebbles);
			pebblesChanged = 1;
			i++;
		} else {
			fprintf(stderr, "Invalid option: %s\n", argv[i]);
			return FLAG_ERROR;
		}
		i++;
	}
	if (!pebblesChanged) {
		printf("Starting pebbles: %d\n", startingPebbles);
	}
	board = (MancalaBoard*)malloc(sizeof(MancalaBoard));
	setupBoard(board, startingPebbles, fastMode);
	printBoard();
	playGame();
	free(board);
	return NO_ERROR;
}
