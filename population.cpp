#include "population.hpp"
#include "chromosome.hpp"
#include <iostream>

// Population constructor
// @param populationSize: the size of the population
Population::Population(int populationSize)
	: populationSize(populationSize), totalFitnessScore(0), generation(1) {
	// create random population
	for (int i = 0; i < populationSize; i++) {
		createChromosome();
	}
}

// Create a random chromosome and add it to the population
// TODO: implement private ctor for Chromosome and make it friend of Population
std::shared_ptr<Chromosome> Population::createChromosome() {
	Chromosome chromosome = Chromosome();
	chromosomes.push_back(std::make_shared<Chromosome>(chromosome));
	return nullptr;
}

std::vector<std::shared_ptr<Chromosome>>
Population::getOffsprings(std::shared_ptr<Chromosome> parent1,
						  std::shared_ptr<Chromosome> parent2) {
	std::vector<std::shared_ptr<Chromosome>> offspring;
	offspring.push_back(std::make_shared<Chromosome>(parent1, parent2));
	offspring.push_back(std::make_shared<Chromosome>(parent2, parent1));

	return offspring;
}

#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
void Population::calculateTotalScore() {
	// calculate total fitness score for the population
	// calculate first to set best and worst chromosomes
	worstChromosome = chromosomes[0];
	bestChromosome = chromosomes[0];
	totalScore = 0;
	int totalFitnessScore = 0;

	for (auto chromosome : chromosomes) {
		chromosome->calculateScore();
		if (chromosome->score > bestChromosome->score) {
			bestChromosome = chromosome;
		}
		if (chromosome->score < worstChromosome->score) {
			worstChromosome = chromosome;
		}
		totalFitnessScore += chromosome->score;
		this->totalScore += chromosome->score;
	}

	// update the fitness score for each chromosome
	for (auto chromosome : chromosomes) {
		chromosome->calculateFitness(static_cast<double>(totalFitnessScore));
	}
}

void Population::selection(
	double selectionPressure,
	void (*selectionFunction)(
		std::vector<std::shared_ptr<Chromosome>> &chromosomes,
		int survivorsSize)) {
	// Select parents based on fitness score
	// The higher the selection pressure the more likely it is to choose the
	// fittest chromosomes
	int survivorsSize =
		std::max(1, static_cast<int>(populationSize * selectionPressure));
	selectionFunction(chromosomes, survivorsSize);
}

void Population::crossover() {
	// create a new population
	std::vector<std::shared_ptr<Chromosome>> newPopulation;

	// Generate new individuals until the new population is full
	while (static_cast<int>(newPopulation.size()) < populationSize) {
		// Select two parents
		int parent1Id = arc4random_uniform(chromosomes.size());
		int parent2Id = arc4random_uniform(chromosomes.size());

		// Perform crossover
		std::shared_ptr<Chromosome> parent1 = chromosomes[parent1Id];
		std::shared_ptr<Chromosome> parent2 = chromosomes[parent2Id];

		auto offspring = getOffsprings(parent1, parent2);

		// Add the new individuals to the new population
		newPopulation.push_back(offspring[0]);
		newPopulation.push_back(offspring[1]);

		// If the new population is not full, add the fittest parent to the new
		// population
		if (newPopulation.size() < chromosomes.size()) {
			newPopulation.push_back(chromosomes[parent1Id]);
		}
	}

	// Replace the old population with the new population
	chromosomes = newPopulation;
}

// Uniform mutation implementation
// @param mutationRate: the probability of a gene to mutate
void Population::mutation(double mutationRate) {
	// Random mutation based on mutation rate
	for (auto &individual : chromosomes) {
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = 0; j < BOARD_SIZE; ++j) {
				double randNum = (arc4random_uniform(UINT32_MAX) / (double)UINT32_MAX);
				if (randNum < mutationRate) {
					individual->board[i][j] =
						1 - individual->board[i][j]; // Flips the bit
				}
			}
		}
	}
}
