// Bored Adventurer

//usual including
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <thread>
#include <chrono>
#include <curses.h>
#include <windows.h>
#include "Player.h"

//define characters which appear on map, for easy usage
#define EMPTY       ' '
#define WALL        '#'
#define TREE        't'
#define PLAYER      '@'
#define ENEMY       'e'
#define BOSS        'b'
#define IGOLD       '*'
#define KEY         'k'
#define CHEST       'c'
#define H_TRAP      'x'
#define V_TRAP      'X'
#define U_STAIR     '^'
#define D_STAIR     'v'
#define WATER       'w'

//define color pair numbers, for easy usage
#define EMPTY_PAIR  1
#define WALL_PAIR   2
#define PLAYER_PAIR 3
#define GOLD_PAIR   4
#define TITLE_PAIR  5
#define INFO_PAIR   6
#define BOX_PAIR    7
#define HG_PAIR     8
#define KEY_PAIR    9
#define CHEST_PAIR  10
#define TRAP_PAIR   11
#define DEAD_PAIR   12
#define STAIR_PAIR  13
#define TREE_PAIR   14
#define WATER_PAIR  15

std::vector<std::string> start = {
    "tttttwwwwwwttttttttt",
    "t@t   cwtt  *t * t t",
    "t   tttw   ttt t  xt",
    "txt  * w t   t  t  t",
    "t tttt t*t t   xtt t",
    "t t x    t *ttt tk t",
    "tttttttttwwwwttttttt"
};

std::vector<std::string> forest = {
    "ttttttttttttwwwwtttt",
    "t@      w      w   t",
    "ttttttt   twww w w t",
    "t    *ttttt   ** t t",
    "t*tttttt  *  ttttt t",
    "t     *x* tt   **  t",
    "###ttt ttxtttttttt t",
    "#v#* t *     t*    t",
    "# #t t ttttte  xtttt",
    "# e  *  tttttt  tctt",
    "##tttt   *     *   t",
    "tttttttttttttttttttt"
};

std::vector<std::string> dungeon = {
    "#####################",
    "#    *xx#ccc#xx*    #",
    "# #       e       # #",
    "#*#  x#       #x  #*#",
    "#*#      ###   e  #*#",
    "# ##x    x#x    x## #",
    "#     e             #",
    "#x  #############  x#",
    "###   *       *   ###",
    "#^######## ##########",
    "#@          e    * *#",
    "#####################"
};

//map
std::map<std::string, std::vector<std::string>> maps{
    {"start", start},
    {"forest", forest},
    {"dungeon", dungeon}
};

//function prototypes
void printEntireGame(WINDOW* window, Player& plyr, std::vector<std::string>* cMap, bool& delayedPrint);
std::vector<std::string> getfovMap (Player& plyr, const std::vector<std::string>* cMap);
void printFovGame (WINDOW* window, Player& plyr, const std::vector<std::string>* cMap, bool& delayedPrint);
void printInfo(WINDOW* window, Player& plyr);
void printLog(WINDOW* window, std::vector<std::string> logs);
void printInv(WINDOW* window, Player& plyr, bool highlight, unsigned short int& invhgNum);
std::string getHG(WINDOW* window, Player& plyr, unsigned short int& invhgNum);
void addLog(std::vector<std::string>& log, std::string message);
void spawnPlayer(Player& plyr, std::vector<std::string>& cMap);
void renderScreen(Player& plyr, const std::vector<std::string>& currentMap, bool& changedMap, bool& highlight, unsigned short int& invhgNum, WINDOW* stdscr, WINDOW* main, WINDOW* info, WINDOW* log, WINDOW* inv, bool delayedPrint);
void moveEnemy(std::vector<std::string>& cMap, Player& plyr);

