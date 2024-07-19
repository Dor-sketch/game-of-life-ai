#include "chromosome.hpp"
#include <cstdlib>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept> // for std::out_of_range
#include <string>
#include <iostream>
#include "utils.hpp"
#include <unordered_set>

extern void gameOfLife(int board[BOARD_SIZE][BOARD_SIZE], int boardColSize);

Chromosome::Chromosome() : score(0), fitness(0) {
	memset(board, 0, sizeof(board[0][0]) * BOARD_SIZE * BOARD_SIZE);

	// Define shape functions
	auto square_s = [](int boardSize, int x, int y) {
		return x > boardSize * 3 / 8 && x < boardSize * 5 / 8 &&
			   y > boardSize * 3 / 8 && y < boardSize * 5 / 8;
	};
	auto circle = [](int boardSize, int x, int y) {
		int dx = x - boardSize / 2;
		int dy = y - boardSize / 2;
		int radius = boardSize / 7;
		return dx * dx + dy * dy <= radius * radius;
	};
	auto diamond = [](int boardSize, int x, int y) {
		int center = boardSize / 2;
		int dx = std::abs(x - center);
		int dy = std::abs(y - center);
		return dx + dy <= boardSize / 8;
	};
	auto triangle = [](int boardSize, int x, int y) {
		int h = boardSize / 11;		// quarter the board size
		int center = boardSize / 2; // center of the board
		return y >= center - h && y <= center + h &&
			   abs(x - center) <= y - center + h;
	};

	// Create a map of shape functions
	std::map<std::string, std::function<bool(int, int, int)>> shapes = {
		{"square_s", square_s}, {"circle", circle}, {"diamond", diamond},
		{"triangle", triangle}};

	// Select a random shape function
	auto shapeEntry = std::next(shapes.begin(), arc4random_uniform(shapes.size()));

	// Adjust aliveProbability based on the shape
	double adjustedAliveProbability = aliveProbability;
	if (shapeEntry->first == "circle") {
		adjustedAliveProbability *= 0.4; // Increase for circle
	} else if (shapeEntry->first == "square_s") {
		adjustedAliveProbability *= 1.4; // Decrease for square_s
	} else if (shapeEntry->first == "triangle") {
		adjustedAliveProbability *= 1.8; // Increase for triangle
	}
	// Create random configuration in the middle of the board
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (shapeEntry->second(BOARD_SIZE, i, j) &&
				((arc4random_uniform(UINT32_MAX) / (double)UINT32_MAX) <
				 adjustedAliveProbability)) {
				board[i][j] = 1;
			}
		}
	}
}



Chromosome::Chromosome(int crossoverPointX, std::shared_ptr<Chromosome> parent1,
					   std::shared_ptr<Chromosome> parent2)
	: score(0), fitness(0) {

	memset(board, 0, sizeof(board[0][0]) * BOARD_SIZE * BOARD_SIZE);

	if (crossoverPointX < 0 || crossoverPointX >= BOARD_SIZE) {
		throw std::out_of_range("crossover point is out of bounds");
	}

	// Perform crossover
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			if (i < crossoverPointX) {
				board[i][j] = parent1->board[i][j];
			} else {
				board[i][j] = parent2->board[i][j];
			}
		}
	}
}

Chromosome::Chromosome(std::shared_ptr<Chromosome> parent1,
					   std::shared_ptr<Chromosome> parent2)
	: score(0), fitness(0) {

	// init 0 board
	memset(board, 0, sizeof(board[0][0]) * BOARD_SIZE * BOARD_SIZE);

	// Perform uniform crossover
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			if (arc4random_uniform(2) == 0) {
				board[i][j] = parent1->board[i][j];
			} else {
				board[i][j] = parent2->board[i][j];
			}
		}
	}
}

void Chromosome::printBoard() {
	int firstRowWithAliveCell = -1;
	int lastRowWithAliveCell = -1;

	// Find the first and last rows with alive cells
	for (int row = 0; row < BOARD_SIZE; row++) {
		for (int col = 0; col < BOARD_SIZE; col++) {
			if (board[row][col] == 1) {
				if (firstRowWithAliveCell == -1) {
					firstRowWithAliveCell = row;
				}
				lastRowWithAliveCell = row;
				break;
			}
		}
	}

	// Print only the rows with alive cells
	for (int row = firstRowWithAliveCell; row <= lastRowWithAliveCell; row++) {
		for (int col = 0; col < BOARD_SIZE; col++) {
			if (board[row][col] == 1) {
				std::cout << square;
			} else {
				std::cout << " ";
			}
		}
		std::cout << std::endl;
	}
}

