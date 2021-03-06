// BATTLESHIP.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "board.h"
#include "ship.h"
#include "battleship.h"
#include <iostream>
#include <string>
#include <windows.h>

#define MAX_COORDINATES_ROW 5
#define MAX_COORDINATES_COL 2

using namespace std;

int main() {
    Board waterboardOne;
    Board waterboardTwo;
    Board targetboardOne;
    Board targetboardTwo;
    Ship patrolBoatOne("Patrol Boat", 2);
    Ship frigateOne("Frigate", 3);
    Ship submarineOne("Submarine", 3);
    Ship battleshipOne("Battleship", 4);
    Ship aircraftCarrierOne("Aircraft Carrier", 5);
    Ship shipArrayOne[5] = { patrolBoatOne, frigateOne, submarineOne, battleshipOne, aircraftCarrierOne };
    Ship patrolBoatTwo("Patrol Boat", 2);
    Ship frigateTwo("Frigate", 3);
    Ship submarineTwo("Submarine", 3);
    Ship battleshipTwo("Battleship", 4);
    Ship aircraftCarrierTwo("Aircraft Carrier", 5);
    Ship shipArrayTwo[5] = { patrolBoatTwo, frigateTwo, submarineTwo, battleshipTwo, aircraftCarrierTwo };
    bool gameOver = false;
    int shipCoordinates[MAX_COORDINATES_ROW][MAX_COORDINATES_COL] = { { -1 } };

    Battleship newGame;

    // Ship Placement for Player one
    newGame.playerShipPlacement(waterboardOne, shipCoordinates, shipArrayOne, "one");
    // Ship Placement for Player two
    newGame.playerShipPlacement(waterboardTwo, shipCoordinates, shipArrayTwo, "two");


    // Game flow logic
    while (!gameOver) {
        // Player one's turn
        gameOver = newGame.takeTurnsFiring(waterboardOne, waterboardTwo, targetboardOne, shipArrayTwo, "one");
        if (gameOver) {
            break;
        }

        // Player two's turn
        gameOver = newGame.takeTurnsFiring(waterboardTwo, waterboardOne, targetboardTwo, shipArrayOne, "two");
    }

    return 0;
}

Battleship::Battleship(){

}

bool Battleship::takeTurnsFiring(Board &ownWaterboard, Board &enemyWaterboard, Board &targetboard, Ship enemyShips[5], string player) {
    bool fireSucess = false;
    bool gameOver = false;

    while (!fireSucess) {
        fireSucess = launchMissile(ownWaterboard, enemyWaterboard, targetboard, enemyShips, player);

        if (!fireSucess) {
            cout << "***INCORRECT COORDINATES! Please enter in a legal coordinate that you have not already fired at.\n";
        }
    }

    gameOver = checkVictory(enemyShips);
    Sleep(1000);
    clearScreen();

    if (gameOver) {
        cout << "Play " << player << " has won!!!!!";
        Sleep(1000);
        return true;
    }
    return false;
}

// Ship Placement for Players at the beginning of the game
void Battleship::playerShipPlacement(Board &waterboard, int(&shipCoordinates)[MAX_COORDINATES_ROW][MAX_COORDINATES_COL], Ship shipArray[5], string player) {
    string coordinateOne;
    string coordinateTwo;

    clearScreen();
    cout << "Welcome to Battleship Player " << player << "!  Time to choose ship placement\n";
    for (int shipCount = 0; shipCount < 5; shipCount++) {
        bool acceptableInputBounds = false; // boolean to confirm user input is in bounds
        bool acceptableInputBlocked = false; // boolean to confirm user input is in empty square
        int arraySize;
        while (!acceptableInputBounds || !acceptableInputBlocked) {
            arraySize = 0;
            waterboard.drawBoard();
            Ship currentShip = shipArray[shipCount];
            getUserCoordiantes(coordinateOne, coordinateTwo, currentShip);
            acceptableInputBounds = parseInputCoordinates(shipCoordinates, arraySize, currentShip.getSize(), coordinateOne, coordinateTwo);
            acceptableInputBlocked = shipAlreadyExists(waterboard, shipCoordinates, arraySize);

            if (!acceptableInputBounds || !acceptableInputBlocked) {
                cout << "***INCORRECT INPUTS! Please enter in legal coortinates (" << currentShip.getSize() << " spaces long)\n";
                acceptableInputBounds = false;
                acceptableInputBlocked = false;
            }
            else {
                cout << "Your " << currentShip.getName() << " is at coordinates " << coordinateOne << ", " << coordinateTwo << ".\n";
                Sleep(1000);
                clearScreen();
            }
        }
        placeShipOnBoard(waterboard, shipCoordinates, arraySize);
        shipArray[shipCount].setPosition(shipCoordinates);
        memset(shipCoordinates, -1, sizeof(shipCoordinates));
    }
}

