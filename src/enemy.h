#pragma once

struct EnemyData {
    const wchar_t* name;
    int hp;
    int attack;
    int defense;
};

int getEnemyCount();
const EnemyData& getEnemyData(int index);
