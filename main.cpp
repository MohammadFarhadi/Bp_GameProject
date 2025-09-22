#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include "SudokuGenerator.h"
using namespace std;
enum GameState { STORY, LOGIN, PASSWORD_ENTRY, MENU, DIFFICULTY_SELECTION, GAME, LOAD_GAME, SCORESHEET };


// ریدم تو مبانی
struct TypewriterText {
    string fullText;
    string displayedText;
    int textIndex = 0;
    sf::Text sfText;
    sf::Clock clock;
    int speed = 50;

    TypewriterText(const string& text, sf::Font& font, int size, sf::Color color, sf::Vector2f position, int typingSpeed) {
        fullText = text;
        displayedText = "";
        textIndex = 0;
        speed = typingSpeed;
        sfText.setFont(font);
        sfText.setCharacterSize(size);
        sfText.setFillColor(color);
        sfText.setPosition(position);
    }

    void update() {
        if (textIndex < fullText.length() && clock.getElapsedTime().asMilliseconds() > speed) {
            displayedText += fullText[textIndex];
            textIndex++;
            sfText.setString(displayedText);
            clock.restart();
        }
    }

    void reset() {
        displayedText = "";
        textIndex = 0;
        clock.restart();
    }
};

// Max Saved Entries
const int MAX_SAVED_GAMES = 10;
const long long int MAX_SCORES = 1000000;

// Saved Game Storage
string savedGames[MAX_SAVED_GAMES];
int savedGameCount = 0;

// Score Storage
string scores[MAX_SCORES];
int scoreCount = 0;
bool loadGame(const string& filename, int sudokuGrid[9][9], int& mistakes, int& hints, int& score, int& timeLeft, int& loadedDifficulty){
    ifstream file(filename);
    if (!file) {
        cout << "No saved game found!" << endl;
        return false;
    }

    // Load grid
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file >> sudokuGrid[i][j];
        }
    }

    // Load other game-related data
    file >> mistakes >> hints >> score >> timeLeft >> loadedDifficulty;

    file.close();
    cout << "Game loaded successfully!" << endl;


    return true;
}

void saveGame(const string& filename, int sudokuGrid[9][9], int mistakes, int hints, int score, int timeLeft, int selectedDifficulty) {
    ofstream file(filename);
    if (!file) {
        cout << "Error saving game!" << endl;
        return;
    }

    // Save grid
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file << sudokuGrid[i][j] << " ";
        }
        file << endl;
    }

    // Save other game-related data
    file << mistakes << " " << hints << " " << score << " " << timeLeft << " " << selectedDifficulty << endl;

    file.close();

    // Add to saved games list
    if (savedGameCount < MAX_SAVED_GAMES) {
        savedGames[savedGameCount++] = filename;
    }

    cout << "Game saved successfully!" << endl;
}



// Function to read users from file
bool userExists(const string& username, string& storedPassword) {
    ifstream file("users.txt");
    string line, name, password;
    while (getline(file, line)) {
        stringstream ss(line);
        ss >> name >> password;
        if (name == username) {
            storedPassword = password;
            return true;
        }
    }
    return false;
}



void loadScores() {
    scoreCount = 0;
    ifstream file("scores.txt");

    if (!file) {
        cout << "Error opening scores file!" << endl;
        return;
    }

    string line;
    while (getline(file, line) && scoreCount < MAX_SCORES) {
        if (line.empty()) continue; // Skip empty lines

        stringstream ss(line);
        string playerName, result, difficulty;
        int playerScore;

        if (!(ss >> playerName >> playerScore >> result >> difficulty)) {
            cout << "Skipping invalid score entry: " << line << endl;
            continue; // Skip malformed lines
        }

        scores[scoreCount++] = line;
    }

    file.close();
}




// Function to save a new user to the file
void saveUser(const string& username, const string& password) {
    ofstream file("users.txt", ios::app);
    file << username << " " << password << endl;
    file.close();
}


// Enum to Track Different Game States

GameState gameState = STORY; // Start at login screen
bool sudokoSolved = 1;
string playerNames = ""; // Store player name
string playerPassword = "";  // Store user input password
string storedPassword = "";  // Store the correct password if the user exists
bool enteringPassword = false;
bool incorrectPassword = false;
bool settingNewPassword = false;


string difficultyToString(int difficulty) {
    switch (difficulty) {
        case easy: return "Spy";
        case medium: return "Super Spy";
        case hard: return "Semnani";
        default: return "Unknown";
    }
}
void saveScore(const string& name, long long int score, bool won, int difficulty) {
    if (name.empty()) return; // Prevent empty names from being saved

    ofstream file("scores.txt", ios::app);
    if (!file) {
        cout << "Error opening score file!" << endl;
        return;
    }

    file << name << " " << score << " " << (won ? "Win" : "Lose") << " " << difficultyToString(difficulty) << endl;
    file.close();
}



