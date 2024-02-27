#include "utils.hpp"
#include "chromosome.hpp"
#include "ga.hpp"
#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>

int howManyAlive(int board[BOARD_SIZE][BOARD_SIZE]) {
	// count how many cells are alive in a given board
	int alive = 0;
	for (int row = 0; row < BOARD_SIZE; row++) {
		for (int col = 0; col < BOARD_SIZE; col++) {
			alive += board[row][col];
		}
	}
	return alive;
}

void copyBoard(int board[BOARD_SIZE][BOARD_SIZE],
			   int newBoard[BOARD_SIZE][BOARD_SIZE]) {
	// copy a board to a new board
	for (int row = 0; row < BOARD_SIZE; row++) {
		memcpy(newBoard[row], board[row], BOARD_SIZE * sizeof(int));
	}
}

// test chromosome crossover
void testCrossover() {
	std::cout << "Testing crossover" << std::endl;
	std::shared_ptr<Chromosome> parent1 = std::make_shared<Chromosome>();
	std::shared_ptr<Chromosome> parent2 = std::make_shared<Chromosome>();
	std::cout << "Parent 1:" << std::endl;
	parent1->calculateScore();
	parent1->calculateFitness(0);
	parent1->printBoard();
	std::cout << std::endl;
	std::cout << "Parent 1 score: " << parent1->score << std::endl;
	std::cout << "Parent 2:" << std::endl;
	parent2->calculateScore();
	parent2->calculateFitness(0);
	parent2->printBoard();
	std::cout << std::endl;
	std::cout << "Parent 2 score: " << parent2->score << std::endl;

	std::shared_ptr<Chromosome> offspring =
		std::make_shared<Chromosome>(parent1, parent2);
	std::cout << "Offspring:" << std::endl;
	offspring->calculateScore();
	offspring->calculateFitness(0);
	offspring->printBoard();
	std::cout << std::endl;
	std::cout << "Offspring score: " << offspring->score << std::endl;
}

void testSelection() {
	std::cout << "Testing selection" << std::endl;
	std::vector<std::shared_ptr<Chromosome>> chromosomes;
	for (int i = 0; i < 100; i++) {
		std::shared_ptr<Chromosome> chromosome = std::make_shared<Chromosome>();
		chromosome->score = i;
		chromosomes.push_back(chromosome);
	}
	std::cout << "Before selection:" << std::endl;
	for (int i = 0; i < 100; i++) {
		std::cout << chromosomes[i]->score << std::endl;
	}
	GeneticAlgorithm::rouletteWheelSelection(chromosomes, 5);
	std::cout << "After selection:" << std::endl;
	for (int i = 0; i < 5; i++) {
		std::cout << chromosomes[i]->score << std::endl;
	}
}
