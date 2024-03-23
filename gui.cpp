#include "gui.hpp"
#include "ga.hpp"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <thread>
#include <time.h>
#include <unistd.h>

extern void gameOfLife(int board[BOARD_SIZE][BOARD_SIZE], int boardColSize);
static bool shouldDrawGraph = false;
static int saveScreenshotCounter = 0;
int GUI::generationCount = 0;
int GUI::populationSize = 0;
int GUI::maxAliveCells = 0;
bool GUI::isPaused = true;
std::string GUI::lastSavePath = "";
std::vector<int> GUI::aliveCellsData;
GtkWidget *GUI::window;
GtkWidget *GUI::grid;
GtkWidget *GUI::stopButton;
GtkWidget *GUI::clearButton;
GtkWidget *GUI::randomButton;
GtkWidget *GUI::nextButton;
GtkWidget *GUI::saveButton;
GtkWidget *GUI::loadButton;
GtkWidget *GUI::quitButton;
GtkWidget *GUI::loadFromFileButton;
GtkWidget *GUI::buttons[BOARD_SIZE][BOARD_SIZE];
GtkWidget *GUI::graphArea;
GtkWidget *GUI::graphDrawingArea;
GtkWidget *GUI::graphDrawingAreaContainer;
GtkWidget *GUI::runGeneticAlgorithmButton;
GtkLabel *GUI::generationLabel;
GtkLabel *GUI::maxAliveCellsLabel;
GtkLabel *GUI::startAliveCellsLabel;
GtkLabel *GUI::populationLabel;

namespace Themes {
const std::string pastelRed = "FF6961";
const std::string pastelOrange = "FFA07A";
const std::string pastelPink = "FFD1DC";

const std::string lightTheme = R"(
window, button, label, grid, drawingarea {
	background-color: #FFFFFF;             // White background
	color: #000000;                        // Black text
	font-family: 'Courier New', monospace; // Monospace font
}
button { // White background, black text, black border
	background: #FFFFFF;
	color: #000000;
	border-color: #000000;
	border-radius: 0; // Remove rounded corners
}
button:hover { // Darken black on hover
	background: #000000;
	color: #FFFFFF;               // White text on hover
	box-shadow: 0 0 10px #000000; // Black glow on hover
}
.alive {
	background-color: #FF43A4;     // Neon pink for "alive" cells
	box-shadow: 0 0 10px #FF43A4; // Neon pink glow
	animation: pulse 1s infinite; // Pulse animation
}
@keyframes pulse { // Define pulse animation
	0% { box-shadow: 0 0 5px #FF43A4; }
	50% { box-shadow: 0 0 20px #FF43A4, 0 0 30px #FF43A4; }
	100% { box-shadow: 0 0 5px #FF43A4; }
}
)";

const std::string darkTheme = R"(
window, button, label, grid, drawingarea {
	background-color: #000000;             // Pure black background
	color: #FFFFFF;                        // White text
	font-family: 'Courier New', monospace; // Monospace font
}
button { // Black background, neon green text, neon green border
	background: #000000;
	color: #39FF14;
	border-color: #39FF14;
	border-radius: 0; // Remove rounded corners
	box-shadow: 0 0 10px #39FF14; // Neon green glow
}
button:hover { // Brighten neon green on hover
	background: #39FF14;
	color: #000000;               // Black text on hover
	box-shadow: 0 0 20px #39FF14; // Increase glow on hover
}
.alive {
	background-color: #FF43A4;     // Neon pink for "alive" cells
	box-shadow: 0 0 10px #FF43A4; // Neon pink glow
	animation: pulse 1s infinite; // Pulse animation
}
@keyframes pulse { // Define pulse animation
	0% { box-shadow: 0 0 5px #FF43A4; }
	50% { box-shadow: 0 0 20px #FF43A4, 0 0 30px #FF43A4; }
	100% { box-shadow: 0 0 5px #FF43A4; }
}
)";

const std::string salmonTheme = R"(
window, button, label, grid, drawingarea {
	background-color: #F8F8F8;
	color: #333333;
	font-family: 'Courier New', monospace;
}
button {
 background: #F8F8F8;
 color: #333333;
 border-color: #333333;
 border-radius: 0;
}
button:hover {
 box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19), 0 8px 10px 0 rgba(0, 0, 0, 0.2), 0 0 10px #333333;
 background: #333333;
 color: #F8F8F8;
}
.alive {
 box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19), 0 8px 10px 0 rgba(0, 0, 0, 0.2);
 background-color: #FFA07A;
}
drawingarea {
 background: #FFFFFF;
 border: 2px solid #333333;
}
)";

const std::string redTheme = R"(
window, button, label, grid, drawingarea {
	background-color: #F8F8F8;
	color: #333333;
	font-family: 'Courier New', monospace;
}
button {
 background: #F8F8F8;
 color: #333333;
 border-color: #333333;
 border-radius: 0;
}
button:hover {
 box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19), 0 8px 10px 0 rgba(0, 0, 0, 0.2), 0 0 10px #333333;
 background: #333333;
 color: #F8F8F8;
}
.alive {
 box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19), 0 8px 10px 0 rgba(0, 0, 0, 0.2);
 background-color: #FF6961;
}
drawingarea {
 background: #FFFFFF;
 border: 2px solid #333333;
}
)";
} // namespace Themes

