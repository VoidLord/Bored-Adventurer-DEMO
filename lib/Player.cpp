#include "Player.h"

Player::Player(const std::string& name, int health, int gold):
m_name(name),
m_health(health),
m_gold(gold)
{}

void Player::movePlayer(std::vector<std::string>& cMap, int x, int y) {
    int* pPos = this->getPos();
    if (cMap[pPos[0]][pPos[1]] == '@') {
        if (cMap[x][y] != '#') {
            if (cMap[x][y] == '*') {
                this->giveGold(1);
            }
            cMap[pPos[0]][pPos[1]] = ' ';
            cMap[x][y] = '@';
            this->setPos(x, y);
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

void Player::giveItem(std::string item, int amount) {
    if (inventory.find(item) != inventory.end()) {
        inventory[item] += amount;
    } else {
        inventory.insert({item, amount});
    }
}

void Player::useItem(std::string item) {
    if (equippedWeapon == item) {
        equippedWeapon = "";
    } else if (item == "Iron_Sword") {
        equippedWeapon = item;
    } else if (item == "Health_Potion") {
        if (m_health != 100) {
            Heal(25);
            delItem(item, 1);
        }
    }
}

void Player::delItem(std::string item, int amount) {
    if (inventory[item] != 0 && item != equippedWeapon) {
        inventory[item] -= amount;
    }
    if (inventory[item] == 0) {
        inventory.erase(item);
    }
}