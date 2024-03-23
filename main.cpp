#include <condition_variable>
#include <mutex>
#include <thread>
#include <iostream>
#include <gtk/gtk.h>
#include <string>
#include "gui.hpp"
#include "ga.hpp"
#include "chromosome.hpp"
#include "population.hpp"
#include "utils.hpp"
std::mutex mtx;
std::condition_variable cv;
bool ready = false;
GUI* gui = nullptr;

void guiThread() {
	std::unique_lock<std::mutex> lock(mtx);
	while (!ready) { // if not ready, wait until main() sends data
		cv.wait(lock); // release lock and wait
	}
	// after the wait, we own the lock again
	g_timeout_add(100, GUI::start, NULL);
}

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

	gui = new GUI(boardFile); // Create the GUI in the main thread

	std::thread t1(guiThread);

	// do some work
	{
		std::lock_guard<std::mutex> lock(mtx);
		ready = true; // set data as ready
	}
	cv.notify_one(); // notify the waiting thread

	gtk_main(); // start the main GTK loop

	t1.join();

	delete gui; // Don't forget to delete the GUI when you're done with it

	return 0;
}