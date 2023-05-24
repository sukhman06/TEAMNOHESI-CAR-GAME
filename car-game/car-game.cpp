// car-game.cpp
// Developed By: Sukhman Sekhon, Madhav Sharma, Kunj Patel and Shivam Patel
// 

#include <iostream> // Include input/output stream
#include <conio.h> // Include console input/output
#include <dos.h> // Include DOS functions
#include <windows.h> // Include windows functions
#include <time.h> // Include time functions
#include <fstream> // Include file stream
#include <sstream> // Include string stream
#include <vector> // Include vector
#include <string> // Include string
#include <algorithm> // Include algorithm
#include "concol.h" // Include concol header file

using namespace std; // Using standard namespace to call built in functions and classes
using namespace eku; // Using eku namespace to call functions from header file 
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE); // Set console handle
COORD cursorPosition; // Hold cursor position

// Initialize variables 
int screenWidth = 90; // Width of console screen
int screenHeight = 26; // Height of console screen
int windowWidth = 70; // Width of game window

int currentX = 0; // Current X-coordinate of the cursor
int currentY = 0; // Current Y-coordinate of the cursor

int obstacleY[3]; // Y-coordinate of the obstacle cars
int obstacleX[3]; // X-coordinate of the obstacle cars
int obstacleCheck[3]; // Check if an obstacle car is present

char car[4][4] = {  // ASCII for the players car
	' ', '0', '0', ' ',
	'0', '0', '0', '0',
	' ', '0', '0', ' ',
	'0', '0', '0', '0'
};


int carPosition = windowWidth / 2; // Position of the players car
int score = 0; // Initialize score to 0

void instructions(int difficultySelected, int carColor);
void mainMenu(int difficultSelected, int carColor);
void play(int difficulty, int carColor);


struct Player { // Structure to hold player data
	string name;
	int score;
};

// Function to read leaderboard data
vector<Player> read_csv(string filename) {
	vector<Player> players; // Initialize vector to hold player data
	ifstream file(filename); // Open file
	string line; // Initialize string to hold line of file
	getline(file, line); // Get line of file

	while (getline(file, line)) { // Loop through each line of file
		stringstream ss(line); // Initialize string stream to hold line
		string name, score_str; // Initialize string to hold name and score
		getline(ss, name, ','); // Get name
		getline(ss, score_str, ','); // Get score
		int score = stoi(score_str); // Convert score to integer
		players.push_back({ name, score }); // Add player to vector
	}
	return players; // Return vector data
}

// Function to print leaderboard
void leaderboard(vector<Player> players, int difficulty, int carColor) {
	system("cls"); // Clear console screen
	sort(players.begin(), players.end(), [](const Player& a, const Player& b) {
		return a.score > b.score; // Sort players by score
		}
	);
	cout << white << "Leaderboard:" << endl;
	for (const auto& player : players) { // Loop through each player
		cout << white << player.name << ": " << player.score << endl; // Print player name and score
	}
	cout << white << "Press any key to return to the main menu";
	_getch(); // Wait for user input
	mainMenu(difficulty, carColor); // Return to main menu
}

/*
// Function to set cursor position
// Slower than current goToXY function
void goToXY(int x, int y) {
	cursorPosition.X = x; // Set X-coordinate of the cursor
	cursorPosition.Y = y; // Set Y-coordinate of the cursor
	SetConsoleCursorPosition(console, cursorPosition); // Set cursor position
}
*/

// Function to set cursor position
// Faster than old goToXY function because it only moves the cursor when needed
void goToXY(int x, int y) {
	if (currentX != x || currentY != y) { // Check if cursor is already at the position
		cursorPosition.X = x; // Set X-coordinate of the cursor
		cursorPosition.Y = y; // Set Y-coordinate of the cursor
		SetConsoleCursorPosition(console, cursorPosition); // Set cursor position
		currentX = x; // Update current X-coordinate
		currentY = y; // Update current Y-coordinate
	}
}

void setCursor(bool visible) {
	CONSOLE_CURSOR_INFO cursorInfo; // Create cursor info object
	cursorInfo.bVisible = visible; // Set cursor visibility
	SetConsoleCursorInfo(console, &cursorInfo); // Set cursor info
}

//Function to print game border
void drawBorder() {
	for (int i = 0; i < screenHeight; i++) { // Loop through each row
		for (int j = 0; j < 17; j++) { // Loop through each column
			goToXY(0 + j, i); // Set cursor position
			cout << white << "#"; // Print border
			goToXY(windowWidth - j, i); // Set cursor position
			cout << white << "#"; // Print border
		}
	}
}