GUI::GUI(const std::string &boardFile) {
  initGUI();
  if (!boardFile.empty())
    load(boardFile);
}

GUI::GUI() { initGUI(); }

void GUI::initGUI() {
  createWindow();
  createLayoutContainers();
  createButtonsAndLabels();
  connectSignals();
  showWindow();
  setLightTheme();
}

int GUI::board[BOARD_SIZE][BOARD_SIZE] = {{0}}; // double braces to silence
                                                // warning

void GUI::createWindow() {
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Game of Life");
  GdkDisplay *display = gdk_display_get_default();
  GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
  GdkRectangle geometry;
  gdk_monitor_get_geometry(monitor, &geometry);
  int scale = gdk_monitor_get_scale_factor(monitor);
  int new_width =
      std::min(geometry.width * scale * 0.5, geometry.height * scale * 0.5);
  int new_height = new_width;
  gtk_window_resize(GTK_WINDOW(window), new_width, new_height);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
}

void GUI::setLightTheme() {
  GtkCssProvider *provider = gtk_css_provider_new();
  gtk_css_provider_load_from_data(provider, Themes::salmonTheme.c_str(), -1,
                                  NULL);
  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                            GTK_STYLE_PROVIDER(provider),
                                            GTK_STYLE_PROVIDER_PRIORITY_USER);
  g_object_unref(provider);
}

void GUI::setDarkTheme() {
  // Set dark and cool theme
  GtkCssProvider *provider = gtk_css_provider_new();
  gtk_css_provider_load_from_data(provider, Themes::darkTheme.c_str(), -1,
                                  NULL);
  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                            GTK_STYLE_PROVIDER(provider),
                                            GTK_STYLE_PROVIDER_PRIORITY_USER);
  g_object_unref(provider);
}

