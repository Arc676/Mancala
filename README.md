# Mancala
Command line Mancala game and library written in C

The game backend is implemented in the Mancala library and the command line program provides a frontend. Consult the header file for details regarding the available function calls. Where expressions may seem unclear, the implementation is also commented.

Mancala is a strategy game in which pebbles are moved around a board to score points. Game rules can be found in the man page in the `doc` folder.

### License

Mancala frontend and library available under GPLv3.

### Contributing

If you would like to contribute to the code base, please add a copyright line to the opening comments of any source files you modify. Please try to maintain the same coding conventions.

### Implementing a custom frontend

To use the library to implement a frontend, it must be kept in mind that the `computerMove(MancalaBoard*, ComputerMoveData*)` method should be called repeatedly should the computer obtain an extra turn. Because the frontend may need to display animations or otherwise react to the end of the computer's turn, a recursive algorithm is *not* implemented. The frontend must handle re-calling the method after displaying appropriate feedback to the user. In the command line frontend, this is implemented in the `performComputerMove()` method.

Please consult the provided frontend for more details regarding frontend implementation.
