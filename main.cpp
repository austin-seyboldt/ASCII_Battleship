// battleship.cpp
// Austin Seyboldt, CISP 400
// 10/25/2018
#include <iostream>
#include <string>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <fstream>
using namespace std;


enum shipType {NONE, CARRIER, BATTLESHIP, CRUISER, SUBMARINE, DESTROYER};

class Ship;
class LogFile;

// Specification C1 - OOP
class Map
{
  private:
    string map[10][10];   // array for the map boxes
    const static string SHIP_STRING;    // static string that represents the ships on the map
  public:
    Map();
    string getValue(string coordinate);   // returns the string value at the coordinate passed as a string
    string getValue(int number, int letter);  // returns the value at the coordinate passed as ints representing the array structure
    void print() const;     // prints the map
    void placeShip(int number, int letter);   // places a ship at the passed location
    bool checkHit(string);      // returns whether there is a ship at the location
    void setHit(string target);   // sets the location to a hit 'XX'
    void setMiss(string target);  // sets the location to a miss 'OO'
};


// Define what a ship looks like on the map
const string Map::SHIP_STRING = "{}";


class Ship
{
  protected:
    string name;  // the ship's name
    int size;   // the ship's size
    bool isAlive; // whether the ship is alive
  public:
    bool checkAlive(){return isAlive;}  // returns whether the ship is alive
    void updateSize();      // updates the size to size - 1
    int getSize(){return size;}   // returns the ship's size
    string getName(){return name;}    // returns the ship's name
};

class Carrier : public Ship
{
  public:
    Carrier(){name = "Carrier"; size = 5; isAlive = true;}
};

class Battleship : public Ship
{
  public:
    Battleship(){name = "Battleship"; size = 4; isAlive = true;}
};

class Cruiser : public Ship
{
  public:
    Cruiser(){name = "Cruiser"; size = 3; isAlive = true;}
};

class Submarine : public Ship
{
  public:
    Submarine(){name = "Submarine"; size = 3; isAlive = true;}
};

class Destroyer : public Ship
{
  public:
    Destroyer(){name = "Destroyer"; size = 2; isAlive = true;}
};


class Player
{
  protected:
    Map friendlyMap;  // user's record of their ships and enemy hits
    Map enemyMap;     // user's record of hits and misses on enemy ships
    shipType shipLocations[10][10];  // record of ship locations
    int shipsAlive;   // Number of ships currently alive
    string* shotsFired; // Holds the location of shots fired - prohibits repeats
    int numShots;       // holds the num of shots fired
    bool isHuman;       // whether the object is human (the computer is not)
    Ship* carrier;      // ship objects
    Ship* battleship;
    Ship* cruiser;
    Ship* submarine;
    Ship* destroyer;
    bool inShotsFired(string target);   // returns whether the parameter has been fired at
    bool isValidTarget(string);       // returns whether the target passed is a valid one
    string formatCoor(string);      // formats the coordinates
    void updateShotsFired(string target); // updates the shots fired dynamic array
    void updateMissMap(string target);    // updates the missed shots locations on the map
  public:
    Player();
    ~Player();
    void getDefaultShips();   // Places the ships in default locations along the top of board (good for debugging)
    bool placeShip(string coordinate1, string coordinate2, shipType type);  // place a single ship
    void attack(Player*, LogFile&);     // call this to attack the player in the parameter
    void updateFmap(string);            // updates friendlyMap
    void printFmap()const;        // prints the friendlymap
    void printEmap()const;        // prints the enemy map
    Map getBoard()const {return friendlyMap;}
    int getShipSize(shipType)const;   // returns the current size of the ship represented by shipType
    bool areShipsAlive();       // returns whether there are any ships still alive
    string getShipName(shipType)const;    // returns the name of the passed shipType
    int getShipCount()const {return shipsAlive;}  // returns the number of ships still alive
    shipType getShipType(string)const;      // returns the shipType at the string target location passed into it
};


class Computer : public Player
{
  private:
    string* hits;   // array to hold coordinates of successful hits
    string* misses; // array to hold coordinates of missed shots
    string getValidTarget();    // returns a random valid target to fire at
    int numMisses;      // tracks the number of misses
    int numHits;        // tracks the number of hits
    bool inMisses(string target); // returns whether the passed target has been fired at and missed
    bool inHits(string target);    // returns whether the passed target has been fired at and hit
    string getValidCoordinate();  // returns a valid coordinate (for placing ships)
    string getValidCoordinate(string, shipType);  // returns a valid coordinate (for placing ships)
  public:
    Computer();
    ~Computer();
    void placeShips();    // places computer's ships in valid and random locations
    void attack(Player& user, LogFile& file); // attacks the pased player object
    void updateMisses(string);      // updates the misses array
    void updateHits(string);        // updates the hits array
    void printFmap()const;          // prints the friendlymap
};

