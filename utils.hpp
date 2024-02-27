#pragma once

#include "config.hpp"


int howManyAlive(int board[BOARD_SIZE][BOARD_SIZE]);

void copyBoard(int board[BOARD_SIZE][BOARD_SIZE],
               int newBoard[BOARD_SIZE][BOARD_SIZE]);


void testCrossover();
void testSelection();