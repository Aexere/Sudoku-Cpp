#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <filesystem>
#include <cstring>

using namespace std;

#define C 9
// Change this if needed different file path
string filePath = "/Users/aexere/Desktop/SudokuGames/";

//global variables
int sGrid[9][9]; // stores solved grid
int eGrid[9][9]; // stores grid for players to solve
int tempGrid[9][9]; // stores temp grid where grid is emptied
int timeoutCounter = 0;
bool timeoutError = false;
int difficulty;

string optionsInput;
bool done = false;
bool cheatActive = false;

void playGame();
void print(int);
void saveGame();
void resumeGame();
bool isNumberSafe(int, int, int, int);
bool validateSudoku(int, int, int, int);
void generateSudoku(int, int, int, int);
void showGrid(int);
void loadGame();
void previousGames();

//show cheat grid
void print(int arr[9][9])
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
            cout << arr[i][j] << " ";
        cout << endl;
    }
}

//checks if number is safe to be placed into the grid space
bool isNumberSafe(int tGrid[9][9], int row, int col, int num)
{
        //check for same number in the same row, if found return false
        for (int x = 0; x < 9; x++)
        {
            if(tGrid[row][x] == num)
                return false;
        }
        
        //check for same number in the same collumn, if found return false
        for (int x = 0; x < 9; x++)
        {
            if(tGrid[x][col] == num)
                return false;
        }
        
        //check for same number in the same 3 by 3 grid
        int rowStart = row - row % 3, colStart = col - col % 3;
        
        for (int m = 0; m < 3; m++){
            for (int n = 0; n < 3; n++){
                if (tGrid[m + rowStart][n + colStart] == num){
                    return false;
                }
            }
        }
        return true;
}

bool validateSudoku(int tGrid[9][9], int row, int col){
    
    if(row == 9 - 1 && col == 9)
        return true;
    
    if(col == 9) {
        row++;
        col = 0;
    }
    
    if(tGrid[row][col] > 0){
        return validateSudoku(tGrid, row, col + 1);
    }
    
    // check for time out / infinite loop error
    timeoutCounter++;
    if (timeoutCounter == 10000)
    {
        cout << "ERROR: timed out\n";
        return false;
    }
    
    for (int num = 1; num <= 9; num++)
        {
            
            if (isNumberSafe(tGrid, row, col, num))
                {
                    tGrid[row][col] = num;
                    if (validateSudoku(tGrid, row, col + 1))
                    return true;
                }
            tGrid[row][col] = 0;
        }
    return false;
}

void generateSudoku(){
    
    //initialise grid
    int gGrid[9][9];
    //write an empty grid
    for(int a = 0; a < 9; a++){
        for(int b = 0; b < 9; b++){
            gGrid[a][b] = 0;
        }
    }

    // loop through each row fill with a random number in each column as a seed at each row
    for (int i = 0; i < 3; i++){
        int rowRandLoc = rand()%8+0;
        int randNum = rand()%9+1;
        gGrid[i][rowRandLoc] = randNum;
    }
    
    //write generated gGrid(generated grid) to sGrid(solved grid)
    for(int a = 0; a < 9; a++){
        for(int b = 0; b < 9; b++){
           sGrid[a][b] = gGrid[a][b]; //writes local grid into the main grid
        }
    }
    
    validateSudoku(sGrid, 0, 0);
   // print(sGrid);
    
    //write solved sGrid to eGrid(empty player grid)
    for(int a = 0; a < 9; a++){
        for(int b = 0; b < 9; b++){
           eGrid[a][b] = sGrid[a][b]; // writes generated grid into the player grid
        }
    }
    
    if(!cheatActive){
        //remove some numbers from eGrid
        for(int a = 0; a < 9; a++){
            for(int b = 0; b < 9; b++){
                int chanceTemp = rand()%difficulty;
                if(chanceTemp == 1)
                    eGrid[a][b] = 0; //randomly removes some numbers from the grid
            }
        }
    }else{
    eGrid[8][8] = 0;
    }
}

//Print sudoku grid
void showGrid(int dGrid[9][9]){

//Adds X coordinate on top
    cout << "    "; //Create space to line up with the grid
    int spacer = 1;
    for (int i = 1; i <= C; i++){
        cout << i;
        
        //seperate into 3 numbers
        if(spacer == 3 || spacer == 6)
              cout << " |";
           cout <<" ";
        spacer++;
    }

    //add a line spacer between coordinate and grid
    cout << "\n";
    for(int i = 0; i < C; i++)
       cout << "---";
    cout << "\n";
    
//show grid and add the Y coordinate
    int xCoor = 1;
    for (int row = 0; row < C; row++){
        cout << xCoor << " | ";
        
        
        //Fill in the sudoku grid
        for (int col = 0; col < C; col++){
           if(col == 3 || col == 6)
              cout << "| ";
           cout << dGrid[row][col] <<" ";
          }
        
        if(row == 2 || row == 5){ //check for row 2/5 and place a devider
           cout << endl;
           for(int i = 0; i < C; i++)
              cout << "---";
        }
        
        xCoor++;
        cout << endl;
       }
}