// Specification B2 - Logfile to disk
// LogFile class writes data to text file
class LogFile
{
  private:
    string* data;     // pointer to a dynamic array holding the passed strings
    int numData;      // number of data items in dynamic array
    string getCurrentTime();  // returns the current time from the system
  public: 
    LogFile();    
    ~LogFile();
    void addData(string);   // adds the passed string to the dynamic arrayy
    void saveData();    // saves the data to an output file called 'log.txt'
};



const string CHEAT_CODE = "ABABXY";   // cheat code to display location of enemy ships
const int FIELD_WIDTH = 47;           // width of the game window

void greeting();
void playGame();
void playMultiFireGame();
void placeShips(Player&);
string getValidCoordinate();
bool isValidCoordinate(string);
void wait();
void wait2();
bool getCheatCode();
string capWord(string);
bool getMultiFire();
void userWon(LogFile&);
void aiWon(LogFile&);


int main() 
{
  greeting();
  if (getMultiFire())
  {
    playMultiFireGame();
  }
  else
    playGame();

  return 0;
}

void greeting()
{
  string lines;
  lines.assign(FIELD_WIDTH, '-');
  cout << lines << endl;
  cout << "\t     Welcome to BATTLESHIP" << endl;
  cout << lines << endl;
  cout << "Ships will be displayed on the board as '{}'.\n"
       << "Hits will be displayed as 'XX'.\n"
       << "Misses will be displayed as 'OO'\n\n"
       << "You will play against the computer, taking\n"
       << "turns firing at each other's ships.\n"
       << "The first player to sink all of the opponent's\n"
       << "ships will win. Good luck." << endl;
  cout << lines << endl;

}

// Play a regular game - entire game loop lives here
void playGame()
{
  bool gameOver = false;
  string lines;
  lines.assign(FIELD_WIDTH, '-');
  Player user;
  Computer* ai = new Computer;
  LogFile file;

  cout << "\n" << lines << endl << "\t\t   NEW GAME\n" << lines << "\n\n";
  
  placeShips(user);
  ai->placeShips();

  // Specification C3 - Secret Option
  // Allow user to enter a cheat code (if they have one) to display ai ship locations
  // Cheat Code is "ABABXY"
  if (getCheatCode())
  {
    ai->printFmap();
    file.addData("User activated cheat code");
  }

  do
  {
    user.attack(ai, file);
    if (ai->areShipsAlive() == false)
    {
      gameOver = true;
      user.printEmap();
      userWon(file);
    }
  
    if (!gameOver)
    {
      user.printEmap();
      wait();
    }
    
    if (!gameOver)
    {
      ai->attack(user, file);
        if (user.areShipsAlive() == false)
        {
          gameOver = true;
          aiWon(file);
        }
      wait2();
    }

  }while(!gameOver);

  file.saveData();
}

// Specification A2 - Multi-fire game
// Play a game in multi-fire mode - entire game loop lives here
void playMultiFireGame()
{
  bool gameOver = false;
  string lines;
  lines.assign(FIELD_WIDTH, '-');
  Player user;
  Computer* ai = new Computer;
  LogFile file;

  cout << "\n" << lines << endl << "\t\t   NEW GAME\n" << lines << "\n\n";
  
  placeShips(user);
  ai->placeShips();

  // Allow user to enter a cheat code (if they have one) to display ai ship locations
  // Cheat Code is "ABABXY"
  if (getCheatCode())
  {
    ai->printFmap();
    file.addData("User activated cheat code");
  }

  do
  {
    for (int shot = 0; shot < user.getShipCount(); shot++)
    {
      if (!gameOver)
      {
        user.attack(ai, file);
        if (ai->areShipsAlive() == false && !gameOver)
        {
          gameOver = true;
          user.printEmap();
          userWon(file);
        }
      }
    }

    if (!gameOver)
    {
      user.printEmap();
      wait();
    }
    
    
    if (!gameOver)
    {
      for (int shot = 0; shot < ai->getShipCount(); shot++)
      {
        if (!gameOver)
        {
          ai->attack(user, file);
          if (user.areShipsAlive() == false && !gameOver)
          {
            gameOver = true;
            aiWon(file);
          }
          wait2();
        }
      }    
    }


  }while(!gameOver);

  file.saveData();
}

