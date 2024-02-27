#include "ga.hpp"
#include "gui.hpp"
#include "utils.hpp"
#include <ctime> // for time()
#include <iostream>

std::string boardFile;

int main(int argc, char *argv[]) {
	srand(time(NULL)); // seed the random number generator
	testSelection();
	testCrossover();
	std::cout << "Welcome to the Game of Life!" << std::endl;
	gtk_init(&argc, &argv);

	std::string boardFile;
	if (argc > 1) {
		boardFile = argv[1];
		std::cout << "Loading board from file: " << boardFile << std::endl;
	} else {
		std::cout << "No board file specified, starting with blank board"
				  << std::endl;
                  boardFile = "";
	}

	GUI gui(boardFile); // pass the board file to the GUI constructor

	// call start() every 200 milliseconds
	g_timeout_add(200, GUI::start, NULL);
	gtk_main(); // start the main GTK loop

	return 0;
}