// gets inputs from a console
void consoleInput(int cGrid[9][9]) {
    
    int iCoordY; //input of Y coordinate of the grid
    int iCoordX; //input of X coordinate of the grid
    int iUserNum; //input of number changed
    
    cout << "\n";
    
    cout << "Please enter coordinates of number to change \n";
    cout << "TOP X = ";
    cin >> iCoordX;
    
    //validation of intput for X coordinate
    while(true){
        if (iCoordX < 1 || iCoordX > 9)
        {
            cout << "Enter X coordinate between 1..9\n";
            cin >> iCoordX;
        } else{
            break;
        }
    }
    
    cout << "LEFT Y = ";
    cin >> iCoordY;
    
    //validation of intput for Y coordinate
    while(true){
        if (iCoordY < 1 || iCoordY > 9)
        {
            cout << "Enter Y coordinate between 1..9\n";
            cin >> iCoordY;
        } else{
            break;
        }
    }
    
    cout << "You have selected ";
    cout << cGrid[iCoordY - 1][iCoordX - 1] << "\n";
    
    cout << "Change number: ";
    cin >> iUserNum;
    
    //validation of intput for Y coordinate
    while(true){
        if (iUserNum < 1 || iUserNum >9)
        {
            cout << "Enter your chosen number again between 1..9\n";
            cin >> iUserNum;
        } else{
            break;
        }
    }
    
    //write user input into the grid Array at location
    cGrid[iCoordY - 1][iCoordX - 1 ] = iUserNum;
    
    cout << "TYPE OPTIONS: 'undo' 'save' 'done' HIT 's' and ENTER TO SKIP \n";
    cin >> optionsInput;
    
    if(optionsInput == "done"){
        done = true;
        //write user input grit back into the player eGrid
        for(int a = 0; a < 9; a++){
            for(int b = 0; b < 9; b++){
               eGrid[a][b] = cGrid[a][b]; // writes generated grid into the player grid
            }
        }
    }
    
    if(optionsInput == "save")
        saveGame();
    
    //if(optionsInput == "undo")
        
    // this is for testing to show what stored in after user input
   // cout << "You have changed to ";
    //cout << sGrid[iCoordY -1][iCoordX -1] << "\n\n";
}

//checks the game grids to see if the user completed it correctly or not
bool checkEndGame(int userGrid[9][9], int generatedGrid[9][9]){
    int checkCounter = 0;
    for(int a = 0; a < 9; a++){
        for(int b = 0; b < 9; b++){
            if(userGrid[a][b] == generatedGrid[a][b]){
                checkCounter++;
            }
        }
    }
    if(checkCounter == 81){
        return true;
    }
    return false;
}

//chose difficulty of sudoku
void chooseLevel(){
    
    int level;
    
    // output choices for the player
    cout << "Please choose difficulty: " << endl;
    cout << "1 for EASY" << endl;
    cout << "2 for MEDIUM" << endl;
    cout << "3 for HARD" << endl;
    cout << "4 for CHEAT" << endl;
    
    //take input
    cout << "\nYour choice: ";
    cin >> level;
    
    //validation of intput for Y coordinate
    while(true){
        if (level <= 0 || level >=5)
        {
            cout << "Enter your choice again between 1 and 3: ";
            cin >> level;
        } else{
            break;
        }
    }
    
    // asssing difficulty based on chosen difficulty
    if(level == 1)
        difficulty = 10;
    if(level == 2)
        difficulty = 7;
    if(level == 3)
        difficulty = 2;
    //DEBUG
    if(level == 4)
        cheatActive = true;
}


void userMenu(){
    int choice;
    // output choices for the player
    cout << "Please choose one of the options: " << endl;
    cout << "1 for PLAY" << endl;
    cout << "2 for PREVIOUS GAMES" << endl;

    //take input
    cout << "\nYour choice: ";
    cin >> choice;
    
    //validation of intput for Y coordinate
    while(true){
        if (choice < 0 || choice >5)
        {
            cout << "Enter your choice again between 1 and 2: ";
            cin >> choice;
        } else{
            break;
        }
    }
    
    // based on choice
    if(choice == 1)
        playGame();
      
   if(choice == 2)
       previousGames();

}



