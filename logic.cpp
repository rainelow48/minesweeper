#ifndef LOGIC_CPP
#define LOGIC_CPP

#include <algorithm> //To random shuffle
#include <array>
#include <iostream>
#include <vector>
using namespace std;

/**
 * Available difficulty levels.
 */
enum Level {
    // 9x9 with 10 mines.
    EASY,
    // 16x16 with 40 mines.
    NORMAL,
    // 16x30 with 99 mines.
    HARD,
};

enum GameState {
    //Game in progress
    PLAYING,
    //Bombs successfully flagged
    WON,
    //Bomb exploded
    LOST
};

class Board {
public:
    //Constructors
    Board(Level);
    Board(uint8_t, uint8_t, uint16_t);

    //Getters
    vector<vector<int>> GetPlayerGrid();
    // vector<vector<int>> GetAnswerGrid();
    vector<array<int, 2>> GetWrongFlags();
    int GetHeight();
    int GetWidth();
    int GetMoves();
    int GetFlagsLeft();

    //Setters?
    void Flag(int, int);
    GameState Open(int, int, bool);

private:
    //Members
    int height;
    int width;
    int bomb_size;
    int moves = 0;
    int flags = 0;
    GameState game_state = PLAYING;
    vector<vector<int>> player_grid;
    vector<vector<int>> answer_grid;
    vector<array<int, 2>> wrong_flags;

    //Functions
    void Initialise();
    void PopulateAnswerGrid();
    void AppendBomb(vector<array<int, 2>> bomb_coords);
    void UpdateGameStatus();
    vector<array<int, 2>> GetNeighbours(int row, int col);
};

/**
 * Generates grid based on specified difficulty
 * @param difficulty EASY, NORMAL, HARD
 */
Board::Board(Level difficulty) {
    switch (difficulty) {
    case EASY: {
        height = 9;
        width = 9;
        bomb_size = 10;
        break;
    }
    case NORMAL: {
        height = 16;
        width = 16;
        bomb_size = 40;
        break;
    }
    case HARD: {
        height = 16;
        width = 30;
        bomb_size = 99;
        break;
    }
    }
    Initialise();
}

/**
 * Custom difficulty
 * @param height the height of grid
 * @param width the width of grid
 * @param bomb_size the number of bombs
 */
Board::Board(uint8_t h, uint8_t w, uint16_t bs) {
    //Invalid input handling, default to easy mode
    if (h == 0 || w == 0 || bs == 0 || bs >= h * w) {
        height = 9;
        width = 9;
        bomb_size = 10;
    } else {
        height = h;
        width = w;
        bomb_size = bs;
    }
    Initialise();
}

//Getters
vector<vector<int>> Board::GetPlayerGrid() {
    return player_grid;
}
// vector<vector<int>> Board::GetAnswerGrid() {
//     return answer_grid;
// }
vector<array<int, 2>> Board::GetWrongFlags() {
    return wrong_flags;
}
int Board::GetHeight() {
    return height;
}
int Board::GetWidth() {
    return width;
}
int Board::GetMoves() {
    return moves;
}
int Board::GetFlagsLeft() {
    return max(0, bomb_size - flags);
}
//Actions
/**
 * Flag/Unflag cell (row, col) only if cell is unopened/flagged
 */
void Board::Flag(int row, int col) {
    if (player_grid[row][col] == 10) {
        player_grid[row][col] = 11;
        moves += 1;
        flags += 1;
        if (answer_grid[row][col] != 9) {
            wrong_flags.push_back({row, col});
        }
    } else if (player_grid[row][col] == 11) {
        player_grid[row][col] = 10;
        moves += 1;
        flags -= 1;
        for (int i = 0; i < wrong_flags.size(); i++) {
            if (wrong_flags[i][0] == row && wrong_flags[i][1] == col) {
                wrong_flags.erase(wrong_flags.begin() + i);
            }
        }
    }
}

/**
 * Opens cell (row, col)
 * If cell unopened -> opens cell
 * - If opened cell = 0 -> recursively opens neighbouring cells
 * - If opened cell = 9 -> Bomb opened, game over
 * If cell already opened -> Open all neighbour cells if no. of flagged neighbouring = no. on cell
 * Increment move count by 1 (only for open cell called by user)
 */
