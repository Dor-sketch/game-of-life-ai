#pragma once

#include "config.hpp"
#include <memory>
#include <string>
#include <vector>

class Chromosome;
class Population;

/*
	* Genetic Algorithm class
	* The genetic algorithm class is responsible for running the genetic
	algorithm
*/
class GeneticAlgorithm {
  public:

	GeneticAlgorithm(int populationSize, int maxGenerations,
					 double mutationRate, double selectionPressure);

	void save();
	static void rouletteWheelSelection(
		std::vector<std::shared_ptr<Chromosome>> &chromosomes,
		int survivorsSize);
	void initPopulation();
	void run();
	void printSummary();
	void report();
	int multiRoundTournamentSelection();
	std::shared_ptr<Population> population;
	int maxGenerations;
	double mutationRate;
	double selectionPressure;
	int populationSize;
	std::string resultsPath;
};