void GUI::createLayoutContainers() {
  grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
  gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

  // Initialize graphDrawingArea with a fixed size
  graphDrawingArea = gtk_drawing_area_new();
  gtk_widget_set_size_request(graphDrawingArea, 200, 200);

  // Create a new box for graphDrawingArea
  GtkWidget *graphDrawingAreaBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  // Add graphDrawingArea to its box
  gtk_box_pack_start(GTK_BOX(graphDrawingAreaBox), graphDrawingArea, FALSE,
                     FALSE, 0);

  // Create a separate box for graphArea
  graphArea = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

  // Add the graphDrawingAreaBox to graphArea
  gtk_box_pack_start(GTK_BOX(graphArea), graphDrawingAreaBox, FALSE, FALSE, 0);

  // Place graphArea in the grid without affecting button sizes
  gtk_grid_attach(GTK_GRID(grid), graphArea, (BOARD_SIZE + 1), 12, 1, 1);

  gtk_container_add(GTK_CONTAINER(window), grid);
}
void GUI::createButtons() {
  // calculate button size based on window size and board size
  int windowWidth, windowHeight;
  gtk_window_get_size(GTK_WINDOW(window), &windowWidth, &windowHeight);
  int buttonWidth = std::min(windowWidth, windowHeight) / BOARD_SIZE;
  int buttonHeight = std::min(windowWidth, windowHeight) / BOARD_SIZE;

  GtkWidget *buttonContainer =
      gtk_fixed_new(); // Create a fixed container for buttons
  gtk_grid_attach(GTK_GRID(grid), buttonContainer, 0, 0, BOARD_SIZE,
                  BOARD_SIZE); // Attach the container to the grid

  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      buttons[i][j] = gtk_button_new();
      gtk_widget_set_size_request(buttons[i][j], buttonWidth, buttonHeight);
      gtk_fixed_put(GTK_FIXED(buttonContainer), buttons[i][j], i * buttonWidth,
                    j * buttonHeight);
    }
  }

  // make all buttons flat
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      gtk_button_set_relief(GTK_BUTTON(buttons[i][j]), GTK_RELIEF_NONE);
    }
  }
}
void GUI::createButtonsAndLabels() {
  // create buttons
  createButtons();

  // create stop button
  stopButton = gtk_button_new_with_label("Start");
  gtk_grid_attach(GTK_GRID(grid), stopButton, (BOARD_SIZE + 1), 1, 1, 1);

  // create clear button
  clearButton = gtk_button_new_with_label("Clear");
  gtk_grid_attach(GTK_GRID(grid), clearButton, (BOARD_SIZE + 1), 2, 1, 1);

  // create random button
  randomButton = gtk_button_new_with_label("Random");
  gtk_grid_attach(GTK_GRID(grid), randomButton, (BOARD_SIZE + 1), 3, 1, 1);

  // create next button
  nextButton = gtk_button_new_with_label("Next");
  gtk_grid_attach(GTK_GRID(grid), nextButton, (BOARD_SIZE + 1), 4, 1, 1);

  // create save button
  saveButton = gtk_button_new_with_label("Save");
  gtk_grid_attach(GTK_GRID(grid), saveButton, (BOARD_SIZE + 1), 5, 1, 1);

  // create load button
  loadButton = gtk_button_new_with_label("Load");
  gtk_grid_attach(GTK_GRID(grid), loadButton, (BOARD_SIZE + 1), 6, 1, 1);

  // create quit button
  quitButton = gtk_button_new_with_label("Quit");
  gtk_grid_attach(GTK_GRID(grid), quitButton, (BOARD_SIZE + 1), 7, 1, 1);

  // create button to load configuration from file
  loadFromFileButton = gtk_button_new_with_label("Load from file");
  gtk_grid_attach(GTK_GRID(grid), loadFromFileButton, (BOARD_SIZE + 1), 10, 1,
                  1);

  // create button to run the genetic algorithm
  runGeneticAlgorithmButton =
      gtk_button_new_with_label("Run Genetic Algorithm");
  gtk_grid_attach(GTK_GRID(grid), runGeneticAlgorithmButton, (BOARD_SIZE + 1),
                  11, 1, 1);

  // create generation label
  generationLabel = GTK_LABEL(gtk_label_new("Generation: 1"));
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(generationLabel), (BOARD_SIZE + 1),
                  8, 1, 1);

  // create population size label
  populationLabel = GTK_LABEL(gtk_label_new("Population size: 0"));
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(populationLabel), (BOARD_SIZE + 1),
                  14, 1, 1);

  // create max alive cells label
  maxAliveCellsLabel = GTK_LABEL(gtk_label_new("Max alive cells: 0"));
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(maxAliveCellsLabel),
                  (BOARD_SIZE + 1), 15, 1, 1);

  // create start alive cells label
  startAliveCellsLabel = GTK_LABEL(gtk_label_new("Start alive cells: 0"));
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(startAliveCellsLabel),
                  (BOARD_SIZE + 1), 16, 1, 1);
}