GameState Board::Open(int row, int col, bool recursive = false) {
    if (recursive == false) {
        moves += 1;
    }

    int cell = player_grid[row][col];
    int ans = answer_grid[row][col];

    //Open cell if cell is unopened
    if (cell == 10) {
        player_grid[row][col] = ans;

        //End game if bomb cell is opened, display answer
        if (ans == 9) {
            player_grid = answer_grid;
            game_state = LOST;
            return game_state;
        }
        //Else recursively open neighbours of 0 cell
        else if (ans == 0) {
            vector<array<int, 2>> neighbours = GetNeighbours(row, col);
            for (int i = 0; i < neighbours.size(); i++) {
                int r = neighbours[i][0];
                int c = neighbours[i][1];
                if (player_grid[r][c] == 10) {
                    Open(r, c, true);
                }
            }
        } //Else do nothing
    }
    /** 
     * If cell is already open, check if no. of flagged neighbours = no. on cell
     * Open all unopened & unflagged neighbours if true
     */
    else if (cell != 11) { //If cell is opened and unflagged
        //Count no. of flagged neighbours
        vector<array<int, 2>> neighbours = GetNeighbours(row, col);
        int flag_count = 0;
        for (int i = 0; i < neighbours.size(); i++) {
            int r = neighbours[i][0];
            int c = neighbours[i][1];
            if (player_grid[r][c] == 11) {
                flag_count += 1;
            }
        }
        //If no. of flagged neighbours = no. on cell, open all unopened neighbours
        if (flag_count == cell) {
            int unopened = 0;
            for (int i = 0; i < neighbours.size(); i++) {
                int r = neighbours[i][0];
                int c = neighbours[i][1];
                if (player_grid[r][c] == 10) {
                    Open(r, c, true);
                    unopened += 1;
                }
            }
            //Do not count move if all neighbours are already opened
            if (unopened == 0) {
                moves -= 1;
            }
        }
    }
    //Else if cell is flagged, do not count as a move
    else {
        moves -= 1;
    }
    UpdateGameStatus();
    return game_state;
}

//Functions
/**
 * Initialises player_grid and answer_grid based on height, width and bomb_size
 */
void Board::Initialise() {
    for (int i = 0; i < height; i++) {
        player_grid.push_back(vector<int>(width, 10));
        answer_grid.push_back(vector<int>(width, 0));
    }
    PopulateAnswerGrid();
}

/**
 * Populate answer grid with random bomb locations and corresponding neighbour values
*/
void Board::PopulateAnswerGrid() {
    //Obtain random location of bombs
    vector<int> random_integer(height * width);
    for (int i = 0; i < width * height; i++) {
        random_integer[i] = i;
    }
    random_shuffle(random_integer.begin(), random_integer.end());

    //Obtaining bomb coordinates based on playing grid size
    vector<array<int, 2>> bomb_coords(bomb_size);
    for (int i = 0; i < bomb_size; i++) {
        bomb_coords[i] = {random_integer[i] / width, random_integer[i] % width};
    }
    AppendBomb(bomb_coords);
}

/**
 * Append bombs and neighbours values into grid
 * Increment neighbouring cells value by 1 with every bomb appended
 */
void Board::AppendBomb(vector<array<int, 2>> bomb_coords) {
    //Increment bomb neighbours by 1
    for (auto coords : bomb_coords) {
        vector<array<int, 2>> neighbours = GetNeighbours(coords[0], coords[1]);
        for (auto x : neighbours) {
            answer_grid[x[0]][x[1]] += 1;
        }
    }

    // Set all bomb cells to 9
    for (auto coords : bomb_coords) {
        answer_grid[coords[0]][coords[1]] = 9;
    }
}

/**
 * Check if winning condition has been met
 */
void Board::UpdateGameStatus() {
    int count = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (player_grid[i][j] == 10 || player_grid[i][j] == 11) {
                count += 1;
            }
        }
    }
    if (count == bomb_size) {
        game_state = WON;
    }
}

/**
 *Get all neighbouring cell coordinates given a cell coordinate
 */
vector<array<int, 2>> Board::GetNeighbours(int row, int col) {
    vector<array<int, 2>> neighbours_coords;
    int row_min, row_max, col_min, col_max;

    //Set rowMin and rowMax
    row_min = row - 1;
    row_max = row + 1;
    col_min = col - 1;
    col_max = col + 1;

    if (row == 0)
        row_min = 0;
    else if (row == height - 1)
        row_max = row;

    if (col == 0)
        col_min = 0;
    else if (col == width - 1)
        col_max = col;

    //Get neighbour cells coordinates
    for (int i = row_min; i < row_max + 1; i++) {
        for (int j = col_min; j < col_max + 1; j++) {
            if (i != row || j != col) {
                neighbours_coords.push_back({i, j});
            }
        }
    }
    return neighbours_coords;
}

/**
 * Overloaded << operator to print player grid
 */
ostream &operator<<(ostream &out, Board &board) {
    int width = board.GetWidth();
    int height = board.GetHeight();
    vector<vector<int>> grid = board.GetPlayerGrid();

    //Print column label
    cout << "\t";
    for (int h = 0; h < width; h++) {
        cout << h;
        if (h < 10) {
            cout << "  ";
        } else {
            cout << " ";
        }
    }
    cout << endl
         << endl;

    for (int i = 0; i < height; i++) {
        //Print row label
        cout << i << "\t";
        for (int j = 0; j < width; j++) {
            //Print board contents
            cout << grid[i][j];
            if (grid[i][j] < 10) {
                cout << "  ";
            } else {
                cout << " ";
            }
        }
        cout << endl;
    }
    cout << endl;
    return out;
}

#endif