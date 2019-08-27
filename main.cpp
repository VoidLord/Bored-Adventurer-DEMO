// Console RPG Game

//usual including
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <thread>
#include <chrono>
#include <curses.h>
#include <panel.h>
#include <windows.h>
#include "Player.h"

//define characters which appear on map, for easy usage
#define EMPTY   ' '
#define WALL    '#'
#define PLAYER  '@'
#define IGOLD   '*'

//define color pair numbers, for easy usage
#define EMPTY_PAIR  1
#define WALL_PAIR   2
#define PLAYER_PAIR 3
#define GOLD_PAIR   4
#define TITLE_PAIR  5
#define INFO_PAIR   6
#define BOX_PAIR    7
#define HG_PAIR     8

//map
std::vector<std::string> test1 = {
    "##############",
    "#    ****    #",
    "#   ###      #",
    "# * # ####   #",
    "# *      #   #",
    "##########   #",
    "#            #",
    "############ #",
    "#            #",
    "##############"
};

std::vector<std::string> test2 = {
    "#########",
    "#   #   #",
    "#   #   #",
    "#       #",
    "#   #   #",
    "#########"
};

std::map<std::string, std::vector<std::string>> maps{
    {"test1", test1},
    {"test2", test2}
};

//function prototypes
void printGame(WINDOW* window, Player& plyr, std::vector<std::string>& cMap);
void printInfo(WINDOW* window, Player& plyr);
void printLog(WINDOW* window, std::vector<std::string> logs);
void printInv(WINDOW* window, Player& plyr, bool highlight, unsigned short int& hgNum);
std::string getHG(WINDOW* window, Player& plyr, unsigned short int& hgNum);
void addLog(std::vector<std::string>& log, std::string message);
void spawnPlayer(Player& plyr, std::vector<std::string>& cMap, int x, int y);