//controls the gameplay of game.
void playGame(){
    
    // choose a level
    chooseLevel();
    
    // run sudoku generation
    generateSudoku();
    
    //sudoku input
    while(done == false) {
    //system("clear");
    showGrid(eGrid);
    consoleInput(eGrid);
    }
    
    //check for results and return true or false depending on answer
    if(checkEndGame(eGrid, sGrid)){
        //system("Color 0A");
        cout << "Congrats you won!" << endl;
    }else{
        cout << "You lost sudoku is incorrect" << endl;
    }
}

//TODO - write a check for user input
//saves a game result into a selected file
void saveGame(){
    string fileNameIn = "temp"; //holds user
    string fileFormat = ".txt"; //holds format type
    string fileNameResult; //holds combines user input format and file path to the file
    string overwriteCheck; // asks again incase of over ride
    int numCounter = 0;
    
    // checks if user will overwrite their current game and accepts.
    //if no overwrite is detected it continues saving file.
    while(true){
    //ask and take input from user
    cout << "Name of your file (no spaces): ";
    cin >> fileNameIn;
    cout << "\n";
    
    //combines path, user input and file format into one variable
    fileNameResult = filePath + fileNameIn + fileFormat;
    
    //check if file exists
    ifstream fileCheck;
    fileCheck.open(fileNameResult);
    if(fileCheck) {
       cout<<"File exists do you want to overwrite it? y/n" << endl;
        cout << "Your choice: ";
        
        cin >> overwriteCheck;
        
        //check for user to decide to overwrite file or not
        if(overwriteCheck == "y"){
            break;
        }
    }else{
        break;
    }
    fileCheck.close();
    }
    
    // open file to output into
    ofstream saveFile;
     saveFile.open(fileNameResult);
    
    // write user input grit back into the player eGrid
    for(int a = 0; a < 9; a++){
        for(int b = 0; b < 9; b++){
            numCounter++;
           saveFile << eGrid[a][b]; // writes generated grid into the player grid
            if(numCounter != 81)
                saveFile << "\n";
        }
        
    }
    
     // saveFile << "Writing this to a file.\n" << endl;
     saveFile.close();
    
    // save file names into a txt with list of file names
    ofstream saveFileNames;
    saveFileNames.open(filePath + "savedgamenames.txt", fstream::app);
    saveFileNames << "\n";
    saveFileNames << fileNameIn;
    saveFileNames.close();
    
    userMenu();
}

//loads game from file
void previousGames(){
    string getNames;
    
    cout << "Please choose one of your saved games: " << endl;
    // load names of saved games from file
    ifstream fileNames;
    fileNames.open(filePath + "savedgamenames.txt");
    
    //loop through file until end and display names into console
    while(!fileNames.eof()){
        getline(fileNames, getNames);
        cout << getNames << endl;
    }
    
    //close file
    fileNames.close();
    loadGame();
    
}

void loadGame(){
    string gameName;
    string outputStr;
    int outputNum;
    int counterX = 0;
    int counterY = 0;
    int x = 0;
    int y = 0;
    
    cout << "\nPlease type game name to play: ";
    cin >> gameName;
    
    // load names of saved games from file
    ifstream gameData;
    gameData.open(filePath + gameName.append(".txt"));
    
    //loop through file until end and display names into console
    while(!gameData.eof()){
        
        getline(gameData, outputStr);
        //convert string to int
        outputNum = stoi(outputStr);
        
        if (counterX == 9){
            x++;
            counterX = 0;
        }
        
        if (counterY == 9){
            counterY = 0;
        }
        
        eGrid[x][y] = outputNum;
        y++;
   }
    resumeGame();
}

void resumeGame(){
    
    //sudoku input
    while(done == false) {
    //system("clear");
    showGrid(eGrid);
    consoleInput(eGrid);
    }
    
    //check for results and return true or false depending on answer
    if(checkEndGame(eGrid, sGrid)){
        //system("Color 0A");
        cout << "Congrats you won!" << endl;
    }else{
        cout << "You lost sudoku is incorrect" << endl;
    }
}

//main app system
int main(int argc, const char * argv[]) {
    
    // making sure random is random each time
    srand((int)time(0));
    
    //sudoku title
    cout << "Sudoku Hell\n\n";
    //menu
    userMenu();

    /*TODO: have an automatic detection of completed game and create an end game screen for player to choose to either
    TODO: save a game, go back to menu or quick load qnother game
    
    */
    return 0;
}