bool Battleship::checkVictory(Ship shipArray[5]) {
    int counter = 0;
    for (int i = 0; i < 5; i++) {
        if (shipArray[i].isSunk()) {
            counter++;
            if (counter == 5) {
                return true;
            }
        }
    }
    return false;
}

bool Battleship::launchMissile(Board &ownWaterboard, Board &enemyWaterboard, Board &Targetboard, Ship shipArray[5], string player) {
    string fireCoordinates;
    int row;
    int col;
    bool boundsCheck = false;

    cout << "      ***Enter target coordinates.\n";
    cout << "      Ships remaining in enemy fleet: " << enemyWaterboard.getShipsRemaining() << "\n\n";
    Targetboard.drawBoard();
    cout << "\n\n   ***Allied fleet configuration (Player " << player << ").\n";
    cout << "            Allied ships remaining: " << ownWaterboard.getShipsRemaining() << "\n\n";
    ownWaterboard.drawBoard();
    cin >> fireCoordinates;

    // Checks the input for being in bounds and maps the letter input to its corresponding integer value
    boundsCheck = mapInput(fireCoordinates, row, col);
    if (!boundsCheck) {
        return false;
    }

    // If a hit
    if (enemyWaterboard.hasShip(row, col) && !enemyWaterboard.alreadyFired(row, col)) {
        enemyWaterboard.setStatus(row, col, HIT);
        Targetboard.setStatus(row, col, HIT);

        for (int i = 0; i < 5; i++) {
            if (shipArray[i].inPosition(row, col)) {
                bool shipSunk = false;
                shipSunk = shipArray[i].hit();
                cout << "HIT!!!\n";
                if (shipSunk) {
                    enemyWaterboard.sunkEnemyShip();
                }
                return true;
            }
        }
    }
    // If a miss
    else if (!enemyWaterboard.hasShip(row, col) && !enemyWaterboard.alreadyFired(row, col)) {
        enemyWaterboard.setStatus(row, col, MISS);
        Targetboard.setStatus(row, col, MISS);
        cout << "Miss\n";
        return true;
    }
    return false;
}

// Takes the coordinates for a ship and places it on the board
void Battleship::placeShipOnBoard(Board &waterboard, int(&shipCoordinates)[MAX_COORDINATES_ROW][MAX_COORDINATES_COL], int arraySize) {
    for (int i = 0; i < arraySize; i++) {
        waterboard.setShip(shipCoordinates[i][0], shipCoordinates[i][1]);
    }
}

// Checks for the existance of a ship
bool Battleship::shipAlreadyExists(Board &waterboardOne, int(&shipCoordinates)[MAX_COORDINATES_ROW][MAX_COORDINATES_COL], int arraySize) {

    for (int i = 0; i < arraySize; i++) {
        if (waterboardOne.hasShip(shipCoordinates[i][0], shipCoordinates[i][1])) {
            return false;
        }
    }
    return true;
}

