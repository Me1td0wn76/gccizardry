#include "ground.h"

#include <algorithm>
#include <sstream>

#include "map.h"

namespace {
const wchar_t* MAIN_MENU_ITEMS[] = {
    L"Tavern (Party)",
    L"Inn (Level Up)",
    L"Shop (Weapons/Items)",
    L"Temple (Revive)",
    L"Enter Dungeon"
};
}

Ground::Ground()
    : screen(Screen::MainMenu),
      menuIndex(0),
      rosterIndex(0),
      partyIndex(0),
      createRaceIndex(0),
      createJobIndex(0),
            createBonus(0),
            createRemaining(0),
            createStatIndex(0),
            createPhase(CreatePhase::NameEntry),
            createBaseStats(),
            createAllocated(),
      enterDungeon(false) {
    loadWeapons("src/weapon.txt", weapons);
    loadSpells("src/spell.txt", spells);
        loadRaces("src/race_table.txt", races);
}

void Ground::handleInput(int vkey, bool pressedOnce) {
    if (!pressedOnce) {
        return;
    }

    if (screen == Screen::MainMenu) {
        if (vkey == VK_UP) {
            menuIndex = (menuIndex + 4) % 5;
        } else if (vkey == VK_DOWN) {
            menuIndex = (menuIndex + 1) % 5;
        } else if (vkey == VK_RETURN) {
            switch (menuIndex) {
                case 0: screen = Screen::Tavern; break;
                case 1: screen = Screen::Inn; break;
                case 2: screen = Screen::Shop; break;
                case 3: screen = Screen::Temple; break;
                case 4: enterDungeon = true; break;
                default: break;
            }
        }
        return;
    }

    if (vkey == VK_ESCAPE) {
        screen = Screen::MainMenu;
        statusMessage.clear();
        return;
    }

    if (screen == Screen::Tavern) {
        if (vkey == VK_UP) {
            rosterIndex = std::max(0, rosterIndex - 1);
        } else if (vkey == VK_DOWN) {
            rosterIndex = std::min(static_cast<int>(roster.size()) - 1, rosterIndex + 1);
        } else if (vkey == 'A') {
            addToParty();
        } else if (vkey == 'R') {
            removeFromParty();
        } else if (vkey == 'C') {
            beginCreateCharacter();
        } else if (vkey == 'X') {
            toggleAlive();
        }
        return;
    }

    if (screen == Screen::Inn) {
        if (vkey == 'L') {
            levelUpInn();
        }
        return;
    }

    if (screen == Screen::Shop) {
        return;
    }

    if (screen == Screen::Temple) {
        if (vkey == 'V') {
            reviveTemple();
        }
        return;
    }

    if (screen == Screen::CreateCharacter) {
        if (createPhase == CreatePhase::NameEntry) {
            if (vkey == VK_LEFT) {
                if (!races.empty()) {
                    createRaceIndex = (createRaceIndex + static_cast<int>(races.size()) - 1) % static_cast<int>(races.size());
                }
            } else if (vkey == VK_RIGHT) {
                if (!races.empty()) {
                    createRaceIndex = (createRaceIndex + 1) % static_cast<int>(races.size());
                }
            } else if (vkey == VK_UP) {
                createJobIndex = (createJobIndex + 6) % 7;
            } else if (vkey == VK_DOWN) {
                createJobIndex = (createJobIndex + 1) % 7;
            } else if (vkey == VK_RETURN) {
                if (!inputName.empty()) {
                    createPhase = CreatePhase::AllocateBonus;
                } else {
                    statusMessage = L"Please enter a name.";
                }
            } else if (vkey == VK_BACK) {
                if (!inputName.empty()) {
                    inputName.pop_back();
                }
            }
        } else {
            if (vkey == VK_LEFT) {
                createStatIndex = (createStatIndex + 5) % 6;
            } else if (vkey == VK_RIGHT) {
                createStatIndex = (createStatIndex + 1) % 6;
            } else if (vkey == VK_UP) {
                if (createRemaining > 0) {
                    switch (createStatIndex) {
                        case 0: createAllocated.strength++; break;
                        case 1: createAllocated.dexterity++; break;
                        case 2: createAllocated.constitution++; break;
                        case 3: createAllocated.intelligence++; break;
                        case 4: createAllocated.wisdom++; break;
                        case 5: createAllocated.charisma++; break;
                    }
                    createRemaining--;
                }
            } else if (vkey == VK_DOWN) {
                bool canRemove = false;
                switch (createStatIndex) {
                    case 0: canRemove = createAllocated.strength > 0; break;
                    case 1: canRemove = createAllocated.dexterity > 0; break;
                    case 2: canRemove = createAllocated.constitution > 0; break;
                    case 3: canRemove = createAllocated.intelligence > 0; break;
                    case 4: canRemove = createAllocated.wisdom > 0; break;
                    case 5: canRemove = createAllocated.charisma > 0; break;
                }
                if (canRemove) {
                    switch (createStatIndex) {
                        case 0: createAllocated.strength--; break;
                        case 1: createAllocated.dexterity--; break;
                        case 2: createAllocated.constitution--; break;
                        case 3: createAllocated.intelligence--; break;
                        case 4: createAllocated.wisdom--; break;
                        case 5: createAllocated.charisma--; break;
                    }
                    createRemaining++;
                }
            } else if (vkey == VK_RETURN) {
                if (createRemaining == 0) {
                    finalizeCharacter();
                } else {
                    statusMessage = L"Allocate all bonus points.";
                }
            }
        }
    }
}

