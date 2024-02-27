#pragma once

#include "config.hpp"
#include <memory>
#include <string>

// Class representing a chromosome in the population
// a chromosome is a game of life configuration represented by a 2D array
class Chromosome {
  public:
	Chromosome(); // default constructor will create a random chromosome
	Chromosome(std::shared_ptr<Chromosome> parent1,
			   std::shared_ptr<Chromosome> parent2);
	Chromosome(int crossoverPoint, std::shared_ptr<Chromosome> parent1,
			   std::shared_ptr<Chromosome> parent2);

	void calculateScore();
	void calculateFitness(double totalFitnessScore);
	void printBoard();
	void printBoard(std::ostream &os, int board[BOARD_SIZE][BOARD_SIZE]);

	std::string filename;
	int board[BOARD_SIZE][BOARD_SIZE];
	int maxState[BOARD_SIZE][BOARD_SIZE];
	int stableState[BOARD_SIZE][BOARD_SIZE];
	int stablePeriod;
	int score;		// the success of the chromosome in the defined task
	double fitness; // the chances of the choromosom to move on
};