// Takes in two endpoints passed in by the user, checks them for validity (returns true), adds them to array 'shipCoordinates',
// fills in the array 'shipCoordinates' with any coordinates in between the two points, and returns a reference to the array and its size
bool Battleship::parseInputCoordinates(int(&shipCoordinates)[MAX_COORDINATES_ROW][MAX_COORDINATES_COL], int &arraySize, int shipSize, string coordinateOne, string coordinateTwo) {
    int row1, row2;
    int col1, col2;
    bool boundsCheck = false;

    // Checks the input for being in bounds and maps the letter input to its corresponding integer value
    boundsCheck = mapInput(coordinateOne, row1, col1);
    if (!boundsCheck) {
        return false;
    }
    boundsCheck = mapInput(coordinateTwo, row2, col2);
    if (!boundsCheck) {
        return false;
    }

    // Array of first set of coordinates
    shipCoordinates[0][0] = row1;
    shipCoordinates[0][1] = col1;
    arraySize++;

    // Array of second set of coordinates
    shipCoordinates[1][0] = row2;
    shipCoordinates[1][1] = col2;
    arraySize++;

    // If placing the ship horizontally
    if (row1 == row2) {
        fillInCoordinates(shipCoordinates, arraySize, row1, col1, col2, true);
    }
    // Else placing the ship vertically
    else if (col1 == col2) {
        fillInCoordinates(shipCoordinates, arraySize, col1, row1, row2, false);
    }
    // Return false if ship is not aligned either horizontally or vertically
    else { 
        return false;
    }

    // If the number of ship coordinates equals the size of the ship then the ship is within bounds
    if (shipSize == arraySize) {
        return true;
    }

    return false;
}

// Fills in the coordinates between two endpoints
void Battleship::fillInCoordinates(int(&shipCoordinates)[MAX_COORDINATES_ROW][MAX_COORDINATES_COL], int &arraySize, int fixedAxis, int changeAxis1, int changeAxis2, bool horizontal) {

    // if changeAxis1 is larger than changeAxis2 then switch their values
    if (changeAxis1 > changeAxis2) {
        int old_col = changeAxis1;
        changeAxis1 = changeAxis2;
        changeAxis2 = old_col;
    }

    int counter = changeAxis1 + 1;
    int index = 2;

    // add in between values to array
    while (counter < changeAxis2) {

        if (horizontal) {
            shipCoordinates[index][0] = fixedAxis;
            shipCoordinates[index][1] = counter;
        }
        else {
            shipCoordinates[index][0] = counter;
            shipCoordinates[index][1] = fixedAxis;
        }

        index++;
        counter++;
        arraySize++;
    }
}

// Takes user input and directly returns whether it is within bounds, and relative references to the return row/col
bool Battleship::mapInput(string inputCoordinates, int &returnRow, int &returnCol) {
    string row;
    string col;

    if (inputCoordinates.find(".") != std::string::npos) {
        return false;
    }

    row = inputCoordinates.substr(0, 1);
    col = inputCoordinates.substr(1, 2);

    int asciiSubtract = upperOrLower(row);

    returnRow = row.at(0) - asciiSubtract;
    returnCol = atoi(col.c_str()) - 1; // subtract one because arrays count from 0

    return acceptableInput(row, returnCol);
}


// Checks that coordinates are in bounds and correct
bool Battleship::acceptableInput(string row, int col) {
    // input was not a letter between 'A' and 'J'
    if (upperOrLower(row) == 0) {
        return false;
    }
    // input was not between one and 10
    else if (!withinBounds(col)) {
        return false;
    }
    return true;
}

// Confirm the user is within bounds of the board
bool Battleship::withinBounds(int value) {
    return ((value >= 0) && (value <= 9));
}

// Method to help map an ASCII char to an int
int Battleship::upperOrLower(string letter) {
    if (letter >= "a" && letter <= "j") {
        return 97;
    }
    else if( letter >= "A" && letter <= "J") {
        return 65;
    }
    return 0;
}

// Method to get user coordinates for ships
void Battleship::getUserCoordiantes(string &coordinateOne, string &coordinateTwo, Ship currentShip) {
    cout << "Time to set your " << currentShip.getName() << "!\n";
    cout << "The " << currentShip.getName() << " is " << currentShip.getSize() << " spaces long. Please enter in first end coordinate(ex.A6).\n";
    cin >> coordinateOne;
    cout << "You entered " << coordinateOne << ". Please enter in the second end coordinate (ex. A9).\n ";
    cin >> coordinateTwo;
}

void Battleship::clearScreen() {
    cout << string(100, '\n');
}