//Function to generate obstacles 
void generateObstacle(int num) {
	obstacleX[num] = 17 + rand() % (33); // Generate random X-coordinate
}

//Function to draw the obstacle 
void drawObstacle(int num) {
	if (obstacleCheck[num] == true) { // Check if obstacle is present
		goToXY(obstacleX[num], obstacleY[num]); // Set cursor position
		cout << red << "****"; // Print obstacle
		goToXY(obstacleX[num], obstacleY[num] + 1); // Set cursor position
		cout << red << " ** "; // Print obstacle
		goToXY(obstacleX[num], obstacleY[num] + 2); // Set cursor position
		cout << red << "****"; // Print obstacle
		goToXY(obstacleX[num], obstacleY[num] + 3); // Set cursor position
		cout << red << " ** "; // Print obstacle
	}
}


//Function to erase the obstacle from the screen
void eraseObstacle(int num) {
	if (obstacleCheck[num] == true) { // Check if obstacle is present
		goToXY(obstacleX[num], obstacleY[num]); // Set cursor position
		cout << "    "; // Erase obstacle
		goToXY(obstacleX[num], obstacleY[num] + 1); // Set cursor position
		cout << "    "; // Erase obstacle
		goToXY(obstacleX[num], obstacleY[num] + 2); // Set cursor position
		cout << "    "; // Erase obstacle
		goToXY(obstacleX[num], obstacleY[num] + 3); // Set cursor position
		cout << "    "; // Erase obstacle
	}
}

//Function to reset obstacle position
void resetObstacle(int num) { 
	eraseObstacle(num); // Erase obstacle
	obstacleY[num] = 1; // Reset Y-coordinate
	generateObstacle(num); // Generate new X-coordinate
}

//Function to draw the car 
void drawCar(int carColor) {
	for (int i = 0; i < 4; i++) { // Loop through each row
		for (int j = 0; j < 4; j++) { // Loop through each column
			goToXY(carPosition + j, i + 22); // Set cursor position
			switch (carColor) { // Check car color
			case 1: 
				cout << blue << car[i][j]; // Print car
				break;
			case 2: 
				cout << green << car[i][j]; // Print car
				break;
			case 3:
				cout << aqua << car[i][j]; // Print car
				break;
			case 4:
				cout << red << car[i][j]; // Print car
				break;
			case 5: 
				cout << purple << car[i][j]; // Print car
				break;
			case 6:
				cout << yellow << car[i][j]; // Print car
				break;
			case 7:
				cout << white << car[i][j]; // Print car
				break;
			case 8:
				cout << dark_green << car[i][j]; // Print car
				break;
			case 9:
				cout << dark_aqua << car[i][j]; // Print car
				break;
			case 10:
				cout << dark_red << car[i][j]; // Print car
				break;
			case 11:
				cout << dark_purple << car[i][j]; // Print car
				break;
			case 12:
				cout << dark_yellow << car[i][j]; // Print car
				break;
			}
		}
	}
}

//Function to erase thw car from the screen
void eraseCar() {
	for (int i = 0; i < 4; i++) { // Loop through each row
		for (int j = 0; j < 4; j++) { // Loop through each column
			goToXY(carPosition + j, i + 22); // Set cursor position
			cout << " "; // Print blank space to erase car
		} 
	}
}

//Function to check for collisions
bool collision() {
	for (int i = 0; i < 3; i++) { // Loop through each obstacle
		if (obstacleCheck[i]) { // Check if obstacle is present
			if (obstacleY[i] + 4 >= 23 && obstacleX[i] + 4 >= carPosition && obstacleX[i] <= carPosition + 3) { // Check if collision is detected
				return true; // Return true if collision is detected
			}
		}
	}
	return false; // Return false if no collision is detected
}



//Function to show the game over screen
void gameover(int score, int difficulty, int carColor) {
	string name; // Create string to store name
	string filename = "leaderboard.csv"; // Set file name

	system("cls"); // Clear screen

	cout << white << "Game Over" << endl;
	cout << white << "Score: " << score << endl;

	ofstream fout(filename, ios::app); // Open file
	cout << white << "Enter your name: "; // Ask for name
	cin >> name; // Store name
	fout << name << "," << score << endl; // Write name and score to file
	fout.close(); // Close file
	
	vector<Player> players = read_csv(filename); // Read file
	leaderboard(players, difficulty, carColor); // Display leaderboard

	//mainMenu();
}

//Function to update and print the score
void updateScore() {
	goToXY(58, 3); // Set cursor position
	cout << white << "Score: " << score << endl; // Print score
}

