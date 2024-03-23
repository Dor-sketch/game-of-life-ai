#pragma once

#include "config.hpp"
#include <gtk/gtk.h>
#include <string>

// gtk gui
class GUI {
public:
  GUI(const std::string &boardFile);
  GUI();
  ~GUI();
  static gboolean start(gpointer data);
  static void setBoard(int[BOARD_SIZE][BOARD_SIZE]);
  static void runGeneticAlgorithm();
  static void pauseResume();
  static void clear();
  static void random();
  static void next();
  static void save();
  static void load(const std::string &path);
  static void quit();
  static void buttonClicked(GtkWidget *widget, gpointer data);
  static void setDarkTheme();
  static void setLightTheme();

private:
  static void createButtons();
  static void connectSignals();
  static void createWindow();
  static void showWindow();
  static void setButtonColor(GtkWidget *button, int cell_state);
  static void open_folder(const std::string &path);
  static void open_from_file();
  static void createLayoutContainers();
  static void createButtonsAndLabels();
  static void initGUI();
  static void update();
  static gboolean draw_graph(GtkWidget *widget, cairo_t *cr, gpointer data);
  static std::string lastSavePath;
  static std::vector<int> aliveCellsData;
  static bool isPaused;
  static int generationCount;
  static int populationSize;
  static int maxAliveCells;
  static int board[BOARD_SIZE][BOARD_SIZE];
  static const int boardColSize = BOARD_SIZE;
  static GtkLabel *generationLabel;
  static GtkLabel *populationLabel;
  static GtkLabel *maxAliveCellsLabel;
  static GtkLabel *startAliveCellsLabel;
  static GtkWidget *window;
  static GtkWidget *grid;
  static GtkWidget *buttons[BOARD_SIZE][BOARD_SIZE];
  static GtkWidget *stopButton;
  static GtkWidget *clearButton;
  static GtkWidget *randomButton;
  static GtkWidget *nextButton;
  static GtkWidget *saveButton;
  static GtkWidget *loadButton;
  static GtkWidget *quitButton;
  static GtkWidget *graphArea;
  static GtkWidget *graphDrawingArea;
  static GtkWidget *runGeneticAlgorithmButton;
  static GtkWidget *graphDrawingAreaContainer;
  static GtkWidget *graphDrawingAreaContainerScroll;
  static GtkWidget *graphDrawingAreaContainerScrollContainer;
  static GtkWidget *loadFromFileButton;
};