// Places all the ships for the passed player (asks for user input and places them as desired)
void placeShips(Player& user)
{
  string coordinate1;
  string coordinate2;
  user.printFmap();
  cout << "\nTo place ships, provide the coordinates of the \n1st box and last box that the ship will occupy." << endl;

  do
  {
    cout << "\nCarrier is 5 spaces large. Where do you want it?" << endl;
    cout << "Coordinate 1:";
    coordinate1 = getValidCoordinate();
    cout << "Coordinate 2:";
    coordinate2 = getValidCoordinate();
  }while (!user.placeShip(coordinate1, coordinate2, CARRIER));

  user.printFmap();

  do
  {
    cout << "\nBattleship is 4 spaces large. Where do you want it?" << endl;
    cout << "Coordinate 1:";
    coordinate1 = getValidCoordinate();
    cout << "Coordinate 2:";
    coordinate2 = getValidCoordinate();
  }while (!user.placeShip(coordinate1, coordinate2, BATTLESHIP));

  user.printFmap();

  do
  {
    cout << "\nCruiser is 3 spaces large. Where do you want it?" << endl;
    cout << "Coordinate 1:";
    coordinate1 = getValidCoordinate();
    cout << "Coordinate 2:";
    coordinate2 = getValidCoordinate();
  }while (!user.placeShip(coordinate1, coordinate2, CRUISER));

  user.printFmap();

  do
  {
    cout << "\nSubmarine is 3 spaces large. Where do you want it?" << endl;
    cout << "Coordinate 1:";
    coordinate1 = getValidCoordinate();
    cout << "Coordinate 2:";
    coordinate2 = getValidCoordinate();
  }while (!user.placeShip(coordinate1, coordinate2, SUBMARINE));

  user.printFmap();

  do
  {
    cout << "\nDestroyer is 2 spaces large. Where do you want it?" << endl;
    cout << "Coordinate 1:";
    coordinate1 = getValidCoordinate();
    cout << "Coordinate 2:";
    coordinate2 = getValidCoordinate();
  }while (!user.placeShip(coordinate1, coordinate2, DESTROYER));

  user.printFmap();
}

// returns valid coordinate
string getValidCoordinate()
{
  string coordinate;

  cin >> coordinate;
  coordinate[0] = toupper(coordinate[0]);
  while(!cin || !isValidCoordinate(coordinate))
  {
    cout << "Coordinates invalid. Please re-enter: ";
    cin.clear();
    cin.ignore(1000, '\n');
    cin >> coordinate;
    coordinate[0] = toupper(coordinate[0]);
  }

  return coordinate;
}

// returns whether the coordinate is valid
bool isValidCoordinate(string target)
{
  bool isValid = true;

  // If 1st character is not A-J, invalid
  if (target[0] < 65 || target[0] > 74)
    isValid = false;
  // If second character is not 0-9, invalid
  if (int(target[1]) - 48 < 0 || int(target[1]) - 48 > 9)
    isValid = false;

  return isValid;
}

// pauses the program (use when buffer is not empty)
void wait()
{
  cout << "\nPress enter to continue\n";
  cin.ignore(1000, '\n');
  cin.get();
}

// pauses the program (use when buffer is empty)
void wait2()
{
  cout << "\nPress enter to continue\n";
  cin.get();
}

// returns whether an entered cheat code is valid
bool getCheatCode()
{
  string code = "";
  bool flag = false;
  cout << "\nIf you have a cheat code, please enter it \nnow or type 'n'" << endl;
  cout << "Code: ";
  cin >>code;
  code = capWord(code);

  while (!cin || (code != "N" && code != CHEAT_CODE))
  {
    cout << "ERROR: Please try again: ";
    cin.clear();
    cin.ignore(1000, '\n');
    cin >> code;
    code = capWord(code);
  }

  if (code == CHEAT_CODE)
    flag = true;
  else
    flag = false;


  return flag;
}

// returns an all-caps version of the passeds string
string capWord(string s)
{
  string temp = "";

  for (int i = 0; i < int(s.length()); i++)
  {
    temp += toupper(s[i]);
  }

  return temp;
}

// returns whether the user wants to play multifire mode
bool getMultiFire()
{
  bool choice = false;
  string select;
  cout << "\nMultiFire game mode allows the active player"
       << "\nto fire once for each ship still alive.\n"
       << "\nActivate multifire mode? (y/n): ";
  cin >> select;
  select = capWord(select);

  while (!cin || (select != "N" && select != "Y"))
  {
    cin.clear();
    cin.ignore(1000, '\n');
    cout << "ERROR: Please type 'y' or 'n': ";
    cin >> select;
    select = capWord(select);
  }

  if (select == "Y")
    choice = true;
  
  return choice;
}

// output for a player victory
void userWon(LogFile& file)
{
  cout << "\nYou destroyed all enemy ships!" << endl;
  cout << "You win!" << endl;
  file.addData("User destroyed all enemy ships");
  file.addData("User wins!");
}

// output for an ai victory
void aiWon(LogFile& file)
{
  cout << "\nThe enemy destroyed all your ships!" << endl;
  cout << "You lost!" << endl;
  file.addData("The enemy destroyed all the user's ships");
  file.addData("The enemy won!");
}