int main() {

    sf::RenderWindow window(sf::VideoMode(800, 600), "SSIS TEST", sf::Style::Default);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    // Sudoku Grid getting the numbers
    int sudokuGrid[9][9];
    generateSudoku(sudokuGrid, medium);

    // Checking which cells are filled
    bool isStatic[9][9] = {false};
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (sudokuGrid[i][j] != 0) {
                isStatic[i][j] = true;
            }
        }
    }


    //toof mali
    bool first = true;

    // Scanline
    const int lineSpacing = 5.5;
    const float lineThickness = 0.75f;
    sf::Color scanlineColor(0, 255, 0, 50);

    // Create Sudoku Grid
    sf::RectangleShape Sudoku_grid[9][9];
    sf::Color background_color(0, 0, 0);
    sf::Color shapes_color(3, 160, 98);

    const float startX = (800 - 450) / 2.0f;
    const float startY = (600 - 450) / 2.0f;

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            Sudoku_grid[i][j].setSize(sf::Vector2f(49.f, 49.f));
            Sudoku_grid[i][j].setPosition(startX + j * 50.f, startY + i * 50.f);
            Sudoku_grid[i][j].setFillColor(sf::Color::Transparent);
            Sudoku_grid[i][j].setOutlineThickness(2);
            Sudoku_grid[i][j].setOutlineColor(shapes_color);
        }
    }

    // Number font
    sf::Font font;
    if (!font.loadFromFile("VT323-Regular.ttf")) {
        cout << "Error loading font!" << endl;
        return 1;
    }

    // Create SFML Text for Sudoku Numbers
    sf::Text numbers[9][9];

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            numbers[i][j].setFont(font);
            numbers[i][j].setCharacterSize(30);
            numbers[i][j].setFillColor(sf::Color::White);
            numbers[i][j].setPosition(startX + j * 50.f + 15, startY + i * 50.f + 5);

            // Set text to the number if it's not zero
            if (sudokuGrid[i][j] != 0) {
                numbers[i][j].setString(to_string(sudokuGrid[i][j]));
            }
        }
    }

    // Input handling
    int selectedRow = 0;
    int selectedCol = 0;

    // Input highlight
    sf::RectangleShape selectedCellHighlight;
    selectedCellHighlight.setSize(sf::Vector2f(49.f, 49.f));
    selectedCellHighlight.setFillColor(sf::Color(255, 255, 0, 100)); // Semi-transparent yellow
    // correct-Input_highlight
    sf::RectangleShape correctHighlight;
    correctHighlight.setSize(sf::Vector2f(49.f, 49.f));
    correctHighlight.setFillColor(sf::Color(0, 255, 0, 100)); // Semi-transparent green
    //wrong-Input-highlihgt
    sf::RectangleShape wrongHighLight;
    wrongHighLight.setSize(sf::Vector2f (49.f, 49.f));
    wrongHighLight.setFillColor(sf::Color(255, 0, 0, 100));

    //Game over & mistake
    int mistakes = 0;
    bool gameOver = false;

    //undo variable
    int undoMoves[81][3]; // Stores (row, col, value)
    int undoCount = 0;

    //Help
    int HCount = 3;
    bool abbasgholi = 0;


    //timer
    int timeLeft = (selectedDifficulty == easy) ? 300 :(selectedDifficulty == medium) ? 180 : 120;
    sf::Clock gameClock;
    sf::Text timerText;
    timerText.setFont(font);
    timerText.setCharacterSize(24);
    timerText.setFillColor(shapes_color);
    timerText.setPosition(20, 20);

    // Pause UI
    bool isPaused = false;  // Track pause state
    sf::Text pauseText;
    pauseText.setFont(font);
    pauseText.setCharacterSize(90);
    pauseText.setFillColor(shapes_color);
    pauseText.setString("PAUSED");
    pauseText.setPosition(250, 450); // Centered text

    // Login UI
// LOGIN UI Elements
    TypewriterText loginText(
            "Enter your name:",
            font,
            30,
            shapes_color,
            sf::Vector2f(250, 200), // Will adjust to center next
            20
    );

    TypewriterText instructionText(
            "Press ENTER to continue",
            font,
            20,
            shapes_color,
            sf::Vector2f(250, 300), // Will adjust to center next
            20
    );