void Ground::handleChar(wchar_t ch) {
    if (screen != Screen::CreateCharacter || createPhase != CreatePhase::NameEntry) {
        return;
    }
    if (ch >= 32 && ch <= 126) {
        inputName.push_back(ch);
    }
}

void Ground::update() {
    if (screen == Screen::CreateCharacter && createBonus == 0) {
        createBonus = Character::roll3d6();
        createRemaining = createBonus;
    }
}

bool Ground::shouldEnterDungeon() const {
    return enterDungeon;
}

void Ground::clearEnterDungeon() {
    enterDungeon = false;
}

void Ground::render(HDC hdc, int width, int height) {
    RECT rect{0, 0, width, height};
    HBRUSH bg = CreateSolidBrush(RGB(6, 6, 10));
    FillRect(hdc, &rect, bg);
    DeleteObject(bg);

    HFONT font = CreateFontW(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                             FF_DONTCARE, L"Consolas");
    HGDIOBJ oldFont = SelectObject(hdc, font);
    SetTextColor(hdc, RGB(150, 220, 210));
    SetBkMode(hdc, TRANSPARENT);

    if (screen == Screen::MainMenu) {
        drawHeader(hdc, rect, L"Town" );
        std::vector<std::wstring> items;
        for (const auto& item : MAIN_MENU_ITEMS) {
            items.push_back(item);
        }
        drawMenuList(hdc, 40, 80, items, menuIndex);
    } else if (screen == Screen::Tavern) {
        drawHeader(hdc, rect, L"Tavern" );
        drawRoster(hdc, 40, 80);
        drawParty(hdc, width / 2, 80);
        drawText(hdc, 40, height - 60, L"A: Add  R: Remove  C: Create  X: Toggle Alive  Esc: Back");
    } else if (screen == Screen::Inn) {
        drawHeader(hdc, rect, L"Inn" );
        drawParty(hdc, 40, 80);
        drawText(hdc, 40, height - 60, L"L: Level Up All Party  Esc: Back");
    } else if (screen == Screen::Shop) {
        drawHeader(hdc, rect, L"Shop" );
        drawShopInfo(hdc, 40, 80);
        drawText(hdc, 40, height - 60, L"Esc: Back");
    } else if (screen == Screen::Temple) {
        drawHeader(hdc, rect, L"Temple" );
        drawRoster(hdc, 40, 80);
        drawText(hdc, 40, height - 60, L"V: Revive Selected  Esc: Back");
    } else if (screen == Screen::CreateCharacter) {
        drawHeader(hdc, rect, L"Create Character" );
        drawCreateScreen(hdc, 40, 80);
        drawText(hdc, 40, height - 60, L"Type name, Arrow keys: Race/Job, Enter: Create, Esc: Back");
    }

    if (!statusMessage.empty()) {
        drawText(hdc, 40, height - 90, statusMessage);
    }

    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

void Ground::drawHeader(HDC hdc, const RECT& rect, const std::wstring& title) {
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(40, 60, 70));
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
    Rectangle(hdc, 20, 20, rect.right - 20, 60);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    drawText(hdc, 40, 30, title);
}

void Ground::drawText(HDC hdc, int x, int y, const std::wstring& text) {
    TextOutW(hdc, x, y, text.c_str(), static_cast<int>(text.size()));
}

