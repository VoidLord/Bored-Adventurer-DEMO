#include "Player.h"
#include <cstdlib>

Player::Player(const std::string name, std::string startLoc, int health, int gold):
m_name(name),
m_currentMap(startLoc),
m_health(health),
m_gold(gold)
{}

bool Player::movePlayer(std::vector<std::string>& cMap, int x, int y) {
    int* pPos = this->getPos();
    if (cMap[pPos[0]][pPos[1]] == '@' && m_health != 0) {
        if (cMap[x][y] != '#' && cMap[x][y] != 't' && cMap[x][y] != 'c' && cMap[x][y] != 'X' && cMap[x][y] != 'x' && cMap[x][y] != '^' && cMap[x][y] != 'v') {
            if (cMap[x][y] == '*') {
                this->giveGold(1);
            } else if (cMap[x][y] == 'k') {
                this->giveItem("Key", 1);
            }
            cMap[pPos[0]][pPos[1]] = ' ';
            cMap[x][y] = '@';
            this->setPos(x, y);
        } else if (cMap[x][y] == 'c') { //if step on chest
            if (m_inventory.find("Key") != m_inventory.end()) {
                this->delItem("Key", 1, false);
                this->addLog(m_name + " used a Key");
                if (m_currentMap == "start1") {
                    this->giveItem("Iron_Sword", 1);
                } else {
                    this->giveGold((rand() % 20 + 20)); //random number between 20-40
                }
                cMap[x][y] = ' ';
            }
        } else if (cMap[x][y] == 'X') {
            this->Damage(15);
            cMap[x][y] = 'x';
        } else if (cMap[x][y] == '^') {
            if (m_currentMap == "test1") {
                this->changeLoc("test2");
                return true;
            }
        } else if (cMap[x][y] == 'v') {
            if (m_currentMap == "test2") {
                this->changeLoc("test1");
                return true;
            }
        }
    }
    return false;
}

void Player::giveGold(int amount) {
    m_gold += amount;
    if (amount > 1) {
        this->addLog(m_name + " found " + std::to_string(amount) + "g");
    }
}

void Player::Heal(int amount) {
    m_health += amount;
    if (m_health > 100) {
        m_health -= m_health % 100;
    }
}

void Player::Damage(int amount) {
    if (m_health < amount) {
        m_health = 0;
        this->addLog(m_name + " died");
    } else {
        m_health -= amount;
    }
}

std::string Player::getName() {
    return m_name;
}

int Player::getHealth() {
    return m_health;
}

int Player::getGold() {
    return m_gold;
}

std::string Player::getWeapon() {
    return m_equippedWeapon;
}

int* Player::getPos() {
    return m_pos;
}

void Player::setPos(int x, int y) {
    m_pos[0] = x;
    m_pos[1] = y;
}

std::string Player::getLoc() {
    return m_currentMap;
}

void Player::changeLoc(std::string loc) {
    m_currentMap = loc;
    this->addLog(m_name + " moved to " + loc);
}

std::map<std::string, unsigned int> Player::getInv() {
    return m_inventory;
}

std::vector<std::string>& Player::getLogs() {
    return m_logs;
}

void Player::addLog(std::string message) {
    for (unsigned int i = 0; i < m_logs.size()-1; i++) {
        m_logs[i] = m_logs[i+1];
    }
    m_logs[5] = message;
}

void Player::clearLogs() {
    for (unsigned int i = 0; i < m_logs.size()-1; i++) {
        m_logs[i] = "";
    }
}

void Player::giveItem(std::string item, int amount) {
    if (m_inventory.find(item) != m_inventory.end()) {
        m_inventory[item] += amount;
    } else {
        m_inventory.insert({item, amount});
    }
    std::string temp = m_name + " got " + std::to_string(amount) + " " + item;
    this->addLog(temp);
}

void Player::useItem(std::string item) {
    if (m_equippedWeapon == item) {
        m_equippedWeapon = "";
        this->addLog(m_name + " unequipped " + item);
    } else if (item == "Iron_Sword") {
        m_equippedWeapon = item;
        this->addLog(m_name + " equipped " + item);
    } else if (item == "Health_Potion") {
        if (m_health != 100) {
            delItem(item, 1, false);
            this->addLog(m_name + " used " + item);
            Heal(25);
        }
    }
}

void Player::delItem(std::string item, int amount, bool output) {
    if (m_inventory[item] != 0 && item != m_equippedWeapon) {
        m_inventory[item] -= amount;
        if (output == true) {
            this->addLog(m_name + " dropped 1 " + item);
        }
    }
    if (m_inventory[item] == 0) {
        m_inventory.erase(item);
    }
}