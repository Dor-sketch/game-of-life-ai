CXXFLAGS = -Wall -Wextra -std=c++17 `pkg-config --cflags gtk+-3.0` -O3 -march=native -flto `pkg-config --libs gtk+-3.0` -lfftw3 -lm
OBJS = main.cpp ga.cpp chromosome.cpp population.cpp gui.cpp game.cpp utils.cpp

met: $(OBJS)
	$(CXX) $(CXXFLAGS) -o met $(OBJS)

clean:
	rm -f met