void Ground::drawMenuList(HDC hdc, int x, int y, const std::vector<std::wstring>& items, int selected) {
    for (size_t i = 0; i < items.size(); i++) {
        std::wostringstream line;
        line << ((static_cast<int>(i) == selected) ? L"> " : L"  ");
        line << items[i];
        drawText(hdc, x, y + static_cast<int>(i) * 24, line.str());
    }
}

void Ground::drawRoster(HDC hdc, int x, int y) {
    drawText(hdc, x, y, L"Roster:");
    for (size_t i = 0; i < roster.size(); i++) {
        const Character& c = roster[i];
        std::wstring line = (static_cast<int>(i) == rosterIndex) ? L"> " : L"  ";
        line += toWide(c.getName());
        line += L"  L" + std::to_wstring(c.getLevel());
        line += L"  " + toWide(Character::jobToString(c.getJob()));
        line += L"  " + toWide(c.getRaceName());
        line += c.isAlive() ? L"" : L"  (DEAD)";
        drawText(hdc, x, y + 24 + static_cast<int>(i) * 22, line);
    }
    if (roster.empty()) {
        drawText(hdc, x, y + 24, L"(No characters)  Press C to create");
    }
}

void Ground::drawParty(HDC hdc, int x, int y) {
    drawText(hdc, x, y, L"Party (max 6):");
    for (size_t i = 0; i < party.size(); i++) {
        int idx = party[i];
        if (idx < 0 || idx >= static_cast<int>(roster.size())) {
            continue;
        }
        const Character& c = roster[idx];
        std::wstring line = (static_cast<int>(i) == partyIndex) ? L"> " : L"  ";
        line += toWide(c.getName());
        line += L"  L" + std::to_wstring(c.getLevel());
        line += L"  " + toWide(Character::jobToString(c.getJob()));
        line += L"  " + toWide(c.getRaceName());
        drawText(hdc, x, y + 24 + static_cast<int>(i) * 22, line);
    }
    if (party.empty()) {
        drawText(hdc, x, y + 24, L"(No party members)" );
    }
}

void Ground::drawShopInfo(HDC hdc, int x, int y) {
    std::wostringstream info;
    info << L"Weapons loaded: " << weapons.size();
    drawText(hdc, x, y, info.str());
    std::wostringstream info2;
    info2 << L"Spells loaded: " << spells.size();
    drawText(hdc, x, y + 24, info2.str());
    drawText(hdc, x, y + 60, L"(Use weapon.txt / spell.txt to extend items)" );
}

void Ground::drawCreateScreen(HDC hdc, int x, int y) {
    drawText(hdc, x, y, L"Name: " + inputName + L"_");

    const RaceEntry* raceEntry = getSelectedRaceEntry();
    std::wstring race = raceEntry ? toWide(raceEntry->name) : L"Unknown";
    std::wstring job = toWide(Character::jobToString(getSelectedJob()));

    drawText(hdc, x, y + 30, L"Race: " + race + L"  (Left/Right)");
    drawText(hdc, x, y + 60, L"Job: " + job + L"  (Up/Down)");
    drawText(hdc, x, y + 90, L"Bonus Pool (3d6): " + std::to_wstring(createBonus));

    Character::Stats raceStats{};
    if (raceEntry) {
        raceStats.strength = raceEntry->str;
        raceStats.intelligence = raceEntry->intl;
        raceStats.dexterity = raceEntry->dex;
        raceStats.constitution = raceEntry->vit;
        raceStats.wisdom = raceEntry->agi;
        raceStats.charisma = raceEntry->luk;
    }

    Character::Stats total = raceStats;
    total.strength += createAllocated.strength;
    total.dexterity += createAllocated.dexterity;
    total.constitution += createAllocated.constitution;
    total.intelligence += createAllocated.intelligence;
    total.wisdom += createAllocated.wisdom;
    total.charisma += createAllocated.charisma;

    drawText(hdc, x, y + 120, L"Race : " + statLine(raceStats));
    drawText(hdc, x, y + 150, L"Bonus: " + statLine(createAllocated));
    drawText(hdc, x, y + 180, L"Total: " + statLine(total));

    if (createPhase == CreatePhase::AllocateBonus) {
        std::wstring selector = L"Select stat (Left/Right), Up/Down to add/remove. Remaining: " + std::to_wstring(createRemaining);
        drawText(hdc, x, y + 240, selector);
    } else {
        drawText(hdc, x, y + 240, L"Press Enter to allocate bonus points.");
    }
}

