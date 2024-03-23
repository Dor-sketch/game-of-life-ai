#pragma once

#include "config.hpp"
#include <vector>
#include <memory>

class Chromosome;

// Class representing a population of chromosomes in a specific generation
class Population {
  public:
	Population(int populationSize); // create random population

    std::vector<std::shared_ptr<Chromosome>>
	getOffsprings(std::shared_ptr<Chromosome> parent1,
				  std::shared_ptr<Chromosome> parent2);
	std::shared_ptr<Chromosome> createChromosome();
	void mutation(double mutationRate);
	void crossover();
	void calculateTotalScore();
	void selection(double selectionPressure,
				   void (*selectionFunction)(
					   std::vector<std::shared_ptr<Chromosome>> &chromosomes,
					   int survivorsSize));

	int populationSize;
	int totalFitnessScore;
	int totalScore;
	int generation;
	std::vector<std::shared_ptr<Chromosome>> chromosomes;
	std::shared_ptr<Chromosome> bestChromosome;
	std::shared_ptr<Chromosome> worstChromosome;
};
