#include "enemy.h"

static const EnemyData ENEMIES[] = {
    {L"SLIME", 8, 2, 1},
    {L"RED SLIME", 10, 3, 1},
    {L"BLUE SLIME", 12, 3, 2},
    {L"GOBLIN", 14, 4, 2},
    {L"GOBLIN SCOUT", 12, 5, 2},
    {L"GOBLIN CHIEF", 18, 6, 3},
    {L"WOLF", 16, 5, 2},
    {L"DIRE WOLF", 22, 7, 3},
    {L"BAT", 10, 3, 1},
    {L"GIANT BAT", 18, 5, 2},
    {L"SKELETON", 20, 6, 4},
    {L"SKELETON WARRIOR", 26, 8, 5},
    {L"ZOMBIE", 24, 6, 4},
    {L"GHOUL", 28, 7, 4},
    {L"BANDIT", 18, 6, 2},
    {L"ROGUE", 22, 7, 3},
    {L"ORC", 30, 9, 5},
    {L"ORC WARRIOR", 34, 11, 6},
    {L"TROLL", 45, 12, 7},
    {L"OGRE", 50, 13, 7},
    {L"LIZARDMAN", 26, 8, 4},
    {L"LIZARD CHIEF", 32, 10, 5},
    {L"GIANT SPIDER", 20, 7, 3},
    {L"POISON SPIDER", 24, 8, 3},
    {L"SCORPION", 28, 9, 4},
    {L"ARMORED SCORPION", 34, 10, 6},
    {L"GHOST", 22, 7, 2},
    {L"SPECTER", 28, 9, 3},
    {L"WRAITH", 36, 11, 5},
    {L"WISP", 16, 6, 1},
    {L"HARPY", 30, 9, 4},
    {L"MINOTAUR", 42, 12, 6},
    {L"WYVERN", 48, 13, 6},
    {L"LESSER DEMON", 40, 12, 7},
    {L"IMP", 18, 6, 2},
    {L"SAND WORM", 38, 11, 5},
    {L"CAVE BEAR", 36, 10, 6},
    {L"STONE GOLEM", 55, 12, 10},
    {L"IRON GOLEM", 70, 14, 12},
    {L"DARK KNIGHT", 60, 15, 10},
    {L"DARK MAGE", 35, 16, 4},
    {L"FIRE ELEMENTAL", 45, 14, 8},
    {L"ICE ELEMENTAL", 45, 14, 8},
    {L"THUNDER ELEMENTAL", 48, 15, 9},
    {L"HYDRA", 80, 18, 12},
    {L"DRAGON WHELP", 55, 16, 9},
    {L"YOUNG DRAGON", 70, 18, 11},
    {L"ELDER DRAGON", 95, 22, 14},
    {L"LICH", 85, 20, 12},
    {L"ANCIENT LICH", 110, 24, 15}
};

int getEnemyCount() {
    return static_cast<int>(sizeof(ENEMIES) / sizeof(ENEMIES[0]));
}

const EnemyData& getEnemyData(int index) {
    if (index < 0) {
        index = 0;
    }
    int count = getEnemyCount();
    if (index >= count) {
        index = count - 1;
    }
    return ENEMIES[index];
}
