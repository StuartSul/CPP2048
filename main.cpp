#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

using namespace std;

const int board_width = 4;
const int board_height = 4;

const int LEFT = 1;
const int RIGHT = 2;
const int UP = 3;
const int DOWN = 4;

const int NOCHANGE = 1;
const int NORMAL = 0;
const int INVALID = -1;
const int QUIT = -2;
const int OVER = -3;

int is_movable(unsigned *board, int movement);
int is_equal(unsigned *board1, unsigned *board2);
char receive_input();
double random_probability();
unsigned *init_board();
void free_board(unsigned *board);
int generate_tile(unsigned *board);
int best_score(unsigned *board);
int process_input(unsigned *board, char input);
void print_board(unsigned *board);
void move_board(unsigned *board, int movement);
int board_status(unsigned *board);

int main() {
    unsigned *board = init_board();
    print_board(board);

    while (1) {
        char input = receive_input();
        int result = process_input(board, input);
        if (result == INVALID) {
            cout << "  Invalid input: must be either w, a, s, d, or q" << endl;
            continue;
        }
        print_board(board);
        if (result == QUIT) {
            cout << "  Game Quit" << endl << endl;
            break;
        } else if (result == OVER) {
            cout << "  Game Over" << endl << endl;
            break;
        }
    }

    free_board(board);
}


unsigned *init_board() {
    unsigned board_size = board_width * board_height;
    unsigned *board = new unsigned[board_size];
    memset(board, 0, sizeof(unsigned) * board_size);
    generate_tile(board);
    generate_tile(board);
    return board;
}


void free_board(unsigned *board) {
    delete[] board;
}


char receive_input() {
    char input;
    cout << "  Enter input: ";
    cin >> input;
    return input;
}


double random_probability() {
    random_device rand;
    mt19937 generator(rand());
    uniform_real_distribution<> distribution(0.0, 1.0);
    return distribution(generator);
}

int generate_tile(unsigned *board) {
    int empty_tiles = 0;
    int empty_tiles_index[board_height * board_width];
    for (int i = 0; i < board_height * board_width; i++) {
        if (board[i] == 0)
            empty_tiles_index[empty_tiles++] = i;
    }
    
    if (empty_tiles == 0)
        return 0;

    int new_place = random_probability() * empty_tiles;
    board[empty_tiles_index[new_place]] = (random_probability() < 0.1) ? 4 : 2;
    return 1;
}


int process_input(unsigned *board, char input) {
    int movement;

    switch (input) {
        case 'w':
        case 'W':
            movement = UP;
            break;
        case 'a':
        case 'A':
            movement = LEFT;
            break;
        case 's':
        case 'S':
            movement = DOWN;
            break;
        case 'd':
        case 'D':
            movement = RIGHT;
            break;
        case 'q':
        case 'Q':
            return QUIT;
        default:
            return INVALID;
    }

    if (is_movable(board, movement)) {
        move_board(board, movement);
        generate_tile(board);
        return board_status(board);
    } else
        return NOCHANGE;
}

void move_board(unsigned *board, int movement) {
    int axis1_start = 0, axis2_start;
    int axis1_end, axis2_end;
    int axis1_adder = 1, axis2_adder;
    int axis1_multiplier, axis2_multiplier;

    switch (movement) {
        case LEFT:
            axis2_start = 0;
            axis1_end = board_height;
            axis2_end = board_width;
            axis2_adder = 1;
            axis1_multiplier = board_width;
            axis2_multiplier = 1;
            break;
        case RIGHT:
            axis2_start = board_width - 1;
            axis1_end = board_height;
            axis2_end = -1;
            axis2_adder = -1;
            axis1_multiplier = board_width;
            axis2_multiplier = 1;
            break;
        case UP:
            axis2_start = 0;
            axis1_end = board_width;
            axis2_end = board_height;
            axis2_adder = 1;
            axis1_multiplier = 1;
            axis2_multiplier = board_width;
            break;
        case DOWN:
            axis2_start = board_height - 1;
            axis1_end = board_width;
            axis2_end = -1;
            axis2_adder = -1;
            axis1_multiplier = 1;
            axis2_multiplier = board_width;
            break;
    }

    for (int i = axis1_start; i != axis1_end; i += axis1_adder) {
        for (int count = axis2_start; count != axis2_end - axis2_adder; count += axis2_adder) {
            for (int j = axis2_start; j != axis2_end - axis2_adder; j += axis2_adder) {
                int curr = axis1_multiplier * i + axis2_multiplier * j;
                int next = axis1_multiplier * i + axis2_multiplier * (j + axis2_adder);
                if (board[curr] == 0) {
                    board[curr] = board[next];
                    board[next] = 0;
                }
            }
        }

        for (int j = axis2_start; j != axis2_end - axis2_adder; j += axis2_adder) {
            int curr = axis1_multiplier * i + axis2_multiplier * j;
            int next = axis1_multiplier * i + axis2_multiplier * (j + axis2_adder);
            if (board[curr] == board[next]) {
                board[curr] *= 2;
                board[next] = 0;
            }
        }

        for (int count = axis2_start; count != axis2_end - 2 * axis2_adder; count += axis2_adder) {
            for (int j = axis2_start; j != axis2_end - axis2_adder; j += axis2_adder) {
                int curr = axis1_multiplier * i + axis2_multiplier * j;
                int next = axis1_multiplier * i + axis2_multiplier * (j + axis2_adder);
                if (board[curr] == 0) {
                    board[curr] = board[next];
                    board[next] = 0;
                }
            }
        }
    }
}

int board_status(unsigned *board) {
    if (is_movable(board, UP) ||
        is_movable(board, DOWN) ||
        is_movable(board, LEFT) ||
        is_movable(board, RIGHT))
        return NORMAL;
    else
        return OVER;
}

int is_movable(unsigned *board, int movement) {
    unsigned moved[board_width * board_height];
    memcpy(moved, board, sizeof(unsigned) * board_width * board_height);
    move_board(moved, movement);
    if (is_equal(moved, board))
        return 0;
    return 1;
}

int is_equal(unsigned *board1, unsigned *board2) {
    for (int i = 0; i < board_width * board_height; i++) {
        if (board1[i] != board2[i])
            return 0;
    }
    return 1;
}


int best_score(unsigned *board) {
    int best_score = 0;
    for (int i = 0; i < board_width * board_height; i++) {
        if (board[i] > best_score)
            best_score = board[i];
    }
    return best_score;
}


void print_board(unsigned *board) {
    if (system("clear"))
        cout << "ERROR: Failed to clear stdout" << endl;

    cout << endl;
    cout << "    C Plus Plus 2048                                 " << endl;
    cout << "    Copyright 2020 Hyoung Uk \"Stuart\" Sul          " << endl;
    cout << "    https://github.com/StuartSul                     " << endl;
    cout << "                                                     " << endl;
    cout << "    Best Score: " << best_score(board) << endl;
    cout << "    _____________________________________________    " << endl;

    for (int i = 0; i < board_height; i++) {
        cout << "    |          |          |          |          |    " << endl;
        cout << "    |  ";
        for (int j = 0; j < board_width; j++) {
            if (board[i * board_width + j] > 0)
                cout << setw(6) << board[i * board_width + j] % 1000000;
            else
                cout << "      ";
            cout << "  |  ";
        }
        cout << " " << endl;
        cout << "    |          |          |          |          |    " << endl;
        if (i == board_height - 1)
            cout << "    |___________________________________________|    " << endl;
        else
            cout << "    |-------------------------------------------|    " << endl;
    }
    cout << endl;
}
