#ifndef Player_h
#define Player_h

#include <string>
#include <vector>
#include <map>

class Player
{
private:
    std::string m_name;
    int m_health;
    int m_gold;
    int pos[2] = {0,0};
    std::map<std::string, int> inventory;
public:
    Player(const std::string& name, int health = 100, int gold = 0);
    void movePlayer(std::vector<std::string>& cMap, int x, int y);
    int* getPos();
    void setPos(int x, int y);
    std::string getName();
    int getHealth();
    int getGold();
    void giveGold(int amount);
    std::map<std::string, int> getInv();
    void giveItem(std::string item, int amount);
};

#endif