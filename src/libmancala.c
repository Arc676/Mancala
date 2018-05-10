
//Copyright (C) 2018  Arc676/Alessandro Vinciguerra <alesvinciguerra@gmail.com>

//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation (version 3).

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "libmancala.h"

void setupBoard(MancalaBoard* board, int startingPebbles, int fast) {
	board->fastMode = fast;
	board->startingPebbles = startingPebbles;
	for (int i = 0; i < 14; i++) {
		if (i == MANCALA_GOAL1 || i == MANCALA_GOAL2) {
			board->board[i] = 0;
		} else {
			board->board[i] = startingPebbles;
		}
	}
}

int getOppositePocket(int pocket) {
	//opposite pocket given by P+2(6-P)=P+12-2P=12-P
	return 12 - pocket;
}

int getDestinationPocket(MancalaBoard* board, int pocket) {
	//destination pocket is N pockets over, where N is the number of pebbles in the source pocket
	//the number of times the opponent's goal is crossed is N/13
	return (pocket + board->board[pocket] + board->board[pocket] / 13) % 14;
}

int getDistanceBetween(int p1, int p2) {
	return (p1 - p2 + 14) % 14;
}

int gameIsOver(MancalaBoard* board) {
	//in fast mode, if either player possesses more than half the pebbles, that player wins and the game ends
	if (board->fastMode) {
		if (board->board[MANCALA_GOAL1] >= 6 * board->startingPebbles) {
			return P1_WINS | FAST_WIN;
		} else if (board->board[MANCALA_GOAL2] >= 6 * board->startingPebbles) {
			return P2_WINS | FAST_WIN;
		}
	}
	int p1over = 1, p2over = 1, winner = 0;
	//has either player run out of pebbles on their side?
	for (int i = 0; i < MANCALA_GOAL1; i++) {
		if (board->board[i] != 0) {
			p1over = 0;
			break;
		}
	}
	for (int i = MANCALA_GOAL1 + 1; i < MANCALA_GOAL2; i++) {
		if (board->board[i] != 0) {
			p2over = 0;
			break;
		}
	}
	//move remaining pebbles into players' goal pockets
	if (p2over) {
		for (int i = 0; i < MANCALA_GOAL1; i++) {
			board->board[MANCALA_GOAL1] += board->board[i];
			board->board[i] = 0;
		}
	}
	if (p1over) {
		for (int i = MANCALA_GOAL1 + 1; i < MANCALA_GOAL2; i++) {
			board->board[MANCALA_GOAL2] += board->board[i];
			board->board[i] = 0;
		}
	}
	//if either player has run out of pebbles, game ends
	if (p1over || p2over) {
		int result = P1_WINS | P2_WINS;
		if (board->board[MANCALA_GOAL1] > board->board[MANCALA_GOAL2]) {
			result &= !P2_WINS;
		} else if (board->board[MANCALA_GOAL1] < board->board[MANCALA_GOAL2]) {
			result &= !P1_WINS;
		}
		return result;
	}
	return NOT_OVER;
}