// Constructor populates map array with 'empty' data: "--"
Map::Map()
{
  for(int A = 0; A < 10; A++)
  {
    for(int B = 0; B < 10; B++)
      map[A][B] = "--";
  }
}

string Map::getValue(string coordinates)
{
  string value;
  int letter = int(coordinates[0]) - 65;
  int number = int(coordinates[1]) - 48;

  // If the coordinates are valid, return the value
  if(!(number < 0 || number > 9 || letter < 0 || letter > 9))
  {
    value = map[number][letter];
  }
  // If the coordinates are invalid
  else
  {
    value = "INVALID COORDINATES";
  }

  return value;
}

string Map::getValue(int number, int letter)
{
  string value;

  // If the coordinates are valid, return the value
  if(!(number < 0 || number > 9 || letter < 0 || letter > 9))
  {
    value = map[number][letter];
  }
  // If the coordinates are invalid
  else
  {
    value = "INVALID COORDINATES";
  }

  return value;
}

// Prints the gameboard
void Map::print() const
{
  cout << "\tA   B   C   D   E   F   G   H   I   J" << endl;
  for (int num = 0; num < 10; num++)
  {
    cout << num << "\t";
    for (int i = 0; i < 10; i++)
    {
      cout << map[num][i] << "  "; 
    }
    cout << endl;
  }
}

void Map::placeShip(int number, int letter)
{
  map[number][letter] = "{}";
}

// Cross check the target coordinates with the map to see if there is a hit
bool Map::checkHit(string target)
{
  bool isHit = false;
  int letter = int(target[0]) - 65;
  int number = int(target[1]) - 48;

  if (map[number][letter] == SHIP_STRING)
    isHit = true;
  return isHit;
}

// Update the map to reflect a hit
void Map::setHit(string target)
{
  int letter = int(target[0]) - 65;
  int number = int(target[1]) - 48;

  map[number][letter] = "XX";
}

// Update the map to reflect a miss
void Map::setMiss(string target)
{
  int letter = int(target[0]) - 65;
  int number = int(target[1]) - 48;

  map[number][letter] = "OO";
}

void Ship::updateSize()
{
  size -= 1;

  if (size <= 0)
    isAlive = false;
}

Player::Player()
{
  shipsAlive = 5;
  numShots = 0;
  isHuman = true;
  carrier = new Carrier;
  battleship = new Battleship;
  cruiser = new Cruiser;
  submarine = new Submarine;
  destroyer = new Destroyer;

  // Initialize shipLocations array
  for (int i = 0; i < 10; i++)
  {
    for (int j = 0; j < 10; j++)
      shipLocations[i][j] = NONE;
  }
}

// Player deconstructor - free all memory
Player::~Player()
{
  if (shotsFired != nullptr)
  {
    delete [] shotsFired;
    shotsFired = nullptr;
  }
  if (carrier != nullptr)
  {
    delete carrier;
    carrier = nullptr;
  }
  if (battleship != nullptr)
  {
    delete battleship;
    battleship = nullptr;
  }
  if (cruiser != nullptr)
  {
    delete cruiser;
    cruiser = nullptr;
  }
  if (submarine != nullptr)
  {
    delete submarine;
    submarine = nullptr;
  }
  if (destroyer != nullptr)
  {
    delete destroyer;
    destroyer = nullptr;
  }
}

void Player::getDefaultShips()
{

  placeShip("A0", "A4", CARRIER);
  placeShip("C0", "C3", BATTLESHIP);
  placeShip("E0", "E2", CRUISER);
  placeShip("G0", "G2", SUBMARINE);
  placeShip("I0", "I1", DESTROYER);
}