void GUI::connectSignals() {

  // connect signals
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(stopButton, "clicked", G_CALLBACK(pauseResume), NULL);
  g_signal_connect(clearButton, "clicked", G_CALLBACK(clear), NULL);
  g_signal_connect(randomButton, "clicked", G_CALLBACK(random), NULL);
  g_signal_connect(nextButton, "clicked", G_CALLBACK(next), NULL);
  g_signal_connect(saveButton, "clicked", G_CALLBACK(save), NULL);
  g_signal_connect(loadButton, "clicked", G_CALLBACK(load), NULL);
  g_signal_connect(quitButton, "clicked", G_CALLBACK(quit), NULL);
  // turned of for performance reasons
  g_signal_connect(graphDrawingArea, "draw", G_CALLBACK(draw_graph), NULL);
  g_signal_connect(loadFromFileButton, "clicked", G_CALLBACK(open_from_file),
                   NULL);
  g_signal_connect(runGeneticAlgorithmButton, "clicked",
                   G_CALLBACK(runGeneticAlgorithm), NULL);

  // connect buttons to board
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      g_signal_connect(buttons[i][j], "clicked", G_CALLBACK(buttonClicked),
                       &board[i][j]);
    }
  }
}

void GUI::showWindow() { gtk_widget_show_all(window); }

GUI::~GUI() { gtk_widget_destroy(window); }

void GUI::runGeneticAlgorithm() {
  // Run the genetic algorithm in a new thread
  std::thread gaThread([]() {
    GeneticAlgorithm ga(POPULATION_SIZE, GENERATIONS, MUTATION_RATE,
                        CROSSOVER_RATE);
    ga.run();
  });
  gaThread.detach(); // Detach the thread to let it run independently
}
gboolean GUI::start(gpointer /*data*/) {
  if (!isPaused) {
    gameOfLife(board, BOARD_SIZE);
    update();
    generationCount++;
  }
  return TRUE; // return TRUE to keep the timeout
}

void GUI::pauseResume() {
  // pause game (dont quit)
  isPaused = !isPaused;
  if (isPaused) {
    gtk_button_set_label(GTK_BUTTON(stopButton), "Resume");
  } else {
    gtk_button_set_label(GTK_BUTTON(stopButton), "Pause");
  }
}

void GUI::clear() {
  // clear board
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      board[i][j] = 0;
    }
  }
  // aliveCellsData.clear();
  generationCount = 1;
  populationSize = 0;
  maxAliveCells = 0;
  update();
}

void GUI::random() {
  // randomize board
  srand(time(NULL));
  int random;
  int count = 0;
  int numberOfCells = arc4random_uniform(BOARD_SIZE * BOARD_SIZE);
  while (count < numberOfCells / 5) {
    random = arc4random_uniform(BOARD_SIZE * BOARD_SIZE);
    board[random / BOARD_SIZE][random % BOARD_SIZE] = 1;
    count++;
  }
  update();
}

void GUI::setBoard(int newBoard[BOARD_SIZE][BOARD_SIZE]) {
  // set board
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      board[i][j] = newBoard[i][j];
    }
  }
  update();
}

void GUI::next() {
  // next generation
  gameOfLife(board, BOARD_SIZE);
  generationCount++;
  update();
}

void GUI::save() {
  // save board to file
  std::string dirName = "saves";
  // create directory if it doesn't exist
  struct stat st = {};
  if (stat("saves", &st) == -1) {
    mkdir("saves", 0700);
  }
  system(("mkdir " + dirName).c_str());

  // open window and ask user for filename
  GtkWidget *dialog, *entry;
  dialog = gtk_dialog_new_with_buttons("Save", GTK_WINDOW(window),
                                       GTK_DIALOG_MODAL, "OK", GTK_RESPONSE_OK,
                                       "Cancel", GTK_RESPONSE_CANCEL, NULL);
  entry = gtk_entry_new();
  gtk_container_add(
      GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), entry);
  gtk_widget_show_all(dialog);

  gint response = gtk_dialog_run(GTK_DIALOG(dialog));
  const char *filename;
  if (response == GTK_RESPONSE_OK) {
    filename = gtk_entry_get_text(GTK_ENTRY(entry));
    std::string fileName = dirName + "/" + filename + ".txt";

    // save board to file
    lastSavePath = dirName;
    std::ofstream file(fileName);
    for (int i = 0; i < BOARD_SIZE; i++) {
      for (int j = 0; j < BOARD_SIZE; ++j) {
        file << board[i][j];
      }
      file << std::endl;
    }

    file.close();
  } else {
    gtk_widget_destroy(dialog);
    return;
  }

  gtk_widget_destroy(dialog);
  static const std::string lastSavedFile = filename;
}