//here comes the fun part
int main() {
    int input;
    initscr(); //initialize pdcurses
    SetConsoleOutputCP(437);
    if (has_colors() == false) {
        endwin();
        std::cout << "ERROR: Colors are not supported in this console/terminal." << std::endl;
        return 0;
    }
    start_color(); //initialize coloring in pdcurses
    init_pair(1, COLOR_BLACK, COLOR_BLACK);     //empty
    init_pair(2, COLOR_MAGENTA, COLOR_BLACK);   //wall
    init_pair(3, COLOR_WHITE, COLOR_BLACK);     //player
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);    //igold
    init_pair(5, COLOR_CYAN, COLOR_BLACK);      //title
    init_pair(6, COLOR_YELLOW, COLOR_BLACK);    //info
    init_pair(7, COLOR_WHITE, COLOR_BLACK);     //box
    init_pair(8, COLOR_BLACK, COLOR_WHITE);     //highlight
    init_pair(9, COLOR_BLUE, COLOR_BLACK);      //key
    init_pair(10, COLOR_CYAN, COLOR_BLACK);     //chest
    init_pair(11, COLOR_CYAN, COLOR_BLACK);     //trap
    init_pair(12, COLOR_RED, COLOR_BLACK);      //dead
    init_pair(13, COLOR_CYAN, COLOR_BLACK);     //stair
    init_pair(14, COLOR_GREEN, COLOR_BLACK);    //tree
    init_pair(15, COLOR_WHITE, COLOR_CYAN);     //water
    resize_term(16, 24);
    SetConsoleTitle(TEXT(""));
    curs_set(0);            //hide the blinking underline
    keypad(stdscr, true);   //enable keypad to be able to use arrows on keyboard
    noecho();               //disable input feedback
    cbreak();               //idk what this does but according to the PDMANUAL, this enables immediate input instead of buffering
    unsigned int mmhgNum = 0;
    std::vector<std::string> mmList = {
        "Play game",
        "Exit"
    };

    //main menu loop
    do {
        werase(stdscr);
        wattron(stdscr, COLOR_PAIR(TITLE_PAIR));
        mvwprintw(stdscr, 4, 4, "Bored Adventurer");
        mvwprintw(stdscr, 5, 16, "DEMO");
        wattroff(stdscr, COLOR_PAIR(TITLE_PAIR));

        //show selectable options
        for (unsigned int i = 0; i < mmList.size(); i++) {

            wattron(stdscr, COLOR_PAIR(GOLD_PAIR));

            if (i == mmhgNum) {
                mvwprintw(stdscr, 9 + (2*i), 5, ">");   //add the "currently selected" character
                wattron(stdscr, COLOR_PAIR(HG_PAIR));
            } else {
                wattron(stdscr, COLOR_PAIR(BOX_PAIR));
            }
            
            mvwprintw(stdscr, 9 + (2*i), 7, mmList[i].c_str());

            if (i == mmhgNum) {
                wattroff(stdscr, COLOR_PAIR(HG_PAIR));
            } else {
                wattroff(stdscr, COLOR_PAIR(BOX_PAIR));
            }

            wattroff(stdscr, COLOR_PAIR(GOLD_PAIR));
        }
        
        wattron(stdscr, COLOR_PAIR(GOLD_PAIR));
        box(stdscr, 0, 0);
        wattroff(stdscr, COLOR_PAIR(GOLD_PAIR));

        wnoutrefresh(stdscr);
        doupdate();
        input = wgetch(stdscr);
        if (input == KEY_UP) {
            if (mmhgNum != 0) {
                mmhgNum -= 1;
            }
        } else if (input == KEY_DOWN) {
            if (mmhgNum < mmList.size()-1) {
                mmhgNum += 1;
            }
        } else if (input == KEY_ENTER || input == '\r' || input == '\n') {
            if (mmhgNum == 0) {
                break;
            } else if (mmhgNum == 1) {
                return 0;
            }
        }
    } while (true);
    keypad(stdscr, false);      //disable keypad during name input
    echo();                     //return input characters
    curs_set(1);                //blinking cursor
    char playerName[] = {""};   //NOTE: never forget to initialize a C-style string, even if its meant to be empty
    do {
        werase(stdscr);
        mvwprintw(stdscr, 8, 5, "Enter a name: ");

        wattron(stdscr, COLOR_PAIR(GOLD_PAIR));
        box(stdscr, 0, 0);
        wattroff(stdscr, COLOR_PAIR(GOLD_PAIR));

        wnoutrefresh(stdscr);
        doupdate();
        mvwgetnstr(stdscr, 10, 7, playerName, 12);
    } while (playerName[0] == '\0');

    Player player(playerName, "start", 100, 0);  //initialize player
    std::vector<std::string>* currentMap = &maps[player.getLoc()];

    //initialize the sizes for all windows
    int main_x = currentMap->size() + 2;
    int main_y = (*currentMap)[0].size() + 2;
    int info_x = (main_x < 8 ? 8 : main_x);
    int info_y = 19;
    int log_x = 8;
    int log_y = main_y + info_y + 1;
    int inv_x = main_x + log_x;
    int inv_y = 22;
    resize_term((main_x > info_x ? main_x : info_x) + log_x, main_y + info_y + 3 + inv_y + 1);

    //initialize the main windows
    WINDOW* main = newwin(main_x, main_y, 0, 1);
    WINDOW* info = newwin(info_x, info_y, 0, main_y+2);
    WINDOW* log = newwin(log_x, log_y, (main_x > info_x ? main_x : info_x), 1);
    WINDOW* inv = newwin(inv_x, inv_y, 0, main_y + info_y+3);
    keypad(stdscr, true);
    noecho();
    curs_set(0);        //set cursor to invisible

    //initialize logs array
    std::vector<std::string>& logs = player.getLogs();
    for (unsigned int i = 0; i < 6; i++) {
        logs.push_back("");
    }

    resize_term((main_x > info_x ? main_x : info_x) + log_x, main_y + info_y + 3 + inv_y + 1);
    werase(stdscr);

    wattron(stdscr, COLOR_PAIR(TITLE_PAIR));
    box(stdscr, 0, 0);
    wattroff(stdscr, COLOR_PAIR(TITLE_PAIR));

    wnoutrefresh(stdscr);
    doupdate();
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    mvprintw(3, 11, "One day our adventurer got bored and");
    mvprintw(4, 14, "decided to go into the woods...");
    wnoutrefresh(stdscr);
    doupdate();
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    mvprintw(7, 13, "when he suddenly found a mysterious chest");
    wnoutrefresh(stdscr);
    doupdate();
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    mvprintw(13, 19, "Press any key to continue");
    doupdate();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    getch();

    spawnPlayer(player, *currentMap);

    int* pPos = player.getPos();
    int mode = 1;   //1=move, 2=inv
    bool highlight = false; //to highlight items in inventory mode
    unsigned short int invhgNum = 0;
    bool changedMap = false;

    wnoutrefresh(stdscr);
    renderScreen(player, *currentMap, changedMap, highlight, invhgNum, stdscr, main, info, log, inv, true);     //render once before the game loop which starts with player input

    //first game loop
    do {
        input = wgetch(stdscr);             //get player input

        if (mode == 1) {
            if (input == KEY_LEFT) {
                changedMap = player.movePlayer(*currentMap, pPos[0], pPos[1]-1);
            } else if (input == KEY_UP) {
                changedMap = player.movePlayer(*currentMap, pPos[0]-1, pPos[1]);
            } else if (input == KEY_RIGHT) {
                changedMap = player.movePlayer(*currentMap, pPos[0], pPos[1]+1);
            } else if (input == KEY_DOWN) {
                changedMap = player.movePlayer(*currentMap, pPos[0]+1, pPos[1]);
            } else if (input == 'i') {
                highlight = true;
                mode = 2;
            } else if (input == KEY_RESIZE) {
                resize_term((main_x > info_x ? main_x : info_x) + log_x, main_y + info_y + 3 + inv_y + 1);
                curs_set(0);
            }
            if (changedMap == true) {       //if map has changed
                currentMap = &maps[player.getLoc()];
                int main_x = currentMap->size() + 2;
                int main_y = (*currentMap)[0].size() + 2;
                int info_x = (main_x < 8 ? 8 : main_x);
                int log_y = main_y + info_y + 1;
                int inv_x = main_x + log_x;
                resize_term((main_x > info_x ? main_x : info_x) + log_x, main_y + info_y + 3 + inv_y + 1);
                resize_window(main, main_x, main_y);
                mvwin(main, 0, 1);
                resize_window(info, info_x, info_y);
                mvwin(info, 0, main_y+2);
                resize_window(log, log_x, log_y);
                mvwin(log, (main_x > info_x ? main_x : info_x), 1);
                resize_window(inv, inv_x, inv_y);
                mvwin(inv, 0, main_y + info_y+3);
                spawnPlayer(player, *currentMap);
            }
        } else if (mode == 2) {             //if in inventory
            if (input == KEY_UP && invhgNum > 0) {
                invhgNum -= 1;
            } else if (input == KEY_UP && invhgNum == 0) {
                invhgNum = player.getInv().size() - 1;
            } else if (input == KEY_DOWN && invhgNum < player.getInv().size() - 1) {
                invhgNum += 1;
            } else if (input == KEY_DOWN && invhgNum == player.getInv().size() - 1) {
                invhgNum = 0;
            } else if (input == 'e') {
                player.useItem(getHG(inv, player, invhgNum));
            } else if (input == 'd') {
                player.delItem(getHG(inv, player, invhgNum), 1);
            } else if (input == 'i') {
                highlight = false;
                mode = 1;
            }
        }

        renderScreen(player, *currentMap, changedMap, highlight, invhgNum, stdscr, main, info, log, inv, false);

        //set a little delay between input
        if (mode == 1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    } while (player.getWeapon() != "Iron_Sword");   //set a requirement for the starter map

    highlight = false;
    mode = 1;

    werase(stdscr);

    wattron(stdscr, COLOR_PAIR(TITLE_PAIR));
    box(stdscr, 0, 0);
    wattroff(stdscr, COLOR_PAIR(TITLE_PAIR));

    wnoutrefresh(stdscr);
    doupdate();
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    mvprintw(3, 11, "He found a sword");
    mvprintw(4, 15, "with weird shining symbols on it");
    wnoutrefresh(stdscr);
    doupdate();
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    mvprintw(7, 13, "he then decided to go home.");
    wnoutrefresh(stdscr);
    doupdate();
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    mvprintw(13, 19, "Press any key to continue");
    wnoutrefresh(stdscr);
    doupdate();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    getch();

    werase(stdscr);
    wnoutrefresh(stdscr);
    doupdate();

    //change loc from "start" to "forest" and update screen accordingly
    player.changeLoc("forest");
    currentMap = &maps[player.getLoc()];
    changedMap = true;
    spawnPlayer(player, *currentMap);

    main_x = (*currentMap).size() + 2;
    main_y = (*currentMap)[0].size() + 2;
    info_x = (main_x < 8 ? 8 : main_x);
    log_y = main_y + info_y + 1;
    inv_x = main_x + log_x;
    resize_term((main_x > info_x ? main_x : info_x) + log_x, main_y + info_y + 3 + inv_y + 1);
    resize_window(main, main_x, main_y);
    mvwin(main, 0, 1);
    resize_window(info, info_x, info_y);
    mvwin(info, 0, main_y+2);
    resize_window(log, log_x, log_y);
    mvwin(log, (main_x > info_x ? main_x : info_x), 1);
    resize_window(inv, inv_x, inv_y);
    mvwin(inv, 0, main_y + info_y+3);
    werase(main);
    box(main, 0, 0);
    wnoutrefresh(main);
    doupdate();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    renderScreen(player, *currentMap, changedMap, highlight, invhgNum, stdscr, main, info, log, inv, true);     //render screen once more before player input

    do {
        input = wgetch(stdscr);             //get player input

        if (mode == 1) {
            if (input == KEY_LEFT) {
                changedMap = player.movePlayer(*currentMap, pPos[0], pPos[1]-1);
            } else if (input == KEY_UP) {
                changedMap = player.movePlayer(*currentMap, pPos[0]-1, pPos[1]);
            } else if (input == KEY_RIGHT) {
                changedMap = player.movePlayer(*currentMap, pPos[0], pPos[1]+1);
            } else if (input == KEY_DOWN) {
                changedMap = player.movePlayer(*currentMap, pPos[0]+1, pPos[1]);
            } else if (input == 'i') {
                highlight = true;
                mode = 2;
            } else if (input == KEY_RESIZE) {
                resize_term((main_x > info_x ? main_x : info_x) + log_x, main_y + info_y + 3 + inv_y + 1);
                curs_set(0);
            }
            if (changedMap == true) {       //if map has changed
                currentMap = &maps[player.getLoc()];
                int main_x = (*currentMap).size() + 2;
                int main_y = (*currentMap)[0].size() + 2;
                int info_x = (main_x < 8 ? 8 : main_x);
                int log_y = main_y + info_y + 1;
                int inv_x = main_x + log_x;
                resize_term((main_x > info_x ? main_x : info_x) + log_x, main_y + info_y + 3 + inv_y + 1);
                resize_window(main, main_x, main_y);
                mvwin(main, 0, 1);
                resize_window(info, info_x, info_y);
                mvwin(info, 0, main_y+2);
                resize_window(log, log_x, log_y);
                mvwin(log, (main_x > info_x ? main_x : info_x), 1);
                resize_window(inv, inv_x, inv_y);
                mvwin(inv, 0, main_y + info_y+3);
                spawnPlayer(player, *currentMap);
            }
        } else if (mode == 2) {             //if in inventory
            if (input == KEY_UP && invhgNum > 0) {
                invhgNum -= 1;
            } else if (input == KEY_UP && invhgNum == 0) {
                invhgNum = player.getInv().size() - 1;
            } else if (input == KEY_DOWN && invhgNum < player.getInv().size() - 1) {
                invhgNum += 1;
            } else if (input == KEY_DOWN && invhgNum == player.getInv().size() - 1) {
                invhgNum = 0;
            } else if (input == 'e') {
                player.useItem(getHG(inv, player, invhgNum));
            } else if (input == 'd') {
                player.delItem(getHG(inv, player, invhgNum), 1);
            } else if (input == 'i') {
                highlight = false;
                mode = 1;
            }
        }
        
        moveEnemy(*currentMap, player);

        renderScreen(player, *currentMap, changedMap, highlight, invhgNum, stdscr, main, info, log, inv, false);

        //set a little delay between input
        if (mode == 1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    } while (!player.hasItem("Golden Amulet"));     //set a requirement for the end

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    werase(stdscr);

    wattron(stdscr, COLOR_PAIR(TITLE_PAIR));
    box(stdscr, 0, 0);
    wattroff(stdscr, COLOR_PAIR(TITLE_PAIR));

    wnoutrefresh(stdscr);
    doupdate();
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    mvprintw(4, 31, "THE END");
    wnoutrefresh(stdscr);
    doupdate();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    mvprintw(8, 25, "Created by VoidL_rd");
    wnoutrefresh(stdscr);
    doupdate();
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    mvprintw(16, 19, "Press any key to close game");
    wnoutrefresh(stdscr);
    doupdate();
    getch();

    endwin();   //correctly end the curses window
    return 0;
}

//this function prints out the *ENTIRE* game to the screen
void printEntireGame(WINDOW* window, Player& plyr, const std::vector<std::string>* cMap, bool& delayedPrint) {
    for (unsigned int i = 0; i < (*cMap).size(); i++) {
        for (unsigned int j = 0; j < (*cMap)[i].size(); j++) {
            char chr = (*cMap)[i][j];
            if (chr == PLAYER) {                //if its a player
                if (plyr.getHealth() == 0) {
                    wattron(window, COLOR_PAIR(DEAD_PAIR));
                    mvwaddch(window, i + 1, j + 1, chr);
                    wattroff(window, COLOR_PAIR(DEAD_PAIR));
                } else {
                    wattron(window, COLOR_PAIR(PLAYER_PAIR));
                    mvwaddch(window, i + 1, j + 1, chr);
                    wattroff(window, COLOR_PAIR(PLAYER_PAIR));
                }
            } else if (chr == EMPTY) {          //if its empty space
                wattron(window, COLOR_PAIR(EMPTY_PAIR));
                mvwaddch(window, i + 1, j + 1, ' ');
                wattroff(window, COLOR_PAIR(EMPTY_PAIR));
            } else if (chr == WALL) {           //if its a wall
                wattron(window, COLOR_PAIR(WALL_PAIR));
                mvwaddch(window, i + 1, j + 1, L'▒');
                wattroff(window, COLOR_PAIR(WALL_PAIR));
            } else if (chr == TREE) {           //if its a tree
                wattron(window, A_DIM);
                wattron(window, COLOR_PAIR(TREE_PAIR));
                mvwaddch(window, i + 1, j + 1, L'♣');
                wattroff(window, COLOR_PAIR(TREE_PAIR));
                wattroff(window, A_DIM);
            } else if (chr == WATER) {          //if its water
                wattron(window, COLOR_PAIR(WATER_PAIR));
                mvwaddch(window, i + 1, j + 1, L'≈');
                wattroff(window, COLOR_PAIR(WATER_PAIR));
            } else if (chr == IGOLD) {          //if its item gold
                wattron(window, COLOR_PAIR(GOLD_PAIR));
                mvwaddch(window, i + 1, j + 1, L'©');
                wattroff(window, COLOR_PAIR(GOLD_PAIR));
            } else if (chr == KEY) {            //if its item gold
                wattron(window, COLOR_PAIR(KEY_PAIR));
                mvwaddch(window, i + 1, j + 1, L'⌐');
                wattroff(window, COLOR_PAIR(KEY_PAIR));
            } else if (chr == CHEST) {          //if its item gold
                wattron(window, COLOR_PAIR(CHEST_PAIR));
                mvwaddch(window, i + 1, j + 1, L'□');
                wattroff(window, COLOR_PAIR(CHEST_PAIR));
            } else if (chr == H_TRAP) {         //if its a hidden trap
                wattron(window, COLOR_PAIR(EMPTY_PAIR));
                mvwaddch(window, i + 1, j + 1, ' ');
                wattroff(window, COLOR_PAIR(EMPTY_PAIR));
            } else if (chr == V_TRAP) {         //if its a visible trap
                wattron(window, COLOR_PAIR(TRAP_PAIR));
                mvwaddch(window, i + 1, j + 1, L'×');
                wattroff(window, COLOR_PAIR(TRAP_PAIR));
            } else if (chr == U_STAIR) {        //if its up-stairs
                wattron(window, COLOR_PAIR(STAIR_PAIR));
                mvwaddch(window, i + 1, j + 1, L'▲');
                wattroff(window, COLOR_PAIR(STAIR_PAIR));
            } else if (chr == D_STAIR) {        //if its down-stairs
                wattron(window, COLOR_PAIR(STAIR_PAIR));
                mvwaddch(window, i + 1, j + 1, L'▼');
                wattroff(window, COLOR_PAIR(STAIR_PAIR));
            } else if (chr == ENEMY) {      //if its enemy
                wattron(window, COLOR_PAIR(PLAYER_PAIR));
                mvwaddch(window, i + 1, j + 1, L'ô');
                wattroff(window, COLOR_PAIR(PLAYER_PAIR));
            } else if (chr == BOSS) {       //if its boss
                wattron(window, COLOR_PAIR(PLAYER_PAIR));
                mvwaddch(window, i + 1, j + 1, L'☻');
                wattroff(window, COLOR_PAIR(PLAYER_PAIR));
            } else {                            //if unknown
                mvwaddch(window, i + 1, j + 1, chr);
            }
            if (delayedPrint == true) {         //delayed print for animated map loading
                Sleep(10);
                wnoutrefresh(window);
                doupdate();
            }
        }
        printw("\n");
    }
}

//return FOV of current map
std::vector<std::string> getfovMap (Player& plyr, const std::vector<std::string>* cMap) {
    std::vector<std::string> fovMap;
    int* pP = plyr.getPos();
    int x = pP[0];
    int y = pP[1];
    //first generate a fov map
    for (unsigned int i = 0; i < (*cMap).size(); i++) {
        std::string temp;
        for (unsigned int j = 0; j < (*cMap)[0].size(); j++) {
            bool visible = false;
            if ((i == x - 1) && (j == y - 1)) {         //above-left
                visible = true;
            } else if ((i == x - 1) && (j == y)) {      //above
                visible = true;
            } else if ((i == x - 1) && (j == y + 1)) {  //above-right
                visible = true;
            } else if ((i == x) && (j == y - 1)) {      //left
                visible = true;
            } else if ((i == x) && (j == y + 1)) {      //right
                visible = true;
            } else if ((i == x + 1) && (j == y - 1)) {  //under-left
                visible = true;
            } else if ((i == x + 1) && (j == y)) {      //under
                visible = true;
            } else if ((i == x + 1) && (j == y + 1)) {  //under-right
                visible = true;
            }

            if (visible == true) {
                temp.push_back('v');
            } else if (visible == false) {
                temp.push_back('i');
            }
        }
        fovMap.push_back(temp);
    }
    //then extend the vision to 2 blocks
    if ((*cMap)[x-1][y] != WALL && (*cMap)[x-1][y] != TREE && x != 1) {                     //above
        fovMap[x-2][y-1] = 'v';
        fovMap[x-2][y] = 'v';
        fovMap[x-2][y+1] = 'v';
    }
    if ((*cMap)[x][y-1] != WALL && (*cMap)[x][y-1] != TREE && y != 1) {                     //left
        fovMap[x-1][y-2] = 'v';
        fovMap[x][y-2] = 'v';
        fovMap[x+1][y-2] = 'v';
    }
    if ((*cMap)[x][y+1] != WALL && (*cMap)[x][y+1] != TREE && y != (*cMap)[0].size()-2) {   //right
        fovMap[x-1][y+2] = 'v';
        fovMap[x][y+2] = 'v';
        fovMap[x+1][y+2] = 'v';
    }
    if ((*cMap)[x+1][y] != WALL && (*cMap)[x+1][y] != TREE && x != (*cMap).size()-2) {      //under
        fovMap[x+2][y-1] = 'v';
        fovMap[x+2][y] = 'v';
        fovMap[x+2][y+1] = 'v';
    }
    return fovMap;
};

//this function prints the game parts *VISIBLE* to player
void printFovGame (WINDOW* window, Player& plyr, const std::vector<std::string>* cMap, bool& delayedPrint) { 

    //get fovMap
    std::vector<std::string> fovMap = getfovMap(plyr, cMap);

    //then print out whatever is visible in fovMap
    for (unsigned int i = 0; i < (*cMap).size(); i++) {
        for (unsigned int j = 0; j < (*cMap)[i].size(); j++) {
            char chr = (*cMap)[i][j];
            bool visible = false;

            if (fovMap[i][j] == 'v') {
                visible = true;
            } else {
                visible = false;
            }

            if (visible == true || chr == PLAYER) {
                if (chr == PLAYER) {
                    if (plyr.getHealth() == 0) {    //if player is dead
                        wattron(window, COLOR_PAIR(DEAD_PAIR));
                        mvwaddch(window, i + 1, j + 1, chr);
                        wattroff(window, COLOR_PAIR(DEAD_PAIR));
                    } else {
                        wattron(window, COLOR_PAIR(PLAYER_PAIR));
                        mvwaddch(window, i + 1, j + 1, chr);
                        wattroff(window, COLOR_PAIR(PLAYER_PAIR));
                    }
                } else if (chr == EMPTY) {          //if its empty space
                    wattron(window, COLOR_PAIR(EMPTY_PAIR));
                    mvwaddch(window, i + 1, j + 1, ' ');
                    wattroff(window, COLOR_PAIR(EMPTY_PAIR));
                } else if (chr == WALL) {           //if its a wall
                    wattron(window, COLOR_PAIR(WALL_PAIR));
                    mvwaddch(window, i + 1, j + 1, L'▒');
                    wattroff(window, COLOR_PAIR(WALL_PAIR));
                } else if (chr == TREE) {           //if its a tree
                    wattron(window, A_DIM);
                    wattron(window, COLOR_PAIR(TREE_PAIR));
                    mvwaddch(window, i + 1, j + 1, L'♣');
                    wattroff(window, COLOR_PAIR(TREE_PAIR));
                    wattroff(window, A_DIM);
                } else if (chr == WATER) {          //if its water
                    wattron(window, COLOR_PAIR(WATER_PAIR));
                    mvwaddch(window, i + 1, j + 1, L'≈');
                    wattroff(window, COLOR_PAIR(WATER_PAIR));
                } else if (chr == IGOLD) {          //if its item gold
                    wattron(window, COLOR_PAIR(GOLD_PAIR));
                    mvwaddch(window, i + 1, j + 1, L'©');
                    wattroff(window, COLOR_PAIR(GOLD_PAIR));
                } else if (chr == KEY) {            //if its item gold
                    wattron(window, COLOR_PAIR(KEY_PAIR));
                    mvwaddch(window, i + 1, j + 1, L'⌐');
                    wattroff(window, COLOR_PAIR(KEY_PAIR));
                } else if (chr == CHEST) {          //if its item gold
                    wattron(window, COLOR_PAIR(CHEST_PAIR));
                    mvwaddch(window, i + 1, j + 1, L'□');
                    wattroff(window, COLOR_PAIR(CHEST_PAIR));
                } else if (chr == H_TRAP) {         //if its a hidden trap
                    wattron(window, COLOR_PAIR(EMPTY_PAIR));
                    mvwaddch(window, i + 1, j + 1, ' ');
                    wattroff(window, COLOR_PAIR(EMPTY_PAIR));
                } else if (chr == V_TRAP) {         //if its a visible trap
                    wattron(window, COLOR_PAIR(TRAP_PAIR));
                    mvwaddch(window, i + 1, j + 1, L'×');
                    wattroff(window, COLOR_PAIR(TRAP_PAIR));
                } else if (chr == U_STAIR) {        //if its up-stairs
                    wattron(window, COLOR_PAIR(STAIR_PAIR));
                    mvwaddch(window, i + 1, j + 1, L'▲');
                    wattroff(window, COLOR_PAIR(STAIR_PAIR));
                } else if (chr == D_STAIR) {        //if its down-stairs
                    wattron(window, COLOR_PAIR(STAIR_PAIR));
                    mvwaddch(window, i + 1, j + 1, L'▼');
                    wattroff(window, COLOR_PAIR(STAIR_PAIR));
                } else if (chr == ENEMY) {          //if its enemy
                    wattron(window, COLOR_PAIR(PLAYER_PAIR));
                    mvwaddch(window, i + 1, j + 1, L'ô');
                    wattroff(window, COLOR_PAIR(PLAYER_PAIR));
                } else if (chr == BOSS) {           //if its boss
                    wattron(window, COLOR_PAIR(PLAYER_PAIR));
                    mvwaddch(window, i + 1, j + 1, L'☻');
                    wattroff(window, COLOR_PAIR(PLAYER_PAIR));
                } else {                            //if unknown just print it in black and white
                    mvwaddch(window, i + 1, j + 1, chr);
                }
                continue;
            } else {
                mvwaddch(window, i + 1, j + 1, EMPTY);
            }
            if (delayedPrint == true) {             //delayed print for animated map loading
                Sleep(10);
                wnoutrefresh(window);
                doupdate();
            }
        }
        printw("\n");
    }
}

//print all the stuff to info window
void printInfo(WINDOW* window, Player& plyr) {
    std::string name = plyr.getName();
    int health = plyr.getHealth();
    int gold = plyr.getGold();
    int* pos = plyr.getPos();

    wattron(window, COLOR_PAIR(TITLE_PAIR));
    mvwprintw(window, 1, 1, name.c_str());          //name
    wattron(window, COLOR_PAIR(TITLE_PAIR));

    wattron(window, COLOR_PAIR(INFO_PAIR));
    std::string temp = std::to_string(health);
    mvwprintw(window, 3, 1, temp.c_str());          //health
    wprintw(window, " hp");
    temp = std::to_string(gold);
    mvwprintw(window, 4, 1, temp.c_str());          //gold
    wprintw(window, " g");
    mvwprintw(window, 5, 1, plyr.getLoc().c_str()); //loc
    wprintw(window, ": ");
    temp = std::to_string(pos[0]);                  //pos
    temp.push_back(',');
    std::string tempI = std::to_string(pos[1]);
    wprintw(window, temp.c_str());                  //pos1
    wprintw(window, tempI.c_str());                 //pos2
    wattroff(window, COLOR_PAIR(INFO_PAIR));
}

//print stuff to log window
void printLog(WINDOW* window, std::vector<std::string> logs) {
    for (unsigned int i = 0; i < logs.size(); i++) {
        mvwprintw(window, i + 1, 1, logs[i].c_str());
    }
}

//add stuff to log window
void addLog(std::vector<std::string>& logs, std::string message) {
    for (unsigned int i = 0; i < logs.size()-1; i++) {
        logs[i] = logs[i+1];
    }
    logs[5] = message;
}

//print stuff to inventory window
void printInv(WINDOW* window, Player& plyr, bool highlight, unsigned short int& invhgNum) {
    std::map<std::string, unsigned int> inventory = plyr.getInv();
    unsigned int i = 0;

    //iterate through player's inventory
    for(std::map<std::string, unsigned int>::iterator it = inventory.begin(); it != inventory.end(); it++) {
        int amount = it->second;    //get amount of the item

        if (amount <= 0) {
            continue;
        }

        if (i == invhgNum && highlight == true) {       //if its being highlighted
            wattron(window, COLOR_PAIR(HG_PAIR));
        } else {
            wattron(window, COLOR_PAIR(BOX_PAIR));
        }

        mvwprintw(window, i+1, 1, (it->first).c_str());
        wprintw(window, ": ");
        wprintw(window, (std::to_string(amount)).c_str());

        if (plyr.getWeapon() == it->first) {            //if its equipped
            wprintw(window, " [e]");
        }

        if (i == invhgNum && highlight == true) {
            wattroff(window, COLOR_PAIR(HG_PAIR));
        } else {
            wattroff(window, COLOR_PAIR(BOX_PAIR));
        }

        i++;
    }
}

//get the name of the highlighted item in player's inventory
std::string getHG(WINDOW* window, Player& plyr, unsigned short int& invhgNum) {
    std::map<std::string, unsigned int> inventory = plyr.getInv();
    std::string highlighted;
    unsigned int i = 0;

    //iterate through player's inventory
    for(std::map<std::string, unsigned int>::iterator it = inventory.begin(); it != inventory.end(); it++) {
        if (i == invhgNum) {
            highlighted = it->first;
        }

        i++;
    }

    return highlighted;
}

//set player position to the one within the current map
void spawnPlayer(Player& plyr, std::vector<std::string>& cMap) {
    for (unsigned int i = 0; i < cMap.size(); i++) {
        for (unsigned int j = 0; j < cMap[i].size(); j++) {
            if (cMap[i][j] == PLAYER) {
                plyr.setPos(i, j);
            }
        }
    }
}

//render screen
void renderScreen(Player& plyr, const std::vector<std::string>& currentMap, bool& changedMap, bool& highlight, unsigned short int& invhgNum, WINDOW* stdscr, WINDOW* main, WINDOW* info, WINDOW* log, WINDOW* inv, bool delayedPrint) {
    std::vector<std::string>& logs = plyr.getLogs();
    werase(stdscr);
    werase(main);
    werase(info);
    werase(log);
    werase(inv);

    wattron(main, COLOR_PAIR(GOLD_PAIR));
    wborder(main, L'║', L'║', L'═', L'═', L'╔', L'╗', L'╚', L'╝');
    wattroff(main, COLOR_PAIR(GOLD_PAIR));

    box(info, 0, 0);
    box(log, 0, 0);
    box(inv, 0, 0);

    wnoutrefresh(stdscr);
    
    printInfo(info, plyr);
    printLog(log, logs);
    printInv(inv, plyr, highlight, invhgNum);
    printFovGame(main, plyr, &currentMap, delayedPrint);

    wattron(main, COLOR_PAIR(GOLD_PAIR));
    wborder(main, L'║', L'║', L'═', L'═', L'╔', L'╗', L'╚', L'╝');
    wattroff(main, COLOR_PAIR(GOLD_PAIR));

    box(info, 0, 0);
    box(log, 0, 0);
    box(inv, 0, 0);

    wnoutrefresh(stdscr);
    wnoutrefresh(main);
    wnoutrefresh(info);
    wnoutrefresh(log);
    wnoutrefresh(inv);
    doupdate();
}

//move enemies in current map
void moveEnemy(std::vector<std::string>& cMap, Player& plyr) {
    int x = 0;
    int y = 0;

    for (unsigned int i = 0; i < cMap.size(); i++) {
        for (unsigned int j = 0; j < cMap[i].size(); j++) {
            if (cMap[i][j] == ENEMY) {
                x = i;
                y = j;
                
                //check around itself and attack
                if (cMap[x-1][y] == PLAYER) {               //top
                    plyr.Damage(rand() % 6 + 8);
                    break;
                } else if (cMap[x][y-1] == PLAYER) {        //left
                    plyr.Damage(rand() % 6 + 8);
                    break;
                } else if (cMap[x][y+1] == PLAYER) {        //right
                    plyr.Damage(rand() % 6 + 8);
                    break;
                } else if (cMap[x+1][y] == PLAYER) {        //bottom
                    plyr.Damage(rand() % 6 + 8);
                    break;
                }

                //check vertically and horizontally
                if (cMap[x-1][y] != WALL && cMap[x-1][y] != TREE && cMap[x-1][y] != CHEST && cMap[x-1][y] != KEY && x != 1) {                   //top
                    if (cMap[x-2][y] == PLAYER) {
                        cMap[x-1][y] = ENEMY;
                        cMap[x][y] = EMPTY;
                    }
                }
                if (cMap[x][y-1] != WALL && cMap[x][y-1] != TREE && cMap[x][y-1] != CHEST && cMap[x][y-1] != KEY && y != 1) {                   //left
                    if (cMap[x][y-2] == PLAYER) {
                        cMap[x][y-1] = ENEMY;
                        cMap[x][y] = EMPTY;
                    }
                }
                if (cMap[x][y+1] != WALL && cMap[x][y+1] != TREE && cMap[x][y+1] != CHEST && cMap[x][y+1] != KEY && y != cMap[0].size()-2) {    //right
                    if (cMap[x][y+2] == PLAYER) {
                        cMap[x][y+1] = ENEMY;
                        cMap[x][y] = EMPTY;
                    }
                }
                if (cMap[x+1][y] != WALL && cMap[x+1][y] != TREE && cMap[x+1][y] != CHEST && cMap[x+1][y] != KEY && x != cMap.size()-2) {       //bottom
                    if (cMap[x+2][y] == PLAYER) {
                        cMap[x+1][y] = ENEMY;
                        cMap[x][y] = EMPTY;
                    }
                }

                //check diagonally
                if (cMap[x-1][y-1] == PLAYER) {                                                                                         //top left
                    if (cMap[x][y-1] != WALL && cMap[x][y-1] != TREE && cMap[x][y-1] != CHEST && cMap[x][y-1] != KEY && x != 1) {               //left
                        cMap[x][y-1] = ENEMY;
                        cMap[x][y] = EMPTY;
                    } else if (cMap[x-1][y] != WALL && cMap[x-1][y] != TREE && cMap[x-1][y] != CHEST && cMap[x-1][y] != KEY && x != 1) {        //top
                        cMap[x-1][y] = ENEMY;
                        cMap[x][y] = EMPTY;
                    }
                }
                if (cMap[x-1][y+1] == PLAYER) {                                                                                         //top right
                    if (cMap[x][y+1] != WALL && cMap[x][y+1] != TREE && cMap[x][y+1] != CHEST && cMap[x][y+1] != KEY && x != 1) {               //right
                        cMap[x][y+1] = ENEMY;
                        cMap[x][y] = EMPTY;
                    } else if (cMap[x-1][y] != WALL && cMap[x-1][y] != TREE && cMap[x-1][y] != CHEST && cMap[x-1][y] != KEY && x != 1) {        //top
                        cMap[x-1][y] = ENEMY;
                        cMap[x][y] = EMPTY;
                    }
                }
                if (cMap[x+1][y-1] == PLAYER) {                                                                                         //bottom left
                    if (cMap[x][y-1] != WALL && cMap[x][y-1] != TREE && cMap[x][y-1] != CHEST && cMap[x][y-1] != KEY && x != 1) {               //left
                        cMap[x][y-1] = ENEMY;
                        cMap[x][y] = EMPTY;
                    } else if (cMap[x+1][y] != WALL && cMap[x+1][y] != TREE && cMap[x+1][y] != CHEST && cMap[x+1][y] != KEY && x != 1) {        //bottom
                        cMap[x+1][y] = ENEMY;
                        cMap[x][y] = EMPTY;
                    }
                }                                                                                                                       //bottom right
                if (cMap[x+1][y+1] == PLAYER) {
                    if (cMap[x][y+1] != WALL && cMap[x][y+1] != TREE && cMap[x][y+1] != CHEST && cMap[x][y+1] != KEY && x != 1) {               //right
                        cMap[x][y+1] = ENEMY;
                        cMap[x][y] = EMPTY;
                    } else if (cMap[x+1][y] != WALL && cMap[x+1][y] != TREE && cMap[x+1][y] != CHEST && cMap[x+1][y] != KEY && x != 1) {        //bottom
                        cMap[x+1][y] = ENEMY;
                        cMap[x][y] = EMPTY;
                    }
                }
            }
        }
    }
}

//hello