// Specification A3 - Random Human Start
// Places a ship at the coordinates provided (from 1st coordinate to 2nd coordinate) if possible,
// returns boolean corresponding to whether the ship was successfully placed (valid location)
bool Player::placeShip(string coordinate1, string coordinate2, shipType type)
{
  int shipSize = getShipSize(type);

  bool isValidTargets = true;

  // If the coordinates are invalid, then ship placement invalid
  if (!isValidTarget(coordinate1) || !isValidTarget(coordinate2))
    isValidTargets = false;

  int letter1 = int(coordinate1[0]) - 65;
  int number1 = int(coordinate1[1]) - 48;
  int letter2 = int(coordinate2[0]) - 65;
  int number2 = int(coordinate2[1]) - 48;

  // Make sure the coordinates are appropriately spaced
  if (!((abs(letter2 - letter1) + 1) == shipSize) && !((abs(number2 - number1) + 1) == shipSize))
    isValidTargets = false;

  // Ensure coordinates are in the same row or column
  if (!(letter1 == letter2) && !(number1 == number2))
    isValidTargets = false;

  // Check to see if the space is already occupied
  if (isValidTargets)
  {
    if (number1 == number2)
    {
      if (letter2 > letter1)
      {
        for (int i = 0; i < shipSize; i++)
        {
          if (friendlyMap.getValue(number1, letter1 + i) != "--")
            isValidTargets = false;
          if (!isValidTargets)
            break;
        }
      }
      else
      {
        for (int i = 0; i < shipSize; i++)
        {
          if (friendlyMap.getValue(number1, letter2 + i) != "--")
            isValidTargets = false;
          if (!isValidTargets)
            break;
        }
      }
    }
    else
    {
      if (number2 > number1)
      {
        for (int i = 0; i < shipSize; i++)
        {
          if (friendlyMap.getValue(number1 + i, letter1) != "--")
            isValidTargets = false;
          if (!isValidTargets)
            break;
        }
      }
      else
      {
        for (int i = 0; i < shipSize; i++)
        {
          if (friendlyMap.getValue(number2 + i, letter1) != "--")
            isValidTargets = false;
          if (!isValidTargets)
            break;
        }
      }
    }
  }

  // Code for ship placement
  if (isValidTargets)
  {
    
    // Code for ship placement if all boxes are the same row
    if (number1 == number2)
    {
      // If 2nd coordinate is further right than first coordinate, place the ships on the friendlymap and update the shipLocations array to hold the ships at the coordinates
      if (letter2 > letter1)
      {
        for (int i = 0; i < shipSize; i++)
        {
          friendlyMap.placeShip(number1, letter1 + i);
          shipLocations[number1][letter1 + i] = type;
        }
      }
      // If 1st coordinate is further right than second coordinate, place the ships on the friendlymap and update the shipLocations array to hold the ships at the coordinates
      else
      {
        for (int i = 0; i < shipSize; i++)
        {
          friendlyMap.placeShip(number1, letter2 + i);
          shipLocations[number1][letter2 + i] = type;
        }
      }
    }
    // Code for ship placement if all boxes are the same column
    else
    {
      // If second coordinate is further down than coordinate1, place the ships on the friendlymap and update the shipLocations array to hold the ships at the coordinates
      if (number2 > number1)
      {
        for (int i = 0; i < shipSize; i++)
        {
          friendlyMap.placeShip(number1 + i, letter1);
          shipLocations[number1 + i][letter1] = type;
        }
      }
      else
      {
        // If first coordinate is further down than second coordinate, place the ships on the friendlymap and update the shipLocations array to hold the ships at the coordinates
        for (int i = 0; i < shipSize; i++)
        {
          friendlyMap.placeShip(number2 + i, letter1);
          shipLocations[number2 + i][letter1] = type;
        }
      }
    }

  }
  else if (isHuman)
  {
    cout << "\nInvalid ship placement!" << endl;
  }
  
  return isValidTargets;
}

void Player::attack(Player* enemy, LogFile& file)
{
  string target;
  Map enemyMap = enemy->getBoard();
  shipType thisShip;
  cout << endl;
  printEmap();
  string stars;
  stars.assign(FIELD_WIDTH, '*');
  cout << "\n" << stars << endl;
  cout << "Please select your target: ";
  cin >> target;
  target = formatCoor(target);

  // Specification A1 - Advanced Input Validation - human can only fire on a unique target one time
  // Validate coordinates
  while (!cin || !isValidTarget(target) || inShotsFired(target))
  {
    if (inShotsFired(target))
    {
      cout << "\nYou have already fired on this location, please \nchoose a new target" << endl;
    }
    else
    {
      cout << "\nInvalid target. Please choose a target in the \nform [letter][number] -- Example: 'A5'" << endl;
    }

    cin.clear();
    cin.ignore(1000, '\n');
    cout << "Target: ";
    cin >> target;
    target = formatCoor(target);
  }

  // Check if the target is a hit
  if(enemyMap.checkHit(target))
  {
    cout << "\nHit!" << endl;
    thisShip = enemy->getShipType(target);

    // Pass data to logfile object
    file.addData("User hit enemy ship at location: " + target);
    // If the hit ship has a size of 1, it will be destroyed
    if (enemy->getShipSize(thisShip) == 1)
    {
      cout << "\nYou destroyed the enemy's " << getShipName(thisShip) << endl;

      // Pass data to logfile object
      file.addData("User destroyed the enemy's " + getShipName(thisShip));

    }

    // If there is a hit, update the gameboards appropriately
    this->enemyMap.setHit(target);
    enemy->updateFmap(target);
  }
  else
  {
    cout << "\nMiss!" << endl;
    updateMissMap(target);

    // Pass data to logfile object
    file.addData("User fired at location: " + target + ", but missed!");
  }

  // Track the shots fired to prohibit repeat moves
  updateShotsFired(target);
  cout << stars << endl;

}