//here comes the fun
int main() {
    std::vector<std::string> currentMap = maps["test1"];
    int main_x = currentMap.size() + 2;
    int main_y = currentMap[0].size() + 2;
    int info_x = 12;
    int info_y = 16;
    int log_x = 8;
    int log_y = main_y + info_y + 1;
    int inv_x = main_x + log_x;
    int inv_y = 22;
    initscr(); //initialize pdcurses
    resize_term((main_x > info_x ? main_x : info_x) + log_x, main_y + info_y + 3 + inv_y + 1);
    SetConsoleTitle(TEXT(""));
    curs_set(0); //hide the blinking underline
    noecho();
    keypad(stdscr, true);
    if (has_colors() == false) {
        endwin();
        std::cout << "ERROR: Colors are not supported in this console/terminal." << std::endl;
        return 0;
    }
    start_color(); //initialize coloring in pdcurses
    init_pair(1, COLOR_BLACK, COLOR_BLACK); //empty
    init_pair(2, COLOR_MAGENTA, COLOR_BLACK); //wall
    init_pair(3, COLOR_WHITE, COLOR_BLACK); //player
    init_pair(4, COLOR_YELLOW, COLOR_BLACK); //igold
    init_pair(5, COLOR_CYAN, COLOR_BLACK); //title
    init_pair(6, COLOR_YELLOW, COLOR_BLACK); //info
    init_pair(7, COLOR_WHITE, COLOR_BLACK); //box
    init_pair(8, COLOR_BLACK, COLOR_WHITE); //highlight
    WINDOW* main = newwin(main_x, main_y, 0, 1);
    WINDOW* info = newwin(info_x, info_y, 0, main_y+2);
    WINDOW* log = newwin(log_x, log_y, (main_x > info_x ? main_x : info_x), 1);
    WINDOW* inv = newwin(inv_x, inv_y, 0, main_y + info_y+3);
    //setup player information
    std::vector<std::string> logs;
    for (unsigned int i = 0; i < 6; i++) {
        logs.push_back("");
    }
    Player player("void", 100, 0);
    int* pPos = player.getPos();
    spawnPlayer(player, currentMap, 1, 1);
    int mode; //1=move, 2=inv
    int input;
    bool highlight = false;
    unsigned short int hgNum = 0;
    mode = 1;
    player.giveItem("Iron_Sword", 1);
    player.giveItem("Health_Potion", 1);
    player.giveItem("Health_Potion", 4);
    player.giveItem("Cookie", 8);
    do {
        werase(stdscr);
        werase(main);
        werase(info);
        werase(log);
        werase(inv);
        printGame(main, player, currentMap);
        printInfo(info, player);
        printLog(log, logs);
        printInv(inv, player, highlight, hgNum);
        attron(COLOR_PAIR(BOX_PAIR));
        box(main, 0, 0);
        box(info, 0, 0);
        box(log, 0, 0);
        box(inv, 0, 0);
        attroff(COLOR_PAIR(BOX_PAIR));
        wnoutrefresh(stdscr);
        wnoutrefresh(main);
        wnoutrefresh(info);
        wnoutrefresh(log);
        wnoutrefresh(inv);
        doupdate();
        input = wgetch(stdscr); //get player input
        if (mode == 1) {
            if (input == KEY_LEFT) {
                player.movePlayer(currentMap, pPos[0], pPos[1]-1);
            } else if (input == KEY_UP) {
                player.movePlayer(currentMap, pPos[0]-1, pPos[1]);
            } else if (input == KEY_RIGHT) {
                player.movePlayer(currentMap, pPos[0], pPos[1]+1);
            } else if (input == KEY_DOWN) {
                player.movePlayer(currentMap, pPos[0]+1, pPos[1]);
            } else if (input == 'i') {
                highlight = true;
                mode = 2;
            }else if (input == KEY_RESIZE) {
                resize_term((main_x > info_x ? main_x : info_x) + log_x, main_y + info_y + 3);
                curs_set(0);
            }
        } else if (mode == 2) {
            if (input == KEY_UP && hgNum > 0) {
                hgNum -= 1;
            } else if (input == KEY_DOWN && hgNum < player.getInv().size() - 1) {
                hgNum += 1;
            } else if (input == 'd') {
                player.delItem(getHG(inv, player, hgNum), 1);
            } else if (input == 'i') {
                highlight = false;
                mode = 1;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } while (true);
    endwin();
    return 0;
}

//this function prints out the game to the screen
void printGame(WINDOW* window, Player& plyr, std::vector<std::string>& cMap) {
    for (unsigned int i = 0; i < cMap.size(); i++) {
        for (unsigned int j = 0; j < cMap[i].size(); j++) {
            //some weird way to convert char into C-type string
            char chr = cMap[i][j];
            std::string str;
            str.push_back(chr);
            if (chr == PLAYER) {            //if its a player
                wattron(window, COLOR_PAIR(PLAYER_PAIR));
                mvwprintw(window, i + 1, j + 1, str.c_str());
                wattroff(window, COLOR_PAIR(PLAYER_PAIR));
            } else if (chr == EMPTY) {      //if its empty space
                wattron(window, COLOR_PAIR(EMPTY_PAIR));
                mvwprintw(window, i + 1, j + 1, str.c_str());
                wattroff(window, COLOR_PAIR(EMPTY_PAIR));
            } else if (chr == WALL) {       //if its a wall
                wattron(window, A_DIM);
                wattron(window, COLOR_PAIR(WALL_PAIR));
                mvwprintw(window, i + 1, j + 1, str.c_str());
                wattroff(window, A_DIM);
                wattroff(window, COLOR_PAIR(WALL_PAIR));
            } else if (chr == IGOLD) {       //if its item gold
                wattron(window, COLOR_PAIR(GOLD_PAIR));
                mvwprintw(window, i + 1, j + 1, str.c_str());
                wattroff(window, COLOR_PAIR(GOLD_PAIR));
            } else {                        //if unknown
                mvwprintw(window, i + 1, j + 1, str.c_str());
            }
        }
        printw("\n");
    }
}

void printInfo(WINDOW* window, Player& plyr) {
    std::string name = plyr.getName();
    int health = plyr.getHealth();
    int gold = plyr.getGold();
    int* pos = plyr.getPos();

    wattron(window, COLOR_PAIR(TITLE_PAIR));
    mvwprintw(window, 1, 1, "RPG Game");    //title
    wattroff(window, COLOR_PAIR(TITLE_PAIR));
    wattron(window, COLOR_PAIR(INFO_PAIR));
    mvwprintw(window, 3, 1, name.c_str());  //name
    std::string temp = std::to_string(health);
    mvwprintw(window, 4, 1, temp.c_str());  //health
    wprintw(window, " hp");
    temp = std::to_string(gold);
    mvwprintw(window, 5, 1, temp.c_str());  //gold
    wprintw(window, " g");
    temp = std::to_string(pos[0]);
    temp.push_back(',');
    std::string tempI = std::to_string(pos[1]);
    mvwprintw(window, 6, 1, temp.c_str());  //pos
    wprintw(window, tempI.c_str()); //pos
    wattroff(window, COLOR_PAIR(INFO_PAIR));
}

void printLog(WINDOW* window, std::vector<std::string> logs) {
    for (unsigned int i = 0; i < logs.size(); i++) {
        mvwprintw(window, i + 1, 1, logs[i].c_str());
    }
}

void addLog(std::vector<std::string>& logs, std::string message) {
    for (unsigned int i = 0; i < logs.size()-1; i++) {
        logs[i] = logs[i+1];
    }
    logs[5] = message;
}

void printInv(WINDOW* window, Player& plyr, bool highlight, unsigned short int& hgNum) {
    std::map<std::string, unsigned int> inventory = plyr.getInv();
    std::vector<std::string> v;
    unsigned int i = 0;
    for(std::map<std::string, unsigned int>::iterator it = inventory.begin(); it != inventory.end(); it++) {
        v.push_back(it->first);
        int temp = it->second;
        if (temp <= 0) {
            continue;
        }
        if (i == hgNum && highlight == true) {
            wattron(window, COLOR_PAIR(8));
        } else {
            wattron(window, COLOR_PAIR(7));
        }
        mvwprintw(window, i+1, 1, (it->first).c_str());
        wprintw(window, ": ");
        wprintw(window, (std::to_string(temp)).c_str());
        if (i == hgNum && highlight == true) {
            wattroff(window, COLOR_PAIR(8));
        } else {
            wattroff(window, COLOR_PAIR(7));
        }
        i++;
    }
}

std::string getHG(WINDOW* window, Player& plyr, unsigned short int& hgNum) {
    std::map<std::string, unsigned int> inventory = plyr.getInv();
    std::string highlighted;
    unsigned int i = 0;
    for(std::map<std::string, unsigned int>::iterator it = inventory.begin(); it != inventory.end(); it++) {
        if (i == hgNum) {
            highlighted = it->first;
        }
        i++;
    }
    return highlighted;
}

//spawns player
void spawnPlayer(Player& plyr, std::vector<std::string>& cMap, int x, int y) {
    if (cMap[x][y] == EMPTY) {
        cMap[x][y] = PLAYER;
        plyr.setPos(x, y);
    } //TODO: make it so it can spawn the player somewhere if path is blocked, maybe a spawn position for each map will do :L
}