void Ground::addToParty() {
    if (roster.empty() || party.size() >= 6) {
        return;
    }
    if (rosterIndex < 0 || rosterIndex >= static_cast<int>(roster.size())) {
        return;
    }
    int idx = rosterIndex;
    if (std::find(party.begin(), party.end(), idx) == party.end()) {
        party.push_back(idx);
    }
}

void Ground::removeFromParty() {
    if (party.empty()) {
        return;
    }
    if (partyIndex < 0 || partyIndex >= static_cast<int>(party.size())) {
        partyIndex = 0;
    }
    party.erase(party.begin() + partyIndex);
    partyIndex = std::max(0, partyIndex - 1);
}

void Ground::toggleAlive() {
    if (roster.empty()) {
        return;
    }
    Character& c = roster[rosterIndex];
    c.setAlive(!c.isAlive());
    if (!c.isAlive()) {
        c.setHP(0);
        statusMessage = L"Character marked dead.";
    } else {
        c.setHP(1);
        statusMessage = L"Character revived.";
    }
}

void Ground::levelUpInn() {
    if (party.empty()) {
        statusMessage = L"No party members.";
        return;
    }
    for (int idx : party) {
        if (idx >= 0 && idx < static_cast<int>(roster.size())) {
            roster[idx].levelUp();
        }
    }
    statusMessage = L"Party leveled up.";
}

void Ground::reviveTemple() {
    if (roster.empty()) {
        statusMessage = L"No characters.";
        return;
    }
    Character& c = roster[rosterIndex];
    if (c.isAlive()) {
        statusMessage = L"Already alive.";
        return;
    }
    c.setAlive(true);
    c.setHP(1);
    statusMessage = L"Revived by temple.";
}

void Ground::beginCreateCharacter() {
    screen = Screen::CreateCharacter;
    inputName.clear();
    createRaceIndex = 0;
    createJobIndex = 0;
    createBonus = Character::roll3d6();
    createRemaining = createBonus;
    createStatIndex = 0;
    createPhase = CreatePhase::NameEntry;
    createBaseStats = Character::Stats{};
    createAllocated = Character::Stats{};
    statusMessage.clear();
}

void Ground::finalizeCharacter() {
    if (inputName.empty()) {
        statusMessage = L"Please enter a name.";
        return;
    }
    Character character;
    const RaceEntry* raceEntry = getSelectedRaceEntry();
    if (!raceEntry) {
        statusMessage = L"Race data missing.";
        return;
    }
    Character::Stats total{};
    total.strength = raceEntry->str;
    total.intelligence = raceEntry->intl;
    total.dexterity = raceEntry->dex;
    total.constitution = raceEntry->vit;
    total.wisdom = raceEntry->agi;
    total.charisma = raceEntry->luk;
    total.strength += createAllocated.strength;
    total.dexterity += createAllocated.dexterity;
    total.constitution += createAllocated.constitution;
    total.intelligence += createAllocated.intelligence;
    total.wisdom += createAllocated.wisdom;
    total.charisma += createAllocated.charisma;
    character.initialize(std::string(inputName.begin(), inputName.end()), raceEntry->name, getSelectedJob(), total);
    roster.push_back(character);
    rosterIndex = static_cast<int>(roster.size()) - 1;
    screen = Screen::Tavern;
    statusMessage = L"Character created.";
}

Character::Job Ground::getSelectedJob() const {
    return static_cast<Character::Job>(createJobIndex);
}

const RaceEntry* Ground::getSelectedRaceEntry() const {
    if (races.empty()) {
        return nullptr;
    }
    int idx = std::clamp(createRaceIndex, 0, static_cast<int>(races.size()) - 1);
    return &races[idx];
}

std::wstring Ground::toWide(const std::string& value) const {
    return std::wstring(value.begin(), value.end());
}

std::wstring Ground::statLine(const Character::Stats& stats) const {
    std::wostringstream line;
    line << L"STR " << stats.strength
         << L"  DEX " << stats.dexterity
         << L"  CON " << stats.constitution
         << L"  INT " << stats.intelligence
         << L"  WIS " << stats.wisdom
         << L"  CHA " << stats.charisma;
    return line.str();
}

std::vector<Character> Ground::getPartyMembers() const {
    std::vector<Character> members;
    for (int idx : party) {
        if (idx >= 0 && idx < static_cast<int>(roster.size())) {
            members.push_back(roster[idx]);
        }
    }
    return members;
}
