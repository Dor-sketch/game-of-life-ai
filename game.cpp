#include "config.hpp"
#include <iostream>

/*
 * Updates the board according to the rules of the game of life
 * @param board - the board to update (pass by reference)
 * @param boardSize - the size of the board
 */
void gameOfLife(int board[BOARD_SIZE][BOARD_SIZE], int boardSize) {
    int dirX[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dirY[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    int rows = boardSize;
    int cols = boardSize;

    static int paddingRows[BOARD_SIZE + 4][BOARD_SIZE + 4];

    // check if was called for the same board - dont need to copy then
    for (int row = 0; row < rows; row++) {
        if (memcmp(paddingRows[row + 2] + 2, board[row], cols * sizeof(int)) !=
            0) {
            // If any row is different, copy the board to the padded board and
            for (int r = 0; r < rows; r++) {
                memcpy(paddingRows[r + 2] + 2, board[r], cols * sizeof(int));
            }
            // reset padding rows
            for (int r = 0; r < 2; r++) {
                for (int c = 0; c < cols + 4; c++) {
                    paddingRows[r][c] = 0;
                    paddingRows[rows + 2 + r][c] = 0;
                }
            }
            break;
        }
    }

    // Perform actions on the entire padded board
    for (int row = 0; row < rows + 4; row++) {
        for (int col = 0; col < cols + 4; col++) {
            int liveNeighbors = 0;

            for (int d = 0; d < 8; d++) {
                int newRow = row + dirX[d];
                int newCol = col + dirY[d];

                if (newRow >= 0 && newRow < rows + 4 && newCol >= 0 &&
                    newCol < cols + 4) {
                    liveNeighbors += paddingRows[newRow][newCol] & 1;
                }
            }

            if ((paddingRows[row][col] & 1) == 1) {
                if (liveNeighbors == 2 || liveNeighbors == 3) {
                    paddingRows[row][col] |= 2; // Set the second bit
                }
            } else {
                if (liveNeighbors == 3) {
                    paddingRows[row][col] |= 2; // Set the second bit
                }
            }
        }
    }

    // Shift the second bit to the first bit and copy the padded board to the
    // board
    for (int row = 0; row < rows + 4; row++) {
        for (int col = 0; col < cols + 4; col++) {
            paddingRows[row][col] >>=
                1; // Shift the second bit to the first bit
            if (row >= 2 && row < rows + 2 && col >= 2 && col < cols + 2) {
                board[row - 2][col - 2] = paddingRows[row][col] & 1;
            }
        }
    }
}