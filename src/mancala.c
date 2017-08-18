//Copyright (C) 2015-2017  Arc676/Alessandro Vinciguerra <alesvinciguerra@gmail.com>

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

#define GOAL1	goals[0]
#define GOAL2	goals[1]

typedef enum errors{
	NO_ERROR = 0,
	FLAG_ERROR = 1,
	LIMITATION_ERROR = 2,
}errors;

typedef enum moveResults{
	MOVE_NO_EFFECT = 0,
	MOVE_EXTRA_TURN = 1,
	MOVE_CAPTURE = 2
}moveResults;

int _2player = 0;
int fastMode = 0;
int p1First = 1;
int startingPebbles = 4;
int alwaysPrintBoard = 0;

int board[6 * 2 + 2];
const int goals[] = {6, 13};

void printBoard(){
	printf("         6     5     4     3     2     1\n");
	printf("-------------------------------------------------\n");
	printf("|%5d|%5d|%5d|%5d|%5d|%5d|%5d|%5s|\n", board[13], board[12], board[11], board[10], board[9], board[8], board[7], "");
	printf("       -----------------------------------\n");
	printf("|%5s|%5d|%5d|%5d|%5d|%5d|%5d|%5d|\n", "", board[0], board[1], board[2], board[3], board[4], board[5], board[6]);
	printf("-------------------------------------------------\n");
	printf("         1     2     3     4     5     6\n");
}

void setupBoard(){
	printf("Setting up board...");
	for (int i = 0; i < 14; i++){
		if (i == GOAL1 || i == GOAL2){
			board[i] = 0;
		}else{
			board[i] = startingPebbles;
		}
	}
	printf("Done\n");
}

int getOppositePocket(int pocket){
	//opposite pocket given by P+2(6-P)=P+12-2P=12-P
	return 12 - pocket;
}

int getDestinationPocket(int pocket){
	//destination pocket is N pockets over, where N is the number of pebbles in the source pocket
	//the number of times the opponent's goal is crossed is N/13
	return (pocket + board[pocket] + board[pocket] / 13) % 14;
}

int getDistanceBetween(int a, int b){
	return (a - b + 14) % 14;
}

int gameIsOver(){
	//in fast mode, if either player possesses more than half the pebbles, that player wins and the game ends
	if (fastMode){
		if (board[GOAL1] >= 6 * startingPebbles){
			if (_2player){
				printf("Player 1 has 50%% of the pebbles or more\n");
			}else{
				printf("Human has 50%% of the pebbles or more\n");
			}
			return 1;
		}else if (board[GOAL2] >= 6 * startingPebbles){
			if (_2player){
				printf("Player 2 has 50%% of the pebbles or more\n");
			}else{
				printf("Computer has 50%% of the pebbles or more\n");
			}
			return 1;
		}
	}
	int p1over = 1, p2over = 1, winner = 0;
	//has either player run out of pebbles on their side?
	for (int i = 0; i < goals[0]; i++){
		if (board[i] != 0){
			p1over = 0;
			break;
		}
	}
	for (int i = goals[0] + 1; i < goals[1]; i++){
		if (board[i] != 0){
			p2over = 0;
			break;
		}
	}
	//move remaining pebbles into players' goal pockets
	if (p2over){
		for (int i = 0; i < goals[0]; i++){
			board[GOAL1] += board[i];
			board[i] = 0;
		}
	}
	if (p1over){
		for (int i = goals[0] + 1; i < goals[1]; i++){
			board[GOAL2] += board[i];
			board[i] = 0;
		}
	}
	//if either player has run out of pebbles, game ends
	if (p1over || p2over){
		if (board[GOAL1] > board[GOAL2]){
			if (_2player){
				printf("Player 1 wins!\n");
			}else{
				printf("Human wins!\n");
			}
		}else if (board[GOAL1] < board[GOAL2]){
			if (_2player){
				printf("Player 2 wins!\n");
			}else{
				printf("Computer wins!\n");
			}
		}else{
			printf("It's a tie!\n");
		}
		return 1;
	}
	return 0;
}

