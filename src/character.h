#pragma once

#include <string>
#include <vector>

struct WeaponEntry {
    std::string group;
    std::string name;
    std::vector<std::string> fields;
};

struct SpellEntry {
    std::string group;
    std::string name;
    std::vector<std::string> fields;
};

struct RaceEntry {
    std::string name;
    int str = 0;
    int intl = 0;
    int dex = 0;
    int vit = 0;
    int agi = 0;
    int luk = 0;
};

bool loadWeapons(const std::string& path, std::vector<WeaponEntry>& out);
bool loadSpells(const std::string& path, std::vector<SpellEntry>& out);
bool loadRaces(const std::string& path, std::vector<RaceEntry>& out);

class Character {
public:
    enum class Job {
        Warrior,
        Knight,
        Mage,
        Cleric,
        Thief,
        Samurai,
        MagicFighter
    };

    struct Stats {
        int strength = 0;
        int dexterity = 0;
        int constitution = 0;
        int intelligence = 0;
        int wisdom = 0;
        int charisma = 0;
    };

    Character();

    static int roll3d6();
    static Stats rollBaseStats();
    static Stats applyJobBonus(const Stats& base, Job job, int bonus);
    static Stats applyRaceBonus(const Stats& base, const RaceEntry& race);

    void initialize(const std::string& name, const std::string& raceName, Job job, const Stats& stats);

    void levelUp();

    const std::string& getName() const { return name; }
    const std::string& getRaceName() const { return raceName; }
    Job getJob() const { return job; }
    const Stats& getStats() const { return stats; }
    int getLevel() const { return level; }
    int getHP() const { return hp; }
    bool isAlive() const { return alive; }

    void setAlive(bool value) { alive = value; }
    void setHP(int value) { hp = value; }

    static std::string jobToString(Job job);

private:
    std::string name;
    std::string raceName;
    Job job;
    Stats stats;
    int level;
    int exp;
    int hp;
    bool alive;
};