int computerPickPocket(MancalaBoard* board) {
	//check if any pockets will yield captures
	int maxCaptured = -1, bestPocket = 0;
	for (int i = MANCALA_GOAL1 + 1; i < MANCALA_GOAL2; i++) {
		//pocket cannot result in a capture because the last pebble passes the same pocket twice
		//	or because there are no pebbles
		if (board->board[i] > 13 || board->board[i] == 0) {
			continue;
		}
		int newpocket = getDestinationPocket(board, i);
		//pocket cannot result in a capture because it lands on the player's side
		if (newpocket < MANCALA_GOAL1) {
			continue;
		}
		if (board->board[newpocket] == 0) {
			int capture = board->board[getOppositePocket(newpocket)];
			if (capture > maxCaptured) {
				maxCaptured = capture;
				bestPocket = i;
			}
		}
	}
	//if pebbles can be captured, capture as many possible
	if (maxCaptured > 0) {
		return bestPocket;
	}
	//check if any pockets will yield an extra turn, starting from the pocket closest to the goal pocket
	for (int i = MANCALA_GOAL2 - 1; i > MANCALA_GOAL1; i--) {
		if (getDestinationPocket(board, i) == MANCALA_GOAL2) {
			return i;
		}
	}
	//check if opponent can capture any pebbles
	int maxLoss = 0;
	int maxLossPocket = 0;
	for (int i = 0; i < MANCALA_GOAL1; i++) {
		//passes same pocket twice or there are no pebbles
		if (board->board[i] > 13 || board->board[i] == 0) {
			continue;
		}
		int newpocket = getDestinationPocket(board, i);
		//pebble lands in goal pocket or on computer's side
		if (newpocket >= MANCALA_GOAL1) {
			continue;
		}
		if (board->board[newpocket] == 0) {
			int loss = board->board[getOppositePocket(newpocket)];
			if (loss > maxLoss) {
				maxLoss = loss;
				maxLossPocket = getOppositePocket(newpocket);
			}
		}
	}
	//prevent opponent from capturing pebbles by moving them
	if (maxLoss > 0) {
		return maxLossPocket;
	}
	//check if opponent can get an extra turn
	int opponentNewTurns[6];
	int lastNewTurnFound = 0;
	for (int i = 0; i < MANCALA_GOAL1; i++) {
		if (getDestinationPocket(board, i) == MANCALA_GOAL1) {
			opponentNewTurns[lastNewTurnFound++] = i;
		}
	}
	for (int i = lastNewTurnFound - 1; i >= 0; i--) {
		for (int j = MANCALA_GOAL1 + 1; j < MANCALA_GOAL2; j++) {
			if (board->board[j] >= getDistanceBetween(i, j)) {
				return j;
			}
		}
	}
	//by default (i.e. if there are no better moves), pick the pocket farthest from the goal
	for (int i = MANCALA_GOAL1 + 1; i < MANCALA_GOAL2; i++) {
		if (board->board[i] > 0) {
			return i;
		}
	}
	return -1;
}

int move(MancalaBoard* board, int pocket, int goal) {
	//obtain pebble count, clear pocket
	int pebbles = board->board[pocket];
	board->board[pocket] = 0;
	int skippedPockets = 0;

	//pebbles should not end up in this pocket
	int opponentGoal = (goal + 7) % 14;

	//pebbles are dropped in the next pocket, so start from 1
	int newpocket = 0;
	for (int i = 1; i < pebbles + 1;) {
		//make sure the pebble doesn't land in the opponent's goal pocket
		newpocket = (pocket + i + skippedPockets) % 14;
		if (newpocket == opponentGoal) {
			skippedPockets++;
			continue;
		}
		//add pebble, then move on
		board->board[newpocket]++;
		i++;
	}
	int result = MOVE_NO_EFFECT;
	//check if last pebble landed in goal pocket
	if (newpocket == goal) {
		result = MOVE_EXTRA_TURN;
	} else {
		//check if last pebble landed in empty pocket in current player's side and not the goal pocket
		int oppositePocket = getOppositePocket(newpocket);
		if (board->board[newpocket] == 1 && board->board[oppositePocket] > 0 && newpocket < goal && newpocket >= opponentGoal % 13) {
			board->board[goal] += board->board[oppositePocket] + 1;
			board->board[oppositePocket] = 0;
			board->board[newpocket] = 0;
			result = MOVE_CAPTURE;
		}
	}
	return result;
}

void computerMove(MancalaBoard* board, ComputerMoveData* data) {
	int computerPocket = computerPickPocket(board);
	if (computerPocket < MANCALA_GOAL1 + 1 || computerPocket >= MANCALA_GOAL2) {
		data->result = MOVE_FAILED;
		return;
	}
	data->chosenPocket = computerPocket - 6;
	data->result = move(board, computerPocket, MANCALA_GOAL2);
}