std::string Chromosome::printBoardToFile() {
	std::string boardStr;
	for (int row = 0; row < BOARD_SIZE; row++) {
		for (int col = 0; col < BOARD_SIZE; col++) {
			boardStr += std::to_string(board[row][col]);
		}
		boardStr += "\n";
	}
	return boardStr;
}

bool compareBoard(int board1[BOARD_SIZE][BOARD_SIZE],
				  int board2[BOARD_SIZE][BOARD_SIZE]) {
	// compare two boards
	for (int row = 0; row < BOARD_SIZE; row++) {
		for (int col = 0; col < BOARD_SIZE; col++) {
			if (board1[row][col] != board2[row][col]) {
				return false;
			}
		}
	}
	return true;
}
#include <fftw3.h>

#define THRESHOLD 1e-6

bool isShieldOfDavid(int board[BOARD_SIZE][BOARD_SIZE], int scale) {
	// Define the Shield of David pattern
	scale = 1;
	std::vector<std::vector<int>> shieldOfDavid = {
		{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
		{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0},
		{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0},
		{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0},
		{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}
	};

	// Scale the pattern
	std::vector<std::vector<int>> scaledShieldOfDavid(scale * 3, std::vector<int>(scale * 3, 0));
	for (int i = 0; i < scale * 3; ++i) {
		for (int j = 0; j < scale * 3; ++j) {
			scaledShieldOfDavid[i][j] = shieldOfDavid[i / scale][j / scale];
		}
	}

	// Convert the board and the scaled pattern to the frequency domain using FFT
	fftw_complex* boardFreq = fftw_alloc_complex(BOARD_SIZE * BOARD_SIZE);
	fftw_complex* patternFreq = fftw_alloc_complex(scale * 3 * scale * 3);
	fftw_plan boardPlan = fftw_plan_dft_2d(BOARD_SIZE, BOARD_SIZE, boardFreq, boardFreq, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_plan patternPlan = fftw_plan_dft_2d(scale * 3, scale * 3, patternFreq, patternFreq, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(boardPlan);
	fftw_execute(patternPlan);

	// Multiply the frequency-domain representations of the board and the scaled pattern
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
		double temp = boardFreq[i][0];
		boardFreq[i][0] = boardFreq[i][0] * patternFreq[i][0] - boardFreq[i][1] * patternFreq[i][1];
		boardFreq[i][1] = temp * patternFreq[i][1] + boardFreq[i][1] * patternFreq[i][0];
	}

	// Convert the result back to the spatial domain using inverse FFT
	fftw_plan inversePlan = fftw_plan_dft_2d(BOARD_SIZE, BOARD_SIZE, boardFreq, boardFreq, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_execute(inversePlan);

	// Check if the maximum value in the result is greater than a threshold
	double maxVal = 0;
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
		maxVal = std::max(maxVal, std::sqrt(boardFreq[i][0] * boardFreq[i][0] + boardFreq[i][1] * boardFreq[i][1]));
	}

	// Clean up
	fftw_destroy_plan(boardPlan);
	fftw_destroy_plan(patternPlan);
	fftw_destroy_plan(inversePlan);
	fftw_free(boardFreq);
	fftw_free(patternFreq);

	// Return true if the maximum value is greater than the threshold
	return maxVal > THRESHOLD;
}
double getScore(int maxAlive, int startAlive, int stablePeriod, bool shieldOfDavid) {
	// calculate the score for a given board
	if (startAlive == 0) {
		return 0;
	}
	// give more weight to the difference between the start alive and max alive
	// than the number of generations since for all the boards that reach the
	// max alive the number of generations will be the same
	double score = std::max(
		1, maxAlive -
			   static_cast<int>(startAlive * static_cast<int> (BOARD_SIZE / 7)) +
			   stablePeriod);

	// If the Shield of David pattern is found, multiply the score by a factor
	if (shieldOfDavid) {
		score *= 2; // or any other factor you want
	}

	return score;
}

#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
void Chromosome::calculateScore() {
	// Calculate fitness for a given board
	int temp_score = 0, startAlive = 0, curAlive = 0,
		maxAlive = 0, numGenerations = BOARD_SIZE * BOARD_SIZE / 2;

	std::vector<int> lastAliveCells;

	maxAlive = startAlive = curAlive = howManyAlive(board);
	if (startAlive == 0) {
		this->score = 0;
		return;
	}
	// create a copy of the board to avoid changing the original board
	static int tempBoard[BOARD_SIZE][BOARD_SIZE] = {{0}};
	for (int row = 0; row < BOARD_SIZE; row++) {
		memcpy(tempBoard[row], board[row], BOARD_SIZE * sizeof(int));
	}

	// Hash table to store past states
	std::unordered_set<std::string> pastStates;

	// Check for the Shield of David pattern at different scales
	bool shieldOfDavid = false;
	for (int scale = 1; scale <= BOARD_SIZE / 3; ++scale) {
		if (isShieldOfDavid(tempBoard, scale)) {
			shieldOfDavid = true;
			break;
		}
	}

	while (numGenerations > 0 &&
		   curAlive > 2) { // while the board is changing
		// Convert tempBoard to run-length encoded string
		std::string tempBoardStr;
		int runLength = 0;
		int lastCell = tempBoard[0][0];
		for (int i = 0; i < BOARD_SIZE; ++i) {
			for (int j = 0; j < BOARD_SIZE; ++j) {
				if (tempBoard[i][j] == lastCell) {
					runLength++;
				} else {
					tempBoardStr += std::to_string(lastCell) + "#" + std::to_string(runLength) + " ";
					lastCell = tempBoard[i][j];
					runLength = 1;
				}
			}
		}
		tempBoardStr += std::to_string(lastCell) + "#" + std::to_string(runLength);

		// Check if current state matches any past state
		if (pastStates.find(tempBoardStr) != pastStates.end()) {
			break;
		}

		// Add current state to past states
		pastStates.insert(tempBoardStr);

		gameOfLife(tempBoard, BOARD_SIZE);
		numGenerations--;
		temp_score++; // increase fitness for each generation
		curAlive = howManyAlive(tempBoard);
		if (curAlive > maxAlive) {
			maxAlive = curAlive;
			copyBoard(tempBoard, maxState);
		}
	}

	this->score = getScore(maxAlive, startAlive, temp_score, shieldOfDavid);
}

void Chromosome::calculateFitness(double totalFitnessScore) {
	if (totalFitnessScore == 0) {
		this->fitness = 0;
	} else {
		this->fitness = this->score / totalFitnessScore;
	}
}

/* shadowing board on purpose */
void Chromosome::printBoard(std::ostream &os,
							int board[BOARD_SIZE][BOARD_SIZE]) {
	int row = 0, col = 0;

	// find first row with alive cell
	for (row = 0; row < BOARD_SIZE; row++) {
		for (col = 0; col < BOARD_SIZE; col++) {
			if (board[row][col] == 1) {
				break;
			}
		}
		if (col != BOARD_SIZE) {
			break;
		}
	}

	// find last row with alive cell
	int lastRowWithAliveCell = -1;
	for (int i = BOARD_SIZE - 1; i >= 0; i--) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] == 1) {
				lastRowWithAliveCell = i;
				break;
			}
		}
		if (lastRowWithAliveCell != -1) {
			break;
		}
	}

	// add padding to the top and bottom of the board
	lastRowWithAliveCell = std::min(lastRowWithAliveCell + 2, BOARD_SIZE - 1);
	int startRow = std::max(row - 2, 0);

	// print only the rows in the range of the first and last rows with alive
	// cells
	for (row = startRow; row <= lastRowWithAliveCell; row++) {
		for (col = 0; col < BOARD_SIZE; col++) {
			// add border
			if (row == startRow || row == lastRowWithAliveCell || col == 0 ||
				col == BOARD_SIZE - 1) {
				os << "#";
				continue;
			}
			if (board[row][col] == 1) {
				os << square;
			} else {
				os << " ";
			}
		}
		os << std::endl;
	}
}