// Keep this for real-time input
    sf::Text inputText;
    inputText.setFont(font);
    inputText.setCharacterSize(30);
    inputText.setFillColor(shapes_color);
    inputText.setPosition(250, 230);



    // Menu UI
    TypewriterText menuText(
            "1. Start Game\n2. Load Game\n3. Score Sheet\n4. Quit",
            font,
            30,
            shapes_color,
            sf::Vector2f(50, 250),
            20
    );


    // Load Game UI
    TypewriterText loadGameText(
            "Saved Games:\n",
            font,
            30,
            shapes_color,
            sf::Vector2f(50, 200),
            20
    );


    // Player Name Display in Game
    sf::Text playerNameText;
    playerNameText.setFont(font);
    playerNameText.setCharacterSize(24);
    playerNameText.setFillColor(shapes_color);
    playerNameText.setPosition(20, 50);

    //Load usertext
    ifstream file("users.txt");
    if (!file) {
        ofstream createFile("users.txt");  // Ensure file exists
        createFile.close();
    }
    file.close();
    // UI Elements
    sf::Text hintText;
    sf::Text mistakeText;
    sf::Text scoreText;

    // create ui for hint text and stuff
    hintText.setFont(font);
    hintText.setCharacterSize(24);
    hintText.setFillColor(shapes_color);
    hintText.setPosition(300, 20);  // Position at top right

    mistakeText.setFont(font);
    mistakeText.setCharacterSize(24);
    mistakeText.setFillColor(shapes_color);
    mistakeText.setPosition(0, 250);  // Below hint text

    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(shapes_color);
    scoreText.setPosition(0, 280);  // Below mistake text

    int maxMistakes = 5;  // Maximum allowed mistakes
    int playerScore = 0;  // Initial score

    hintText.setString("Hints Left: " + to_string(HCount));
    mistakeText.setString("Mistakes Left: " + to_string(maxMistakes - mistakes));
    scoreText.setString("Score: " + to_string(playerScore));

    sf::Text winText;
    winText.setFont(font);
    winText.setCharacterSize(50);
    winText.setFillColor(shapes_color);
    winText.setString("You Won!");
    winText.setPosition(300, 250); // Center it

    sf::Text loseText;
    loseText.setFont(font);
    loseText.setCharacterSize(50);
    loseText.setFillColor(shapes_color);
    loseText.setString("You Lost!");
    loseText.setPosition(300, 250); // Center it
    //difficulty
    TypewriterText difficultyText(
            "Select Difficulty:\n1. Easy\n2. Medium\n3. Hard",
            font,
            30,
            shapes_color,
            sf::Vector2f(50, 200),
            20
    );


    TypewriterText storyText(
            "Welcome to the SSIS Spy Test!\n\n"
            "You have been selected as one of the most promising candidates for joining SSIS.\n\n"
            "the world'smost secretive spy organization.\n\n"
            "Never heard of SSIS?\n\n"
            "Thats exactly the point. it shows its success in staying hidden.\n\n"
            "SSIS stands for the Semnan Secret Intelligence Service\n\n"
            "This test is designed to assess your intelligence across three levels\n\n"
            "determining your rank as either Spy, Super Spy, or Semnani.\n\n"
            "Please choose your level carefully, as this decision will have a profound impact\n\n"
            "on your future career.\n\n"
            "Press any key to continue...",
            font, 24, shapes_color, sf::Vector2f(0, 0), 30); // 40ms per character




    // Main Loop
    while (window.isOpen()) {
        sudokoSolved = 1;
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if ((sudokuGrid[i][j] == 0 || sudokuGrid[i][j] != Answer[i][j])) {
                    sudokoSolved = 0;
                    break;
                }
            }
        }
        sf::Event event;
        while (window.pollEvent(event)) {
            if (gameState == STORY) {
                if (event.type == sf::Event::KeyPressed) {
                    gameState = LOGIN; // Move to login screen after pressing a key
                }
            }
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // LOGIN SCREEN LOGIC
            if (gameState == LOGIN) {
                if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode == '\b' && !playerNames.empty()) {
                        playerNames.pop_back();  // Handle backspace
                    }
                    else if (event.text.unicode < 128 && event.text.unicode != '\b') {
                        playerNames += static_cast<char>(event.text.unicode);
                    }
                    inputText.setString(playerNames);
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                    if (!playerNames.empty()) {
                        if (userExists(playerNames, storedPassword)) {
                            enteringPassword = true;
                            incorrectPassword = false;
                            playerPassword = "";  // Reset password input
                            gameState = PASSWORD_ENTRY;
                        } else {
                            settingNewPassword = true;
                            enteringPassword = true;
                            incorrectPassword = false;
                            playerPassword = "";  // Reset password input
                            gameState = PASSWORD_ENTRY;
                        }
                    }
                }
            }

            //Password_Logic
            bool pass_enter = 0;
            if (gameState == PASSWORD_ENTRY) {
                if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode == '\b' && !playerPassword.empty()) {
                        playerPassword.pop_back();  // Handle backspace
                    } else if (event.text.unicode < 128 && event.text.unicode != '\b') {
                        playerPassword += static_cast<char>(event.text.unicode);
                    }
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                    if (settingNewPassword) {
                        if (!playerPassword.empty()) {  // Only save if the password is not empty
                            saveUser(playerNames, playerPassword);
                            settingNewPassword = false;
                            enteringPassword = false;
                            incorrectPassword = false;
                            gameState = MENU;
                        }
                    } else {
                        if (!playerPassword.empty() && playerPassword == storedPassword) {
                            enteringPassword = false;
                            incorrectPassword = false;
                            gameState = MENU;
                        } else {
                            incorrectPassword = true;  // Indicate wrong password
                            playerPassword = "";       // Clear input
                        }
                    }
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    // Reset login process
                    playerNames = "";
                    playerPassword = "";
                    incorrectPassword = false;
                    enteringPassword = false;
                    settingNewPassword = false;
                    gameState = LOGIN;
                }
            }

                // MAIN MENU LOGIC
            else if (gameState == MENU) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Num1) {
                        gameState = DIFFICULTY_SELECTION;
                    } else if (event.key.code == sf::Keyboard::Num2) {
                        gameState = LOAD_GAME;
                    } else if (event.key.code == sf::Keyboard::Num3) {
                        loadScores();
                        gameState = SCORESHEET;
                    } else if (event.key.code == sf::Keyboard::Num4) {
                        window.close();
                    }
                }
            } else if (gameState == DIFFICULTY_SELECTION) {
                if (event.type == sf::Event::KeyPressed) {
                    // Reset game state
                    mistakes = 0;
                    gameOver = false;
                    playerScore = 0;
                    undoCount = 0;
                    HCount = 3;

                    // Generate new Sudoku puzzle
                    if (event.key.code == sf::Keyboard::Num1) {
                        selectedDifficulty = easy;
                    } else if (event.key.code == sf::Keyboard::Num2) {
                        selectedDifficulty = medium;
                    } else if (event.key.code == sf::Keyboard::Num3) {
                        selectedDifficulty = hard;
                    }

                    generateSudoku(sudokuGrid, selectedDifficulty);

                    // Reset static cells tracking
                    for (int i = 0; i < 9; i++) {
                        for (int j = 0; j < 9; j++) {
                            isStatic[i][j] = (sudokuGrid[i][j] != 0);
                        }
                    }

                    // Reset UI elements
                    hintText.setString("Hints Left: " + to_string(HCount));
                    mistakeText.setString("Mistakes Left: " + to_string(5 - mistakes));
                    scoreText.setString("Score: " + to_string(playerScore));

                    // Reset Timer
                    timeLeft = (selectedDifficulty == easy) ? 300 :
                               (selectedDifficulty == medium) ? 180 : 120;
                    gameClock.restart();

                    // Move to GAME state
                    gameState = GAME;
                }
            } else if (gameState == LOAD_GAME) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Enter) {
                        if (savedGameCount > 0) {
                            int loadedDifficulty = 0;  // Temporary variable to hold difficulty as an int
                            if (loadGame(savedGames[savedGameCount - 1], sudokuGrid, mistakes, HCount, playerScore, timeLeft, loadedDifficulty)) {
                                gameState = GAME; // Resume the loaded gam
                            }
                            else {
                                cout << "Failed to load game!" << endl;
                            }
                        } else {
                            cout << "No saved games available!" << endl;
                        }
                    } else if (event.key.code == sf::Keyboard::Escape) {
                        gameState = MENU; // Return to menu
                    }
                }
            }





                // SCORE SHEET SCREEN LOGIC
            else if (gameState == SCORESHEET) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    gameState = MENU;
                }
            }


            if (event.type == sf::Event::KeyPressed) {
                //puase
                if (event.key.code == sf::Keyboard::Escape && gameState == GAME) {
                    isPaused = !isPaused;
                }
                if (gameState == GAME  && event.key.code == sf::Keyboard::M) {
                    cout << "Saving game..." << endl;  // Debugging: Check if M is detected
                    saveGame("savegame.txt", sudokuGrid, mistakes, HCount, playerScore, timeLeft, selectedDifficulty);
                }
                if (gameState == GAME && event.key.code == sf::Keyboard::Q){
                    gameState = MENU;
                }
                if (gameState == GAME && !gameOver) {

                    // wasd and u and h
                    switch (event.key.code) {
                        case sf::Keyboard::Up:
                        case sf::Keyboard::W:
                            if (selectedRow > 0) {
                                selectedRow--;
                            }
                            break;
                        case sf::Keyboard::Down:
                        case sf::Keyboard::S:
                            if (selectedRow < 8) {
                                selectedRow++;
                            }
                            break;
                        case sf::Keyboard::Left:
                        case sf::Keyboard::A:
                            if (selectedCol > 0) {
                                selectedCol--;
                            }
                            break;
                        case sf::Keyboard::Right:
                        case sf::Keyboard::D:
                            if (selectedCol < 8) {
                                selectedCol++;
                            }
                            break;

                        case sf::Keyboard::Num1:
                            if (!isStatic[selectedRow][selectedCol] && Answer[selectedRow][selectedCol] == 1) {
                                undoMoves[undoCount][0] = selectedRow;
                                undoMoves[undoCount][1] = selectedCol;
                                undoMoves[undoCount][2] = sudokuGrid[selectedRow][selectedCol]; // Store previous value
                                undoCount++;
                                sudokuGrid[selectedRow][selectedCol] = 1;
                                correctHighlight.setPosition(174 + selectedCol * 49.9f, 75 + selectedRow * 50.f);
                                playerScore += 10;
                                scoreText.setString("Score: " + to_string(playerScore));
                                if (!first) {
                                    window.draw(correctHighlight);
                                    window.display();
                                } else {
                                    first = false;
                                }
                                this_thread::sleep_for(chrono::milliseconds(300));
                            } else {
                                mistakes++;
                                mistakeText.setString("Mistakes Left: " + to_string(maxMistakes - mistakes));
                                wrongHighLight.setPosition(174 + selectedCol * 49.9f, 75 + selectedRow * 50.f);
                                if (first == 0) {
                                    window.draw(wrongHighLight);
                                    window.display();
                                } else {
                                    first = false;
                                }
                                this_thread::sleep_for(chrono::milliseconds(300));
                            }
                            break;
                        case sf::Keyboard::Num2:
                            if (!isStatic[selectedRow][selectedCol] && Answer[selectedRow][selectedCol] == 2) {
                                undoMoves[undoCount][0] = selectedRow;
                                undoMoves[undoCount][1] = selectedCol;
                                undoMoves[undoCount][2] = sudokuGrid[selectedRow][selectedCol]; // Store previous value
                                undoCount++;
                                sudokuGrid[selectedRow][selectedCol] = 2;
                                correctHighlight.setPosition(174 + selectedCol * 49.9f, 75 + selectedRow * 50.f);
                                playerScore += 10;  // Increase score
                                scoreText.setString("Score: " + to_string(playerScore));
                                window.draw(correctHighlight);
                                window.display();
                                this_thread::sleep_for(chrono::milliseconds(300));
                            } else {
                                mistakes++;
                                mistakeText.setString("Mistakes Left: " + to_string(maxMistakes - mistakes));
                                wrongHighLight.setPosition(174 + selectedCol * 49.9f, 75 + selectedRow * 50.f);
                                window.draw(wrongHighLight);
                                window.display();
                                this_thread::sleep_for(chrono::milliseconds(300));
                            }
                            break;

                        case sf::Keyboard::Num3:
                            if (!isStatic[selectedRow][selectedCol] && Answer[selectedRow][selectedCol] == 3) {
                                undoMoves[undoCount][0] = selectedRow;
                                undoMoves[undoCount][1] = selectedCol;
                                undoMoves[undoCount][2] = sudokuGrid[selectedRow][selectedCol]; // Store previous value
                                undoCount++;
                                sudokuGrid[selectedRow][selectedCol] = 3;
                                correctHighlight.setPosition(174 + selectedCol * 49.9f, 75 + selectedRow * 50.f);
                                playerScore += 10;  // Increase score
                                scoreText.setString("Score: " + to_string(playerScore));
                                window.draw(correctHighlight);
                                window.display();
                                this_thread::sleep_for(chrono::milliseconds(300));
                            } else {
                                mistakes++;
                                mistakeText.setString("Mistakes Left: " + to_string(maxMistakes - mistakes));
                                wrongHighLight.setPosition(174 + selectedCol * 49.9f, 75 + selectedRow * 50.f);
                                window.draw(wrongHighLight);
                                window.display();
                                this_thread::sleep_for(chrono::milliseconds(300));
                            }
                            break;

                        case sf::Keyboard::Num4:
                            if (!isStatic[selectedRow][selectedCol] && Answer[selectedRow][selectedCol] == 4) {
                                undoMoves[undoCount][0] = selectedRow;
                                undoMoves[undoCount][1] = selectedCol;
                                undoMoves[undoCount][2] = sudokuGrid[selectedRow][selectedCol]; // Store previous value
                                undoCount++;
                                sudokuGrid[selectedRow][selectedCol] = 4;
                                correctHighlight.setPosition(174 + selectedCol * 49.9f, 75 + selectedRow * 50.f);
                                playerScore += 10;  // Increase score
                                scoreText.setString("Score: " + to_string(playerScore));
                                window.draw(correctHighlight);
                                window.display();
                                this_thread::sleep_for(chrono::milliseconds(300));
                            } else {
                                mistakes++;
                                mistakeText.setString("Mistakes Left: " + to_string(maxMistakes - mistakes));
                                wrongHighLight.setPosition(174 + selectedCol * 49.9f, 75 + selectedRow * 50.f);
                                window.draw(wrongHighLight);
                                window.display();
                                this_thread::sleep_for(chrono::milliseconds(300));
                            }
                            break;

                        case sf::Keyboard::Num5:
                            if (!isStatic[selectedRow][selectedCol] && Answer[selectedRow][selectedCol] == 5) {
                                undoMoves[undoCount][0] = selectedRow;
                                undoMoves[undoCount][1] = selectedCol;
                                undoMoves[undoCount][2] = sudokuGrid[selectedRow][selectedCol]; // Store previous value
                                undoCount++;
                                sudokuGrid[selectedRow][selectedCol] = 5;
                                correctHighlight.setPosition(174 + selectedCol * 49.9f, 75 + selectedRow * 50.f);
                                playerScore += 10;  // Increase score
                                scoreText.setString("Score: " + to_string(playerScore));
                                window.draw(correctHighlight);
                                window.display();
                                this_thread::sleep_for(chrono::milliseconds(300));
                            } else {
                                mistakes++;
                                mistakeText.setString("Mistakes Left: " + to_string(maxMistakes - mistakes));
                                wrongHighLight.setPosition(174 + selectedCol * 49.9f, 75 + selectedRow * 50.f);
                                window.draw(wrongHighLight);
                                window.display();
                                this_thread::sleep_for(chrono::milliseconds(300));

                            }
                            break;

                        case sf::Keyboard::Num6:
                            if (!isStatic[selectedRow][selectedCol] && Answer[selectedRow][selectedCol] == 6) {
                                sudokuGrid[selectedRow][selectedCol] = 6;
                                undoMoves[undoCount][0] = selectedRow;
                                undoMoves[undoCount][1] = selectedCol;
                                undoMoves[undoCount][2] = sudokuGrid[selectedRow][selectedCol]; // Store previous value
                                undoCount++;
                                mistakeText.setString("Mistakes Left: " + to_string(maxMistakes - mistakes));
                                sudokuGrid[selectedRow][selectedCol] = 6;
                                correctHighlight.setPosition(174 + selectedCol * 49.9f, 75 + selectedRow * 50.f);
                                playerScore += 10;  // Increase score
                                scoreText.setString("Score: " + to_string(playerScore));
                                window.draw(correctHighlight);
                                window.display();
                                this_thread::sleep_for(chrono::milliseconds(300));
                            } else {
                                mistakes++;
                                mistakeText.setString("Mistakes Left: " + to_string(maxMistakes - mistakes));
                                wrongHighLight.setPosition(174 + selectedCol * 49.9f, 75 + selectedRow * 50.f);
                                window.draw(wrongHighLight);
                                window.display();
                                this_thread::sleep_for(chrono::milliseconds(300));
                            }
                            break;

                        case sf::Keyboard::Num7:
                            if (!isStatic[selectedRow][selectedCol] && Answer[selectedRow][selectedCol] == 7) {
                                undoMoves[undoCount][0] = selectedRow;
                                undoMoves[undoCount][1] = selectedCol;
                                undoMoves[undoCount][2] = sudokuGrid[selectedRow][selectedCol]; // Store previous value
                                undoCount++;
                                sudokuGrid[selectedRow][selectedCol] = 7;
                                correctHighlight.setPosition(174 + selectedCol * 49.9f, 75 + selectedRow * 50.f);
                                playerScore += 10;  // Increase score
                                scoreText.setString("Score: " + to_string(playerScore));
                                window.draw(correctHighlight);
                                window.display();
                                this_thread::sleep_for(chrono::milliseconds(300));
                            } else {
                                mistakes++;
                                mistakeText.setString("Mistakes Left: " + to_string(maxMistakes - mistakes));
                                wrongHighLight.setPosition(174 + selectedCol * 49.9f, 75 + selectedRow * 50.f);
                                window.draw(wrongHighLight);
                                window.display();
                                this_thread::sleep_for(chrono::milliseconds(300));
                            }
                            break;

                        case sf::Keyboard::Num8:
                            if (!isStatic[selectedRow][selectedCol] && Answer[selectedRow][selectedCol] == 8) {
                                undoMoves[undoCount][0] = selectedRow;
                                undoMoves[undoCount][1] = selectedCol;
                                undoMoves[undoCount][2] = sudokuGrid[selectedRow][selectedCol]; // Store previous value
                                undoCount++;
                                sudokuGrid[selectedRow][selectedCol] = 8;
                                correctHighlight.setPosition(174 + selectedCol * 49.9f, 75 + selectedRow * 50.f);
                                playerScore += 10;  // Increase score
                                scoreText.setString("Score: " + to_string(playerScore));
                                window.draw(correctHighlight);
                                window.display();
                                this_thread::sleep_for(chrono::milliseconds(300));
                            } else {
                                mistakes++;
                                mistakeText.setString("Mistakes Left: " + to_string(maxMistakes - mistakes));
                                wrongHighLight.setPosition(174 + selectedCol * 49.9f, 75 + selectedRow * 50.f);
                                window.draw(wrongHighLight);
                                window.display();
                                this_thread::sleep_for(chrono::milliseconds(300));
                            }
                            break;

                        case sf::Keyboard::Num9:
                            if (!isStatic[selectedRow][selectedCol] && Answer[selectedRow][selectedCol] == 9) {
                                undoMoves[undoCount][0] = selectedRow;
                                undoMoves[undoCount][1] = selectedCol;
                                undoMoves[undoCount][2] = sudokuGrid[selectedRow][selectedCol]; // Store previous value
                                undoCount++;
                                sudokuGrid[selectedRow][selectedCol] = 9;
                                correctHighlight.setPosition(174 + selectedCol * 49.9f, 75 + selectedRow * 50.f);
                                playerScore += 10;  // Increase score
                                scoreText.setString("Score: " + to_string(playerScore));
                                window.draw(correctHighlight);
                                window.display();
                                this_thread::sleep_for(chrono::milliseconds(300));
                            } else {
                                mistakes++;
                                mistakeText.setString("Mistakes Left: " + to_string(maxMistakes - mistakes));
                                wrongHighLight.setPosition(174 + selectedCol * 49.9f, 75 + selectedRow * 50.f);
                                window.draw(wrongHighLight);
                                window.display();
                                this_thread::sleep_for(chrono::milliseconds(300));
                            }
                            break;
                            //undo part
                        case sf::Keyboard::U:
                            if (undoCount > 0) {
                                cout << "sdsfs";
                                // Decrement undoCount first to point to the last move
                                undoCount--;
                                int row = undoMoves[undoCount][0];
                                int col = undoMoves[undoCount][1];
                                int prevValue = undoMoves[undoCount][2];
                                cout << "sdsfs";

                                // Restore the previous value in the grid
                                sudokuGrid[row][col] = prevValue;

                                // Update the visual representation
                                if (prevValue == 0) {
                                    cout << "sdsf1s";

                                    numbers[row][col].setString(""); // Clear the cell
                                } else {
                                    cout << "sdsfs";
                                    numbers[row][col].setString(to_string(prevValue)); // Restore the number
                                }
                            }
                            break;
                            //help part
                        case sf::Keyboard::H:
                            abbasgholi = 0;
                            if (HCount > 0) {
                                srand(time(0));
                                for (int i = rand() % 9; i < 9; i++) {
                                    for (int j = 0; j < 9; j++) {
                                        if (sudokuGrid[i][j] == 0) {
                                            abbasgholi = 1;
                                            sudokuGrid[i][j] = Answer[i][j];
                                            HCount--;
                                            break;
                                        }
                                    }
                                    if (abbasgholi) {
                                        break;
                                    }
                                }
                                if (!abbasgholi) {
                                    for (int i = 0; i < 9; i++) {
                                        for (int j = 0; j < 9; j++) {
                                            sudokuGrid[i][j] == Answer[i][j];
                                            HCount--;
                                            break;
                                        }
                                    }
                                }
                                hintText.setString("Hints Left: " + to_string(HCount));
                            }
                    }
                    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                        float mouseX = event.mouseButton.x;
                        float mouseY = event.mouseButton.y;

                        // Check if the click is inside the Sudoku grid
                        if (mouseX >= startX && mouseX <= startX + 450 && mouseY >= startY && mouseY <= startY + 450) {

                            // Calculate the clicked cell
                            selectedCol = (mouseX - startX) / 50;
                            selectedRow = (mouseY - startY) / 50;
                        }
                    }
                }
            }

        }
        window.clear(background_color);
        if (gameState == LOGIN) {

            // Draw Scanline Effect
            for (int y = 0; y < window.getSize().y; y += lineSpacing) {
                sf::RectangleShape scanline(sf::Vector2f(window.getSize().x, lineThickness));
                scanline.setFillColor(scanlineColor);
                scanline.setPosition(0, y);
                window.draw(scanline);
            }
            cout << "hi1" << endl;
            loginText.update();
            instructionText.update();
            window.draw(loginText.sfText);
            window.draw(inputText);
            window.draw(instructionText.sfText);

        } else if (gameState == PASSWORD_ENTRY) {
            // Draw Scanline Effect
            for (int y = 0; y < window.getSize().y; y += lineSpacing) {
                sf::RectangleShape scanline(sf::Vector2f(window.getSize().x, lineThickness));
                scanline.setFillColor(scanlineColor);
                scanline.setPosition(0, y);
                window.draw(scanline);
            }
            sf::Text passwordPrompt("Enter Password:", font, 30);
            passwordPrompt.setPosition(280, 220);
            passwordPrompt.setFillColor(shapes_color);

            sf::Text passwordDisplay(string(playerPassword.length(), '*'), font, 30);
            passwordDisplay.setPosition(350, 250);
            passwordDisplay.setFillColor(shapes_color);

            window.draw(passwordPrompt);
            window.draw(passwordDisplay);

            if (incorrectPassword) {
                sf::Text incorrectText("Incorrect password! Press ESC to retry.", font, 30);
                incorrectText.setPosition(200, 400);
                incorrectText.setFillColor(sf::Color::Red);
                window.draw(incorrectText);
            }
        } else if (gameState == MENU) {
            // Draw Scanline Effect
            for (int y = 0; y < window.getSize().y; y += lineSpacing) {
                sf::RectangleShape scanline(sf::Vector2f(window.getSize().x, lineThickness));
                scanline.setFillColor(scanlineColor);
                scanline.setPosition(0, y);
                window.draw(scanline);
            }
            cout << "hi3 \n";
            if (!enteringPassword) { // Prevent showing MENU before login
                menuText.update();
                window.draw(menuText.sfText);

                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Num1) {
                        gameState = GAME;
                    } else if (event.key.code == sf::Keyboard::Num2) {
                        gameState = LOAD_GAME;
                    } else if (event.key.code == sf::Keyboard::Num3) {
                        loadScores();
                        gameState = SCORESHEET;
                    } else if (event.key.code == sf::Keyboard::Num4) {
                        window.close();
                    }
                }
            }
        } else if (gameState == LOAD_GAME) {
            cout << "hi4 \n";
            // Draw Scanline Effect
            for (int y = 0; y < window.getSize().y; y += lineSpacing) {
                sf::RectangleShape scanline(sf::Vector2f(window.getSize().x, lineThickness));
                scanline.setFillColor(scanlineColor);
                scanline.setPosition(0, y);
                window.draw(scanline);
            }

            sf::Text loadText("Press ESC to return", font, 20);
            loadText.setPosition(350, 500);
            loadGameText.update();
            window.draw(loadGameText.sfText);

            for (int i = 0; i < savedGameCount; i++) {

                sf::Text gameEntry(savedGames[i], font, 24);
                gameEntry.setPosition(250, 250 + (i * 30));
                window.draw(gameEntry);
            }
            window.draw(loadText);
        } else if (gameState == SCORESHEET) {
            window.clear(sf::Color::Black);

            // Draw Scanline Effect
            for (int y = 0; y < window.getSize().y; y += lineSpacing) {
                sf::RectangleShape scanline(sf::Vector2f(window.getSize().x, lineThickness));
                scanline.setFillColor(scanlineColor);
                scanline.setPosition(0, y);
                window.draw(scanline);
            }
            // Title
            TypewriterText scoreHeader(
                    "Score Sheet",
                    font,
                    30,
                    shapes_color,
                    sf::Vector2f(50, 50),
                    20
            );
            scoreHeader.update();
            window.draw(scoreHeader.sfText);

            // Column Headers
            sf::Text columnHeaders("Player       Score       Result      Difficulty", font, 24);
            columnHeaders.setPosition(150, 120);
            columnHeaders.setFillColor(shapes_color);
            window.draw(columnHeaders);

            // Draw top horizontal divider
            sf::RectangleShape topLine(sf::Vector2f(500, 2));
            topLine.setFillColor(shapes_color);
            topLine.setPosition(140, 150);
            window.draw(topLine);

            // Draw each score entry
            for (int i = 0; i < scoreCount; i++) {
                stringstream ss(scores[i]);
                string playerName, result, difficulty;
                int playerScore;
                ss >> playerName >> playerScore >> result >> difficulty; // Extract data

                sf::Text nameText(playerName, font, 20);
                nameText.setPosition(150, 170 + (i * 30));
                nameText.setFillColor(shapes_color);
                window.draw(nameText);

                sf::Text scoreText(to_string(playerScore), font, 20);
                scoreText.setPosition(280, 170 + (i * 30));
                scoreText.setFillColor(shapes_color);
                window.draw(scoreText);

                sf::Text resultText(result, font, 20);
                resultText.setPosition(400, 170 + (i * 30));
                resultText.setFillColor(result == "Win" ? sf::Color::Green : sf::Color::Red); // Color based on win/loss
                window.draw(resultText);

                sf::Text difficultyText(difficulty, font, 20);
                difficultyText.setPosition(530, 170 + (i * 30));
                difficultyText.setFillColor(shapes_color);
                window.draw(difficultyText);

                // Draw horizontal divider between rows
                sf::RectangleShape rowLine(sf::Vector2f(500, 1));
                rowLine.setFillColor(shapes_color);
                rowLine.setPosition(140, 195 + (i * 30));
                window.draw(rowLine);
            }

            // ma ke raftim
            TypewriterText backText(
                    "Press ESC to return",
                    font,
                    20,
                    shapes_color,
                    sf::Vector2f(50, 500),
                    20
            );



        } else if (gameState == LOAD_GAME) {
            loadGameText.update();
            window.draw(loadGameText.sfText);
            sf::Text saveFileText("Press ENTER to load last saved game", font, 24);
            saveFileText.setPosition(250, 350);
            saveFileText.setFillColor(sf::Color::White);
            window.draw(saveFileText);
        }


        else if (gameState == DIFFICULTY_SELECTION) {
            // Draw Scanline Effect
            for (int y = 0; y < window.getSize().y; y += lineSpacing) {
                sf::RectangleShape scanline(sf::Vector2f(window.getSize().x, lineThickness));
                scanline.setFillColor(scanlineColor);
                scanline.setPosition(0, y);
                window.draw(scanline);
            }
            difficultyText.update();
            window.draw(difficultyText.sfText);

        }else if (gameState == GAME) {
            if (!gameOver) {
                if ((sudokoSolved)) {
                    saveScore(playerNames, playerScore, true, selectedDifficulty);
                    loadScores();
                    gameState = SCORESHEET;
                }

                if (mistakes >= 5 || timeLeft <= 0) {
                    gameOver = true;
                    cout << "Game Over! You lost.\n";
                    saveScore(playerNames, playerScore, false, selectedDifficulty);
                    loadScores();
                    gameState = SCORESHEET;
                }
            }

            //timer wroking
        if (gameClock.getElapsedTime().asSeconds() >= 1 && !isPaused && !gameOver) {
            timeLeft--;
            gameClock.restart();
            if (timeLeft <= 0) {
                cout << "Time's up! Game Over." << endl;
                break; // Exit the loop when time runs out
            }
        }
            // Draw Scanline Effect
            for (int y = 0; y < window.getSize().y; y += lineSpacing) {
                sf::RectangleShape scanline(sf::Vector2f(window.getSize().x, lineThickness));
                scanline.setFillColor(scanlineColor);
                scanline.setPosition(0, y);
                window.draw(scanline);
            }
            //  Display Timer
            int minutes = timeLeft / 60;
            int seconds = timeLeft % 60;
            stringstream ss;
            ss << "Time Left: " << minutes << ":" << (seconds < 10 ? "0" : "") << seconds;
            timerText.setString(ss.str());

            // Update numbers in the grid
            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    if (sudokuGrid[i][j] != 0) {
                        numbers[i][j].setString(to_string(sudokuGrid[i][j]));
                    } else {
                        numbers[i][j].setString(""); // Clear the text if the cell is empty
                    }
                }
            }

            // Draw Sudoku Grid
            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    window.draw(Sudoku_grid[i][j]); // Draw grid cells
                    window.draw(numbers[i][j]);    // Draw numbers inside cells
                }
            }

            // Highlight selected cell
            if (selectedRow != -1 && selectedCol != -1) {
                selectedCellHighlight.setPosition(startX + selectedCol * 50.f, startY + selectedRow * 50.f);
                window.draw(selectedCellHighlight);
            }
            //draw ui of timer and hind and mistake and score
            window.draw(timerText);
            window.draw(hintText);
            window.draw(mistakeText);
            window.draw(scoreText);

            if (gameOver) {
                if (sudokoSolved) {
                    window.draw(winText);
                } else {
                    window.draw(loseText);
                }
            }

            // Draw Pause Screen
            if (isPaused) {
                window.draw(pauseText);
            }
        }else if (gameState == STORY) {
            // Draw Scanline Effect
            for (int y = 0; y < window.getSize().y; y += lineSpacing) {
                sf::RectangleShape scanline(sf::Vector2f(window.getSize().x, lineThickness));
                scanline.setFillColor(scanlineColor);
                scanline.setPosition(0, y);
                window.draw(scanline);
            }
            storyText.update();
            window.draw(storyText.sfText);
        }
        window.display();
    }
    return 0;
}