// Specification B1 - Validate Input
// Checks if the target is a valid coordinate on the gameboard and returns a boolean
bool Player::isValidTarget(string target)
{
  bool isValid = true;

  // If 1st character is not A-J, invalid
  if (target[0] < 65 || target[0] > 74)
    isValid = false;
  // If second character is not 0-9, invalid
  if (int(target[1]) - 48 < 0 || int(target[1]) - 48 > 9)
    isValid = false;

  return isValid;
}

string Player::formatCoor(string coordinates)
{
  string s = coordinates;
  string temp = "";

  // Remove any leading whitespace characters
  while (s[0] == 32)
  {
    for (int i = 1; i < int(s.length()); i++)
    {
      temp += s[i];
    }
    s = temp;
    temp = "";
  }

  // capitalize the letter
  s[0] = toupper(s[0]);

  // Ensure coordinates are only 2 characters long
  for(int i = 0; i < 2; i++)
    temp += s[i];
  s = temp;

  return s;
}

void Player::updateFmap(string target)
{
  shipType thisShip;
  // Set the target to 'XX' (a hit)
  friendlyMap.setHit(target);
  // Set thisShip equal to the ship at the target location
  thisShip = getShipType(target);
  int letter = int(target[0]) - 65;
  int number = int(target[1]) - 48;

  // Update the target ship's size and update shipsAlive
  if (thisShip == CARRIER)
  {
    carrier->updateSize();
    if (!carrier->checkAlive())
      shipsAlive -= 1;
  }
  else if (thisShip == BATTLESHIP)
  {
    battleship->updateSize();
    if (!battleship->checkAlive())
      shipsAlive -= 1;
  }
  else if (thisShip == CRUISER)
  {
    cruiser->updateSize();
    if (!cruiser->checkAlive())
      shipsAlive -= 1;
  }
  else if (thisShip == SUBMARINE)
  {
    submarine->updateSize();
    if (!submarine->checkAlive())
      shipsAlive -= 1;
  }
  else
  {
    destroyer->updateSize();
    if (!destroyer->checkAlive())
      shipsAlive -= 1;
  }

  // Remove the hit part of the ship from the shipLocations array
  shipLocations[number][letter] = NONE;

}

void Player::printFmap() const
{
  cout << "\nMap of friendly ships:" << endl;
  friendlyMap.print();
}

void Player::printEmap() const
{
  cout << "\nMap of enemy ships: " << endl;
  enemyMap.print();
}

shipType Player::getShipType(string target)const
{
  shipType thisShip;
  int letter = int(target[0]) - 65;
  int number = int(target[1]) - 48;

  thisShip = shipLocations[number][letter];
  return thisShip;
}

bool Player::areShipsAlive()
{
  bool isAlive = true;
  
  if (carrier->checkAlive() == false && battleship->checkAlive() == false 
      && cruiser->checkAlive() == false && submarine->checkAlive() == false
       && destroyer->checkAlive() == false)
    isAlive = false;

  return isAlive;
}

void Player::updateShotsFired(string target)
{
  int newLength = numShots + 1;
  string* tempArray = new string[newLength];
	
  // If the shotsFired array does not yet exist, create it
  if (shotsFired == nullptr)
    shotsFired = new string[newLength];

  // If this is the first item to be added, set element 0 to the target
  if (newLength == 1)
    tempArray[0] = target;

  // If the array has other coordinates already, fill tempArray with previous coordinates and add target
  else
  {
    for (int i = 0; i < newLength - 1; i++)
    {
      tempArray[i] = shotsFired[i];
    }

    tempArray[newLength - 1] = target;
  }

  // Update arrays
  delete[] shotsFired;
  shotsFired = nullptr;
  shotsFired = tempArray;
  tempArray = nullptr;

  numShots++;
}

// Add missed target to gameboard
void Player::updateMissMap(string target)
{
  if (isValidTarget(target))
  {
    enemyMap.setMiss(target);
  }
}

bool Player::inShotsFired(string target)
{
  bool exists = false;

  if (shotsFired != nullptr)
  {
    for (int i = 0; i < numShots; i++)
    {
      if (shotsFired[i] == target)
        exists = true;
    }
  }

  return exists;
}

int Player::getShipSize(shipType thisShip)const
{
  int size = 0;
  if (thisShip == CARRIER)
    size = carrier->getSize();
  else if (thisShip == BATTLESHIP)
    size = battleship->getSize();
  else if (thisShip == CRUISER)
    size = cruiser->getSize();
  else if (thisShip == SUBMARINE)
    size = submarine->getSize();
  else if (thisShip == DESTROYER)
    size = destroyer->getSize();
  else
    size = -1;

  return size;
}

