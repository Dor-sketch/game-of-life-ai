#include "ga.hpp"
#include "chromosome.hpp"
#include "population.hpp"
#include "utils.hpp"
#include <chrono>
#include <ctime> // for time() - used to seed the random number generator
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <sys/stat.h>
#include <unordered_map>

GeneticAlgorithm::GeneticAlgorithm(int populationSize, int maxGenerations,
								   double mutationRate,
								   double selectionPressure)
	: maxGenerations(maxGenerations), mutationRate(mutationRate),
	  selectionPressure(selectionPressure), populationSize(populationSize) {
	// create random population
	population = std::make_shared<Population>(populationSize);
}

void GeneticAlgorithm::run() {
	while (population->generation < maxGenerations) {
		population->calculateTotalScore();
		report();
		population->selection(selectionPressure,
							  &GeneticAlgorithm::rouletteWheelSelection);
		population->crossover();
		population->mutation(mutationRate);
		population->generation++;
	}
	population->calculateTotalScore();
	save();
	printSummary();
}

void GeneticAlgorithm::rouletteWheelSelection(
	std::vector<std::shared_ptr<Chromosome>> &chromosomes, int survivorsSize) {
	std::vector<int> cumulativeScores(chromosomes.size());
	int totalScore = 0;
	for (int i = 0; i < static_cast<int>(chromosomes.size()); i++) {
		totalScore += chromosomes[i]->score;
		cumulativeScores[i] = totalScore;
	}

	std::vector<std::shared_ptr<Chromosome>> survivors;
	while (static_cast<int>(survivors.size()) < static_cast<int>(survivorsSize)) {
		int random = arc4random_uniform(totalScore);
		auto it = std::lower_bound(cumulativeScores.begin(),
								   cumulativeScores.end(), random);
		int index = std::distance(cumulativeScores.begin(), it);
		survivors.push_back(chromosomes[index]);
		// std::cout << "selected: " << chromosomes[index]->score << std::endl;
	}
	chromosomes = survivors;
}

void GeneticAlgorithm::report() {
	std::cout << "Generation: " << population->generation << std::endl;
	std::cout << "Total score: " << population->totalScore << std::endl;
	std::cout << "Average score: " << population->totalScore / populationSize
			  << std::endl;
	std::cout << "Best score: " << population->bestChromosome->score
			  << std::endl;
	std::cout << "Best fitness: " << population->bestChromosome->fitness
			  << std::endl;
	std::cout << "Best chromosome: " << std::endl;
	population->bestChromosome->printBoard();
	std::cout << std::endl;
}

void GeneticAlgorithm::save() {
	std::time_t t = std::time(0); // get time now
	std::tm *now = std::localtime(&t);
	// track filenames for duplicate scores
	std::unordered_map<std::string, int> filenames;
	std::string dirName =
		"populations/" + std::to_string(now->tm_year + 1900) + "-" +
		std::to_string(now->tm_mon + 1) + "-" + std::to_string(now->tm_mday) +
		"-" + std::to_string(now->tm_hour) + "-" + std::to_string(now->tm_min) +
		"-" + std::to_string(now->tm_sec);

	struct stat st = {};

	if (stat("populations", &st) == -1) {
		mkdir("populations", 0700);
	}
	system(("mkdir " + dirName).c_str());

	for (auto chromosome : population->chromosomes) {

		std::string fileName =
			dirName + "/" + std::to_string(chromosome->score);

		while (filenames.find(fileName) != filenames.end()) {
			static int i = 0;
			fileName += "_" + std::to_string(i++);
		}
		filenames[fileName] = 1;
		fileName += ".txt";

		std::ofstream file(fileName);
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = 0; j < BOARD_SIZE; ++j) {
				file << chromosome->board[i][j];
			}
			file << "\n";
		}
		file.close();
		chromosome->filename = fileName;
	}

	resultsPath = dirName;
}



void GeneticAlgorithm::printSummary() {
	std::string fileName = resultsPath + "/summary.txt";
	std::ofstream file(fileName);

	if (!file.is_open()) {
		std::cerr << "Unable to open file: " << fileName << std::endl;
		return;
	}
	// store individuls info + visual representation from the last generation
	// order by score

	std::multimap<int, std::shared_ptr<Chromosome>> chromosomesMap;
	for (auto chromosome : population->chromosomes) {
		chromosomesMap.insert(std::make_pair(chromosome->score, chromosome));
	}

	file << "Summery of run at: " << resultsPath << "\n\n";
	file << "Population size: " << populationSize << "\n";
	file << "Max generations: " << maxGenerations << "\n";
	file << "Mutation rate: " << mutationRate << "\n";
	file << "Selection pressure: " << selectionPressure << "\n\n";
	file << "Total score: " << population->totalScore << "\n";
	file << "Average score: " << population->totalScore / populationSize
		 << "\n";

	int i = chromosomesMap.size();
	for (auto chromosome = chromosomesMap.rbegin();
		 chromosome != chromosomesMap.rend(); ++chromosome) {
		file << "Individual " << --i << "\n";
		file << "Score: " << std::to_string(chromosome->first) << "\n";
		file << "Fitness: " << std::to_string(chromosome->second->fitness)
			 << "\n";
		file << "Initial board: (size: "
			 << howManyAlive(chromosome->second->board) << ")\n";
		chromosome->second->printBoard(file, chromosome->second->board);
		file << "Maximal board (size: "
			 << howManyAlive(chromosome->second->maxState) << "):\n";
		chromosome->second->printBoard(file, chromosome->second->maxState);
		file << "Stable board: (size: "
			 << howManyAlive(chromosome->second->stableState) << ")\n";
		chromosome->second->printBoard(file, chromosome->second->stableState);
		file << "To run this individual, use the following command:\n";
		file << "	./met " << chromosome->second->filename << "\n";
		file << "\n\n";
	}

	file.close();
}