void carColorMenu(int difficulty) {
	char option; // Create char to store option
	system("cls"); // Clear screen
	cout << white << "Select Car Color" << endl; 
	cout << blue << "1. Blue" << endl;
	cout << green << "2. Green" << endl;
	cout << aqua << "3. Aqua" << endl;
	cout << red << "4. Red" << endl;
	cout << purple << "5. Purple" << endl;
	cout << yellow << "6. Yellow" << endl;
	cout << white << "7. White" << endl;
	cout << dark_green << "8. Dark Green" << endl;
	cout << dark_aqua << "9. Dark Aqua" << endl;
	cout << dark_red << "10. Dark Red" << endl;
	cout << dark_purple << "11. Dark Purple" << endl;
	cout << dark_yellow << "12. Dark Yellow" << endl;

	while (&option != "1" || &option != "2" || &option != "3" || &option != "4" || &option != "5" || &option != "6" || &option != "7" || &option != "8" || &option != "9" || &option != "10" || &option != "11" || &option != "12") { // Loop until valid option is selected
		option = _getch();
		if (option == '1') {
			mainMenu(difficulty, 1); // Go to main menu
		}
		else if (option == '2') {
			mainMenu(difficulty, 2); 
		}
		else if (option == '3') {
			mainMenu(difficulty, 3);
		}
		else if (option == '4') {
			mainMenu(difficulty, 4);
		}
		else if (option == '5') {
			mainMenu(difficulty, 5);
		}
		else if (option == '6') {
			mainMenu(difficulty, 6);
		}
		else if (option == '7') {
			mainMenu(difficulty, 7);
		}
		else if (option == '8') {
			mainMenu(difficulty, 8);
		}
		else if (option == '9') {
			mainMenu(difficulty, 9);
		}
		else if (option == '10') {
			mainMenu(difficulty, 10);
		}
		else if (option == '11') {
			mainMenu(difficulty, 11);
		}
		else if (option == '12') {
			mainMenu(difficulty, 12);
		}
	}
}

// Function to select difficulty
void difficultySelect(int carColor) {
	char difficultyOption; // Create char to store difficulty option 
	system("cls"); // clears the screen
	cout << white << "Select Difficulty" << endl;
	cout << green << "1. Easy" << endl;
	cout << yellow << "2. Normal" << endl;
	cout << dark_red << "3. Hard" << endl;
	cout << dark_purple << "4. Impossible" << endl;

	while (&difficultyOption != "1" || &difficultyOption != "2" || &difficultyOption != "3" || &difficultyOption == "4") { // Loop until valid input is entered
		difficultyOption = _getche();
		if (difficultyOption == '1') {
			mainMenu(1, carColor); // Call mainMenu function with easy difficulty
		}
		else if (difficultyOption == '2') {
			mainMenu(2, carColor); // Call mainMenu function with normal difficulty
		}
		else if (difficultyOption == '3') {
			mainMenu(3, carColor); // Call mainMenu function with hard difficulty
		}
		else if (difficultyOption == '4') {
			mainMenu(4, carColor); // Call mainMenu function with impossible difficulty
		}
	}
}

//Function to show the main menu
void mainMenu(int difficultySelected, int carColor) {
	char option;
	string mode;
	if (difficultySelected == 1) {  // Convert difficulty to string
		mode = "Easy";
	}
	else if (difficultySelected == 2) { 
		mode = "Normal";
	}
	else if (difficultySelected == 3) {
		mode = "Hard";
	}
	else if (difficultySelected == 4) {
		mode = "Impossible";
	}
	else {
		mode = "Select Difficulty"; 
	}

	system("cls"); // Clear screen
	cout << white << "Car Game" << endl;
	cout << white << "1. Start Game" << " " << "Difficulty: " << mode << endl;
	cout << white << "2. How To Play" << endl;
	cout << white << "3. Difficulty" << endl;
	cout << white << "4. Car Color" << endl;
	cout << white << "5. Quit" << endl;


	while (&option != "1" || &option != "2" || &option != "3") { // Loop until valid input is entered
		option = _getche(); // Get input
		if (option == '1') { 
			if (difficultySelected == 0) { // Check if difficulty is selected
				system("cls");
				cout << white << "Select a Difficulty" << endl;
				cout << white << "Press any key to return to the main menu";
				_getch(); // Wait for key press
				mainMenu(0, carColor);
			}
			else {
				play(difficultySelected, carColor); // Call play function with selected difficulty
			}
		}
		else if (option == '2') {
			instructions(difficultySelected, carColor); // Call instructions function
		}
		else if (option == '3') {
			difficultySelect(carColor); // Call difficultySelect function
		}
		else if (option == '4') {
			carColorMenu(difficultySelected); // Call carColorMenu function
		}
		else if (option == '5') {
			exit(0); // Exit program
		}
	}

}


