#include "Player.h"

Player::Player(const std::string& name, int health, int gold):
m_name(name),
m_health(health),
m_gold(gold)
{}

void Player::movePlayer(std::vector<std::string>& cMap, int x, int y) {
    int* pPos = this->getPos();
    if (cMap[pPos[0]][pPos[1]] == '@') {
        if (cMap[x][y] != '#' && cMap[x][y] != 'c') {
            if (cMap[x][y] == '*') {
                this->giveGold(1);
            } else if (cMap[x][y] == 'k') {
                this->giveItem("Key", 1);
            }
            cMap[pPos[0]][pPos[1]] = ' ';
            cMap[x][y] = '@';
            this->setPos(x, y);
        } else if (cMap[x][y] == 'c') {
            if (inventory.find("Key") != inventory.end()) {
                this->delItem("Key", 1);
                std::string temp = m_name + " used a Key";
                this->addLog(temp);
                this->giveItem("Iron_Sword", 1);
                this->giveItem("Health_Potion", 3);
                cMap[x][y] = ' ';
            }
        }
    }
}

void Player::giveGold(int amount) {
    m_gold += amount;
}

void Player::Heal(int amount) {
    m_health += amount;
    if (m_health > 100) {
        m_health -= m_health % 100;
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
    return equippedWeapon;
}

int* Player::getPos() {
    return pos;
}

void Player::setPos(int x, int y) {
    pos[0] = x;
    pos[1] = y;
}

std::map<std::string, unsigned int> Player::getInv() {
    return inventory;
}

std::vector<std::string>& Player::getLogs() {
    return logs;
}

void Player::addLog(std::string message) {
    for (unsigned int i = 0; i < logs.size()-1; i++) {
        logs[i] = logs[i+1];
    }
    logs[5] = message;
}

void Player::giveItem(std::string item, int amount) {
    if (inventory.find(item) != inventory.end()) {
        inventory[item] += amount;
    } else {
        inventory.insert({item, amount});
    }
    std::string temp = m_name + " got " + std::to_string(amount) + " " + item;
    this->addLog(temp);
}

void Player::useItem(std::string item) {
    if (equippedWeapon == item) {
        equippedWeapon = "";
        this->addLog(m_name + " unequipped " + item);
    } else if (item == "Iron_Sword") {
        equippedWeapon = item;
        this->addLog(m_name + " equipped " + item);
    } else if (item == "Health_Potion") {
        if (m_health != 100) {
            delItem(item, 1);
            this->addLog(m_name + " used " + item);
            Heal(25);
        }
    }
}

void Player::delItem(std::string item, int amount) {
    if (inventory[item] != 0 && item != equippedWeapon) {
        inventory[item] -= amount;
        this->addLog(m_name + " deleted 1 " + item);
    }
    if (inventory[item] == 0) {
        inventory.erase(item);
    }
}