int computerPickPocket(){
	//check if any pockets will yield captures
	int maxCaptured = -1, bestPocket = 0;
	for (int i = goals[0] + 1; i < goals[1]; i++){
		//pocket cannot result in a capture because the last pebble passes the same pocket twice
		//	or because there are no pebbles
		if (board[i] > 13 || board[i] == 0){
			continue;
		}
		int newpocket = getDestinationPocket(i);
		//pocket cannot result in a capture because it lands on the player's side
		if (newpocket < GOAL1){
			continue;
		}
		if (board[newpocket] == 0){
			int capture = board[getOppositePocket(newpocket)];
			if (capture > maxCaptured){
				maxCaptured = capture;
				bestPocket = i;
			}
		}
	}
	//if pebbles can be captured, capture as many possible
	if (maxCaptured > 0){
		return bestPocket;
	}
	//check if any pockets will yield an extra turn, starting from the pocket closest to the goal pocket
	for (int i = goals[1] - 1; i > goals[0]; i--){
		if (getDestinationPocket(i) == GOAL2){
			return i;
		}
	}
	//check if opponent can capture any pebbles
	int maxLoss = 0;
	int maxLossPocket = 0;
	for (int i = 0; i < GOAL1; i++){
		//passes same pocket twice or there are no pebbles
		if (board[i] > 13 || board[i] == 0){
			continue;
		}
		int newpocket = getDestinationPocket(i);
		//pebble lands in goal pocket or on computer's side
		if (newpocket >= GOAL1){
			continue;
		}
		if (board[newpocket] == 0){
			int loss = board[getOppositePocket(newpocket)];
			if (loss > maxLoss){
				maxLoss = loss;
				maxLossPocket = getOppositePocket(newpocket);
			}
		}
	}
	//prevent opponent from capturing pebbles by moving them
	if (maxLoss > 0){
		return maxLossPocket;
	}
	//check if opponent can get an extra turn
	int opponentNewTurns[6];
	int lastNewTurnFound = 0;
	for (int i = 0; i < goals[0]; i++){
		if (getDestinationPocket(i) == GOAL1){
			opponentNewTurns[lastNewTurnFound++] = i;
		}
	}
	for (int i = lastNewTurnFound - 1; i >= 0; i--){
		for (int j = goals[0] + 1; j < goals[1]; j++){
			if (board[j] >= getDistanceBetween(i, j)){
				return j;
			}
		}
	}
	//by default (i.e. if there are no better moves), pick the pocket farthest from the goal
	for (int i = goals[0] + 1; i < goals[1]; i++){
		if (board[i] > 0){
			return i;
		}
	}
	return -1;
}

int move(int pocket, int goal){
	//obtain pebble count, clear pocket
	int pebbles = board[pocket];
	board[pocket] = 0;
	int skippedPockets = 0;

	//pebbles should not end up in this pocket
	int opponentGoal = (goal + 7) % 14;

	//pebbles are dropped in the next pocket, so start from 1
	int newpocket = 0;
	for (int i = 1; i < pebbles + 1;){
		//make sure the pebble doesn't land in the opponent's goal pocket
		newpocket = (pocket + i + skippedPockets) % 14;
		if (newpocket == opponentGoal){
			skippedPockets++;
			continue;
		}
		//add pebble, then move on
		board[newpocket]++;
		i++;
	}
	int result = MOVE_NO_EFFECT;
	//check if last pebble landed in goal pocket
	if (newpocket == goal){
		result = MOVE_EXTRA_TURN;
	}else{
		//check if last pebble landed in empty pocket in current player's side and not the goal pocket
		int oppositePocket = getOppositePocket(newpocket);
		if (board[newpocket] == 1 && board[oppositePocket] > 0 && newpocket < goal && newpocket >= opponentGoal % 13){
			board[goal] += board[oppositePocket] + 1;
			board[oppositePocket] = 0;
			board[newpocket] = 0;
			result = MOVE_CAPTURE;
		}
	}
	if (alwaysPrintBoard){
		printBoard();
	}
	return result;
}

void computerMove(){
	int computerPocket = computerPickPocket();
	if (computerPocket < goals[0] + 1 || computerPocket >= goals[1]){
		printf("Computer cannot move\n");
		return;
	}
	printf("Computer chose pocket %d\n", (computerPocket - 6));
	int result = move(computerPocket, GOAL2);
	if (result == MOVE_EXTRA_TURN){
		printf("Computer gets an extra turn!\n");
		computerMove();
	}else if (result == MOVE_CAPTURE){
		printf("Computer captured your pebble(s)!\n");
	}
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
	printf("Mancala  Copyright (C) 2015-2017  Arc676/Alessandro Vinciguerra\n\
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
