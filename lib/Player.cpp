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

std::string Player::getName() {
    return m_name;
}

int Player::getHealth() {
    return m_health;
}

int Player::getGold() {
    return m_gold;
}

int* Player::getPos() {
    return pos;
}

void Player::setPos(int x, int y) {
    pos[0] = x;
    pos[1] = y;
}

std::map<std::string, int> Player::getInv() {
    return inventory;
}

void Player::giveItem(std::string item, int amount) {
    if (inventory.find(item) != inventory.end()) {
        inventory[item] += amount;
    } else {
        inventory.insert({item, amount});
    }
}