void GUI::load(const std::string &path) {
  std::cout << "Loading board from file: " << path << std::endl;
  // load board
  std::ifstream file;
  file.open(path);
  if (!file.is_open()) {
    // open the saved folder instead
    std::cout << "Error opening file" << std::endl;
    open_folder(std::string("saves"));
    return;
  }
  std::string line;
  int i = 0;
  while (getline(file, line)) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      board[i][j] = line[j] - '0';
    }
    i++;
  }
  file.close();
  update();
  gtk_label_set_text(
      GTK_LABEL(startAliveCellsLabel),
      ("Start alive cells: " + std::to_string(populationSize)).c_str());
  generationCount = 1;
}
void GUI::open_folder(const std::string &path) {
  GtkWidget *fileChooser;
  fileChooser = gtk_file_chooser_dialog_new(
      "Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel",
      GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);
  gtk_widget_show_all(fileChooser);
  gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(fileChooser),
                                      path.c_str());
  gint resp = gtk_dialog_run(GTK_DIALOG(fileChooser));
  if (resp == GTK_RESPONSE_ACCEPT) {
    char *filename;
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fileChooser));
    std::ifstream file;
    file.open(filename);
    std::string line;
    int i = 0;
    while (getline(file, line)) {
      for (int j = 0; j < BOARD_SIZE; j++) {
        board[i][j] = line[j] - '0';
      }
      i++;
    }
    file.close();
    update();
    gtk_label_set_text(
        GTK_LABEL(startAliveCellsLabel),
        ("Start alive cells: " + std::to_string(populationSize)).c_str());
    gtk_label_set_text(
        GTK_LABEL(maxAliveCellsLabel),
        ("Max alive cells: " + std::to_string(populationSize)).c_str());
  }
  gtk_widget_destroy(fileChooser);
  generationCount = 1;
}

void GUI::open_from_file() {
  if (!lastSavePath.empty()) {
    open_folder(lastSavePath);
    return;
  }
  // open chose file dialog
  GtkWidget *fileChooser;
  fileChooser = gtk_file_chooser_dialog_new(
      "Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel",
      GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);
  gtk_widget_show_all(fileChooser);
  gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(fileChooser),
                                      g_get_home_dir());
  gint resp = gtk_dialog_run(GTK_DIALOG(fileChooser));
  if (resp == GTK_RESPONSE_ACCEPT) {
    char *filename;
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fileChooser));
    std::ifstream file;
    file.open(filename);
    std::string line;
    int i = 0;
    while (getline(file, line)) {
      for (int j = 0; j < BOARD_SIZE; j++) {
        board[i][j] = line[j] - '0';
      }
      i++;
    }
    file.close();
    update();

    // Store the folder chosen by the user
    char *folder;
    folder = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(fileChooser));
    lastSavePath = folder;
    g_free(folder);
  }
  gtk_widget_destroy(fileChooser);
  generationCount = 1;
}
gboolean GUI::draw_graph(GtkWidget *widget, cairo_t *cr, gpointer data) {
  (void)data;
  if (!shouldDrawGraph) {
    return FALSE;
  }
  shouldDrawGraph = false;
  // draw population (y axis) changes over generation (x axis)
  // adjust to fit the graph inside the drawing area
  int width, height;
  width = gtk_widget_get_allocated_width(widget);
  height = gtk_widget_get_allocated_height(widget);
  cairo_scale(cr, width / 200.0, height / 200.0);

  // draw background
  cairo_set_source_rgb(cr, 0, 0, 0); // Change background to black
  cairo_paint(cr);

  // draw axis
  cairo_set_source_rgb(cr, 0.227, 1, 0.078); // Change axis to neon green
  cairo_set_line_width(cr, 1);
  cairo_move_to(cr, 0, 0);
  cairo_line_to(cr, 0, 200);
  cairo_line_to(cr, 200, 200);
  cairo_stroke(cr);

  // draw data
  cairo_set_source_rgb(cr, 1, 0.263, 0.639); // Change data to neon purple
  cairo_set_line_width(cr, 1);
  cairo_move_to(cr, 0, 200);
  for (int i = 0; i < static_cast<int>(aliveCellsData.size()); i++) {
    cairo_line_to(cr, i * 200.0 / aliveCellsData.size(),
                  200 - aliveCellsData[i] * 200.0 / maxAliveCells);
  }
  cairo_stroke(cr);

  return FALSE;
}
void GUI::quit() {
  if (gtk_main_level() > 0) {
    gtk_main_quit();
  }
}

