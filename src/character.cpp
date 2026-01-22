#include "character.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <random>
#include <sstream>

namespace {
int rollDice(int count, int sides) {
    static std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> dist(1, sides);
    int total = 0;
    for (int i = 0; i < count; i++) {
        total += dist(rng);
    }
    return total;
}

std::string trim(const std::string& value) {
    size_t start = 0;
    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start]))) {
        start++;
    }
    size_t end = value.size();
    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        end--;
    }
    return value.substr(start, end - start);
}

bool parseEntryLine(const std::string& line, std::string& outName, std::vector<std::string>& outFields) {
    auto pos = line.find(':');
    if (pos == std::string::npos) {
        return false;
    }
    outName = trim(line.substr(0, pos));
    std::string rest = line.substr(pos + 1);
    if (!rest.empty() && rest.back() == ',') {
        rest.pop_back();
    }
    std::stringstream ss(rest);
    std::string item;
    outFields.clear();
    while (std::getline(ss, item, '/')) {
        outFields.push_back(trim(item));
    }
    return !outName.empty();
}
}

bool loadWeapons(const std::string& path, std::vector<WeaponEntry>& out) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    out.clear();

    std::string group;
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) {
            continue;
        }
        if (line.front() == '(') {
            continue;
        }
        if (line.back() == '{') {
            group = trim(line.substr(0, line.size() - 1));
            continue;
        }
        if (line.front() == '}') {
            group.clear();
            continue;
        }

        std::string name;
        std::vector<std::string> fields;
        if (parseEntryLine(line, name, fields)) {
            out.push_back({group, name, fields});
        }
    }
    return true;
}

bool loadSpells(const std::string& path, std::vector<SpellEntry>& out) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    out.clear();

    std::string group;
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) {
            continue;
        }
        if (line.front() == '(') {
            continue;
        }
        if (line.back() == '{') {
            group = trim(line.substr(0, line.size() - 1));
            continue;
        }
        if (line.front() == '}') {
            group.clear();
            continue;
        }

        std::string name;
        std::vector<std::string> fields;
        if (parseEntryLine(line, name, fields)) {
            out.push_back({group, name, fields});
        }
    }
    return true;
}

bool loadRaces(const std::string& path, std::vector<RaceEntry>& out) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    out.clear();

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) {
            continue;
        }
        if (line.front() == '{' || line.front() == '}' || line.front() == '(') {
            continue;
        }

        std::string name;
        std::vector<std::string> fields;
        if (parseEntryLine(line, name, fields) && fields.size() >= 6) {
            RaceEntry entry;
            entry.name = name;
            entry.str = std::stoi(fields[0]);
            entry.intl = std::stoi(fields[1]);
            entry.dex = std::stoi(fields[2]);
            entry.vit = std::stoi(fields[3]);
            entry.agi = std::stoi(fields[4]);
            entry.luk = std::stoi(fields[5]);
            out.push_back(entry);
        }
    }

    return !out.empty();
}

Character::Character()
    : job(Job::Warrior),
      level(1),
      exp(0),
      hp(10),
      alive(true) {}

int Character::roll3d6() {
    return rollDice(3, 6);
}

Character::Stats Character::rollBaseStats() {
    Stats stats;
    stats.strength = rollDice(3, 6);
    stats.dexterity = rollDice(3, 6);
    stats.constitution = rollDice(3, 6);
    stats.intelligence = rollDice(3, 6);
    stats.wisdom = rollDice(3, 6);
    stats.charisma = rollDice(3, 6);
    return stats;
}

Character::Stats Character::applyJobBonus(const Stats& base, Job jobValue, int bonus) {
    Stats stats = base;
    switch (jobValue) {
        case Job::Warrior:
        case Job::Knight:
        case Job::Samurai:
            stats.strength += bonus;
            break;
        case Job::Thief:
            stats.dexterity += bonus;
            break;
        case Job::Mage:
            stats.intelligence += bonus;
            break;
        case Job::Cleric:
            stats.wisdom += bonus;
            break;
        case Job::MagicFighter:
            stats.strength += bonus / 2;
            stats.intelligence += bonus - bonus / 2;
            break;
    }
    return stats;
}

Character::Stats Character::applyRaceBonus(const Stats& base, const RaceEntry& race) {
    Stats stats = base;
    stats.strength += race.str;
    stats.intelligence += race.intl;
    stats.dexterity += race.dex;
    stats.constitution += race.vit;
    stats.wisdom += race.agi;
    stats.charisma += race.luk;
    return stats;
}

void Character::initialize(const std::string& nameValue, const std::string& raceValue, Job jobValue, const Stats& statsValue) {
    name = nameValue;
    raceName = raceValue;
    job = jobValue;
    stats = statsValue;
    level = 1;
    exp = 0;
    hp = 8 + stats.constitution / 2;
    alive = true;
}

void Character::levelUp() {
    level += 1;
    hp += std::max(1, stats.constitution / 3 + 2);
}

std::string Character::jobToString(Job jobValue) {
    switch (jobValue) {
        case Job::Warrior: return "Warrior";
        case Job::Knight: return "Knight";
        case Job::Mage: return "Mage";
        case Job::Cleric: return "Cleric";
        case Job::Thief: return "Thief";
        case Job::Samurai: return "Samurai";
        case Job::MagicFighter: return "MagicFighter";
    }
    return "Warrior";
}