// Prints instructions 
void instructions(int difficulty, int carColor) {
	system("cls"); // Clear screen
	cout << white << "Instructions:" << endl;
	cout << white << "Avoid cars by moving left or right" << endl;
	cout << white << "Press the left arrow key to move left" << endl;
	cout << white << "Press the right arrow key to move right" << endl;
	cout << white << "Press the escape key to exit" << endl;
	cout << white << "Press any key to go back to the main menu" << endl;

	_getch(); // Wait for key press
	mainMenu(difficulty, carColor); // Call mainMenu function
}


void play(int difficulty, int carColor) {
	score = 0; // Set score to 0
	carPosition = -1 + windowWidth / 2; // Set car position to middle of screen
	obstacleCheck[0] = 1; // Set obstacle check to 1
	obstacleCheck[1] = 0; // Set obstacle check to 0 

	obstacleY[0] = 1; // Set obstacle y position to 1
	obstacleY[1] = 1; // Set obstacle y position to 1

	system("cls"); // Clear screen
	drawBorder(); // Draw border
	updateScore(); // Update score

	generateObstacle(0); // Generate obstacle 1
	generateObstacle(1); // Generate obstacle 2

	goToXY(24, 12); // Set cursor position
	cout << white << "Press any key to start"; // Print text
	_getch(); // Wait for key press
	goToXY(0, 0); // Set cursor position
	goToXY(24, 12); // Set cursor position
	cout << "                      "; // Clear text

	while (1) { // Infinite loop
		if (_kbhit()) { 
			char input = _getch(); // Get input
			if (input == 75) { // Check if input is left arrow key
				if (carPosition > 20) { // Check if car is not at the left border
					carPosition -= 4; // Move car left
				}
			}
			if (input == 77) { // Check if input is right arrow key
				if (carPosition < 50) { // Check if car is not at the right border
					carPosition += 4; // Move car right
				}
			}
			if (input == 27) { // Check if input is escape key
				mainMenu(difficulty, carColor); // Call mainMenu function
			}
		}

		drawCar(carColor); // Draw car
		drawObstacle(0); // Draw obstacle 1
		drawObstacle(1); // Draw obstacle 2

		if (collision() == true) { // Check if collision occurred
			gameover(score, difficulty, carColor); // Call gameover function
			return;
		}

		// Switch statement to change game speed based on difficulty 
		switch (difficulty) {
		case 1: // Easy
			Sleep(50); // Sleep for 50 milliseconds
			break;
		case 2: // Normal
			Sleep(30); // Sleep for 30 milliseconds
			break;
		case 3: // Hard
			Sleep(10); // Sleep for 10 milliseconds
			break;
		case 4: // Impossible
			Sleep(5); // Sleep for 5 milliseconds
			break;
		}


		eraseCar(); // Erase car

		eraseObstacle(0); // Erase obstacle 1
		eraseObstacle(1); // Erase obstacle 2

		if (obstacleY[0] == 10) { // Check if obstacle 1 is at the bottom of the screen
			if (obstacleCheck[1] == 0) { // Check if obstacle 2 is not generated
				obstacleCheck[1] = 1;
			}
		}

		if (obstacleCheck[0] == 1) { // Check if obstacle 1 is generated
			obstacleY[0] += 1; // Move obstacle 1 down
		}

		if (obstacleCheck[1] == 1) { // Check if obstacle 2 is generated
			obstacleY[1] += 1; // Move obstacle 1 down
		}

		if (obstacleY[0] > screenHeight - 4) { // Check if obstacle 1 is at the bottom of the screen
			resetObstacle(0); // Reset obstacle 1
			score++; // Add one to score
			updateScore(); // Update score
		}
		if (obstacleY[1] > screenHeight - 4) { // Check if obstacle 2 is at the bottom of the screen
			resetObstacle(1); // Reset obstacle 1
			score++; // Add one to score
			updateScore(); // Update score
		} 
	}
}

int main()
{
	setCursor(false); // Hide console cursor
	concolinit(); // Initialize console colors
	srand((unsigned int)time(NULL)); // Initialize random number generator
	SetConsoleTitleA("Car Game"); // Set console title
	int carColor = 7; // Set color to white
	mainMenu(0, carColor); // Show main menu
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