string Player::getShipName(shipType thisShip)const
{
  string name = "";

  if (thisShip == CARRIER)
    name = carrier->getName();
  else if (thisShip == BATTLESHIP)
    name = battleship->getName();
  else if (thisShip == CRUISER)
    name = cruiser->getName();
  else if (thisShip == SUBMARINE)
    name = submarine->getName();
  else if (thisShip == DESTROYER)
    name = destroyer->getName();
  else
    name = "none";

  return name;
}

Computer::Computer()
{
  numHits = 0;
  numMisses = 0;
  isHuman = false;
  srand(time(0));
}

Computer::~Computer()
{
  if (hits != nullptr)
  {
    delete [] hits;
    hits = nullptr;
  }
  if (misses != nullptr)
  {
    delete [] misses;
    misses = nullptr;
  }
}

string Computer::getValidTarget()
{
  string target = "";
  int num = 0;

  // Specification C2 - Prohibit AI wasted shots
  // Generates a random target that has not been fired at before
  do
  {
    target = "";

    // Get letter coordinate (A - J)
    num = 65 + (rand() % (74 - 65 + 1));
    target += char(num);

    // Get number coordinate (0 - 9)
    num = 48 + (rand() % (57 - 48 + 1));
    target += char(num);
  } while(inMisses(target) || inHits(target));

  return target;
}

// Returns a valid coordinate to be used for computer ship placement
string Computer::getValidCoordinate()
{
  string coordinate = "";
  int num = 0;
  string emptyBoard = "--";

  // Generates a valid coordinate
  do
  {
    coordinate = "";

    // Get letter coordinate (A - J)
    num = 65 + (rand() % (74 - 65 + 1));
    coordinate += char(num);

    // Get number coordinate (0 - 9)
    num = 48 + (rand() % (57 - 48 + 1));
    coordinate += char(num);

  } while(friendlyMap.getValue(coordinate) != emptyBoard);

  return coordinate;
}

// Returns a string coordinate that is offset from the parameter coordinate by the length of the
// ship represented by shiptype
string Computer::getValidCoordinate(string coordinate, shipType type)
{
  string coordinate2 = "";
  string tempCoordinate = "";
  // Shipsize must be 1 less than actual ship size otherwise resulting coordinate will be
  // an extra space too far away from first coordinate
  int shipSize = getShipSize(type) - 1;
  int letter = int(coordinate[0]);
  int number = int(coordinate[1]);

  tempCoordinate += char(letter + shipSize);
  tempCoordinate += char(number);
  if (isValidCoordinate(tempCoordinate))
    coordinate2 = tempCoordinate;
  else
  {
    tempCoordinate = "";
    tempCoordinate += char(letter);
    tempCoordinate += char(number + shipSize);
    if (isValidCoordinate(tempCoordinate))
      coordinate2 = tempCoordinate;
    else
    {
      tempCoordinate = "";
      tempCoordinate += char(letter - shipSize);
      tempCoordinate += char(number);
      if (isValidCoordinate(tempCoordinate))
        coordinate2 = tempCoordinate;
      
      else
      {
        tempCoordinate = "";
        tempCoordinate += char(letter);
        tempCoordinate += char(number - shipSize);
        if (isValidCoordinate(tempCoordinate))
          coordinate2 = tempCoordinate;
      }
    }
  }
  
  return coordinate2;
}

bool Computer::inHits(string target)
{
  bool exists = false;

  if (hits != nullptr)
  {
    for (int i = 0; i < numHits; i++)
    {
      if (hits[i] == target)
        exists = true;
    }
  }
  

  return exists;
}

bool Computer::inMisses(string target)
{
  bool exists = false;

  if (misses != nullptr)
  {
    for (int i = 0; i < numMisses; i++)
    {
      if (misses[i] == target)
        exists = true;
    }
  }
  

  return exists;
}

// Specification B3 - Random Start
// Places the computer's ships in valid and random locations on the gameboard
void Computer::placeShips()
{
  string coordinate1;
  string coordinate2;

  // Place Carrier
    do
    {
      coordinate1 = getValidCoordinate();
      coordinate2 = getValidCoordinate(coordinate1, CARRIER);
    }while (!placeShip(coordinate1, coordinate2, CARRIER));

  // Place Battleship
    do
    {
      coordinate1 = getValidCoordinate();
      coordinate2 = getValidCoordinate(coordinate1, BATTLESHIP);
    }while (!placeShip(coordinate1, coordinate2, BATTLESHIP));

  // Place Cruiser
   do
    {
      coordinate1 = getValidCoordinate();
      coordinate2 = getValidCoordinate(coordinate1, CRUISER);
    }while (!placeShip(coordinate1, coordinate2, CRUISER));

  // Place Submarine
    do
    {
      coordinate1 = getValidCoordinate();
      coordinate2 = getValidCoordinate(coordinate1, SUBMARINE);
    }while (!placeShip(coordinate1, coordinate2, SUBMARINE));

  // Place Destroyer
    do
    {
      coordinate1 = getValidCoordinate();
      coordinate2 = getValidCoordinate(coordinate1, DESTROYER);
    }while (!placeShip(coordinate1, coordinate2, DESTROYER));
}

