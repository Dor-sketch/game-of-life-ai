#pragma once

constexpr int BOARD_SIZE = 32;
constexpr char square[] = "\u25A0"; // Unicode character for a filled square
#define GENERATIONS 100
#define POPULATION_SIZE 100
#define MUTATION_RATE 0.00005
#define CROSSOVER_RATE 0.7
const double aliveProbability = 0.2; // the probability of a cell being alive at
                                     // the start of the simulation