void GUI::update() {
  static int lastGeneration = 0, lastPopulation = 0;

  if (populationSize != lastPopulation) {
    std::string population =
        "Population size: " + std::to_string(populationSize);
    gtk_label_set_text(GTK_LABEL(populationLabel), population.c_str());
    lastPopulation = populationSize;
  }

  if (generationCount <= 100 && lastGeneration != generationCount) {
    gtk_label_set_text(
        GTK_LABEL(generationLabel),
        std::to_string(static_cast<int>(generationCount)).c_str());
    lastGeneration = generationCount;
  } else if (generationCount > 100 && generationCount % 10 == 0 &&
             lastGeneration != generationCount) {
    gtk_label_set_text(
        GTK_LABEL(generationLabel),
        std::to_string(static_cast<int>(generationCount)).c_str());
    lastGeneration = generationCount;
  }
  populationSize = 0;
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; ++j) {
      setButtonColor(buttons[i][j], board[i][j]);
      populationSize += board[i][j];
    }
  }
  aliveCellsData.push_back(populationSize);

  if (populationSize > maxAliveCells) {
    std::string maxAliveCellsStr =
        "Max alive: " + std::to_string(populationSize);
    gtk_label_set_text(GTK_LABEL(maxAliveCellsLabel), maxAliveCellsStr.c_str());
    maxAliveCells = populationSize;
  }
  shouldDrawGraph = true;
  gtk_widget_show_all(window);
  gtk_widget_queue_draw(graphArea);
}

void GUI::saveScreenshot() {
  int cellWidth = gtk_widget_get_allocated_width(buttons[0][0]);
  int cellHeight = gtk_widget_get_allocated_height(buttons[0][0]);
  int cellsWidth = cellWidth * BOARD_SIZE;
  int cellsHeight = cellHeight * BOARD_SIZE;
  // Capture screenshot
  GtkWidget *parent =
      gtk_widget_get_parent(buttons[0][0]);          // get parent of a cell
  GdkWindow *window = gtk_widget_get_window(parent); // get window of the parent

  int cellX, cellY;
  gtk_widget_translate_coordinates(buttons[0][0], parent, 0, 0, &cellX, &cellY);
  if (window != nullptr) {
    GdkPixbuf *screenshot =
        gdk_pixbuf_get_from_window(window, 10, 10, cellsWidth, cellsHeight);
    // get unique filename monotonic increasing

    std::ostringstream paddedCounter;
    paddedCounter << std::setw(7) << std::setfill('0')
                  << saveScreenshotCounter++;
    std::string filename = "screenshot" + paddedCounter.str() + ".png";
    if (screenshot != nullptr) {
      gdk_pixbuf_save(screenshot, filename.c_str(), "png", nullptr, nullptr);
      g_object_unref(screenshot);
    }
  }
}

void GUI::buttonClicked(GtkWidget *widget, gpointer data) {
  int *cell = static_cast<int *>(data);
  *cell = !*cell;
  setButtonColor(widget, *cell);
}

void GUI::setButtonColor(GtkWidget *button, int cell_state) {
  // Obtain the style context for the widget
  GtkStyleContext *context = gtk_widget_get_style_context(button);

  if (cell_state == 1) { // If the cell is alive
    // Add the 'alive' class to the button
    gtk_style_context_add_class(context, "alive");
  } else { // If the cell is dead
    // Remove the 'alive' class from the button
    gtk_style_context_remove_class(context, "alive");
  }
}