void Computer::attack(Player& user, LogFile& file)
{
  string target;
  Map enemyMap = user.getBoard();
  shipType thisShip;
  string stars;
  stars.assign(FIELD_WIDTH, '*');
  cout << "\n" << stars;

  // Get target for enemy to fire
  target = getValidTarget();
  target = formatCoor(target);


  // Check if the target is a hit
  if(enemyMap.checkHit(target))
  {
    cout << "\nThe enemy got a hit at location: " << target << endl;
    updateHits(target);
    thisShip = user.getShipType(target);

    // Update data in logfile object
    file.addData("The enemy got at a hit at location: " + target);
    
    // If the hit ship has a size of 1, it will be destroyed
    if (user.getShipSize(thisShip) == 1)
    {
      cout << "\nThe enemy destroyed your " << getShipName(thisShip) << endl;

      // Update logfile data
      file.addData("The enemy destroyed the user's " + getShipName(thisShip));

    }

    // If there is a hit, update the two gameboards appropriately
    this->enemyMap.setHit(target);
    user.updateFmap(target);
  }
  else
  {
    cout << "\nThe enemy missed!" << endl;
    updateMisses(target);

    // Update logfile data
    file.addData("The enemy fired at location: " + target + ", but missed!");
  }

  cout << stars << endl;
  user.printFmap();
  cout << stars << endl;
}

void Computer::updateMisses(string target)
{
  int newLength = numMisses + 1;
  string* tempArray = new string[newLength];
	
  // If the Misses array does not yet exist, create it
  if (misses == nullptr)
    misses = new string[newLength];

  // If this is the first item to be added, set element 0 to the target
  if (newLength == 1)
    tempArray[0] = target;
  // If the array has other coordinates already, fill tempArray with previous coordinates and add target
  else
  {
    for (int i = 0; i < newLength - 1; i++)
    {
      tempArray[i] = misses[i];
    }

    tempArray[newLength - 1] = target;
  }

  // Update arrays
  delete[] misses;
  misses = nullptr;
  misses = tempArray;
  tempArray = nullptr;

  numMisses++;
}

void Computer::updateHits(string target)
{
  int newLength = numHits + 1;
  string* tempArray = new string[newLength];
	
  // If the Hits array does not yet exist, create it
  if (hits == nullptr)
    hits = new string[newLength];

  // If this is the first item to be added, set element 0 to the target
  if (newLength == 1)
    tempArray[0] = target;
  // If the array has other coordinates already, fill tempArray with previous coordinates and add target
  else
  {
    for (int i = 0; i < newLength - 1; i++)
    {
      tempArray[i] = hits[i];
    }

    tempArray[newLength - 1] = target;
  }

  // Update arrays
  delete[] hits;
  hits = nullptr;
  hits = tempArray;
  tempArray = nullptr;

  numHits++;
}

void Computer::printFmap()const
{
  cout << "\nAi ship locations:" << endl;
  friendlyMap.print();
}

LogFile::LogFile()
{
  data = nullptr;
  numData = 0;
}

LogFile::~LogFile()
{
  if (data != nullptr)
  {
    delete [] data;
    data = nullptr;
  }
}

void LogFile::addData(string newData)
{
  int newLength = numData + 1;
  string* tempArray = new string[newLength];
  string dataToAdd = getCurrentTime() + "\t" + newData;
	
  // If the data array does not yet exist, create it
  if (data == nullptr)
    data = new string[newLength];

  // If this is the first item to be added, set element 0 to the string
  if (newLength == 1)
    tempArray[0] = dataToAdd;

  // If the array has other coordinates already, fill tempArray with previous strings, add new string
  else
  {
    for (int i = 0; i < newLength - 1; i++)
    {
      tempArray[i] = data[i];
    }

    tempArray[newLength - 1] = dataToAdd;
  }

  // Update arrays
  delete[] data;
  data = nullptr;
  data = tempArray;
  tempArray = nullptr;

  numData++;
}

void LogFile::saveData()
{
  ofstream file;
  file.open("log.txt");

  for (int i = 0; i < numData; i++)
  {
    file << data[i] << endl << endl;
  }

  file.close();
}

string LogFile::getCurrentTime()
{
  string date = "";

  time_t now = time(0);
  struct tm* currentDate = localtime(&now);
  date = asctime(currentDate);

  return date;
}