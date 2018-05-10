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

#ifndef LIBMANCALA_H
#define LIBMANCALA_H

#define MANCALA_GOAL1	6
#define MANCALA_GOAL2	13

/**
 * Defines error types throwable by algorithms
 */
enum Errors {
	NO_ERROR = 0,
	FLAG_ERROR = 1,
	LIMITATION_ERROR = 2,
};

/**
 * Defines possible results of game moves
 */
enum MoveResult {
	MOVE_NO_EFFECT = 0,
	MOVE_EXTRA_TURN = 1,
	MOVE_CAPTURE = 2,
	MOVE_FAILED = 3
};

/**
 * Represents a board state for a single game
 */
typedef struct MancalaBoard {
	int board[14]; //6 * 2 pockets + 2 goals = 14
	int fastMode;
	int startingPebbles;
} MancalaBoard;

/**
 * Stores information about the computer's move
 */
typedef struct ComputerMoveData {
	int chosenPocket;
	int result;
} ComputerMoveData;

/**
 * Encodes the game result or state as a bitwise combination of the following:
 * Player 1 wins
 * Player 2 wins
 * The game was won due to fast-mode
 * The game is not over
 */
enum GameResult {
	NOT_OVER  = 0b0000,
	P1_WINS   = 0b0001,
	P2_WINS   = 0b0010,
	FAST_WIN  = 0b0100
};

/**
 * Sets up the initial board state given the desired number
 * of starting pebbles in each pocket
 * @param board Pointer to board state to setup
 * @param startingPebbles Number of pebbles to start in each pocket
 * @param fast Whether fast mode should be enabled
 */
void setupBoard(MancalaBoard* board, int startingPebbles, int fast);

/**
 * Determines the index of the pocket directly opposite
 * another pocket given its index
 * @param pocket Index of relevant pocket
 * @return Index of opposite pocket
 */
int getOppositePocket(int pocket);

/**
 * Determines the index of the pocket in which the
 * final pebble in the given pocket will land
 * @param board Pointer to board state
 * @param pocket Index of starting pocket
 * @return Index of the destination pocket
 */
int getDestinationPocket(MancalaBoard* board, int pocket);

/**
 * Determines the number of pockets between two given
 * pockets following normal Mancala pocket placement rules
 * and therefore the minimum number of pebbles required in
 * the first pocket to reach the second pocket
 * @param p1 Index of first pocket
 * @param p2 Index of second pocket
 * @return Minimum number of pebbles to travel between given pockets
 */
int getDistanceBetween(int p1, int p2);

/**
 * Determines whether the game is over based on the board state
 * @param board Pointer to board state
 * @return Whether the game has ended
 */
int gameIsOver(MancalaBoard* board);

/**
 * Determines the move the computer should take based on the
 * board state
 * @param board Pointer to board state
 * @return The index of the pocket to be chosen
 */
int computerPickPocket(MancalaBoard* board);

/**
 * Computes the consequences and new board state given an index
 * as the chosen move
 * @param board Pointer to board state
 * @param pocket The index of the chosen pocket
 * @param goal The index of the goal of the moving player
 * @return The result of the move
 */
int move(MancalaBoard* board, int pocket, int goal);

/**
 * Computes the computer's turn given the board state
 * @param board Pointer to board state
 * @param data Pointer to struct in which to store data regarding the computer's move
 */
void computerMove(MancalaBoard* board, ComputerMoveData* data);

#endif
