#pragma once

#include <string>
#include <vector>
#include <windows.h>

#include "character.h"

class Ground {
public:
    enum class Screen {
        MainMenu,
        Tavern,
        Inn,
        Shop,
        Temple,
        CreateCharacter
    };

    Ground();

    void handleInput(int vkey, bool pressedOnce);
    void handleChar(wchar_t ch);
    void update();
    void render(HDC hdc, int width, int height);

    bool shouldEnterDungeon() const;
    void clearEnterDungeon();
    std::vector<Character> getPartyMembers() const;

private:
    enum class CreatePhase {
        NameEntry,
        AllocateBonus
    };

    Screen screen;
    int menuIndex;
    int rosterIndex;
    int partyIndex;

    std::vector<Character> roster;
    std::vector<int> party;

    int createRaceIndex;
    int createJobIndex;
    int createBonus;
    int createRemaining;
    int createStatIndex;
    CreatePhase createPhase;
    Character::Stats createBaseStats;
    Character::Stats createAllocated;
    std::wstring inputName;
    std::wstring statusMessage;

    bool enterDungeon;

    std::vector<WeaponEntry> weapons;
    std::vector<SpellEntry> spells;
    std::vector<RaceEntry> races;

    void drawHeader(HDC hdc, const RECT& rect, const std::wstring& title);
    void drawText(HDC hdc, int x, int y, const std::wstring& text);
    void drawMenuList(HDC hdc, int x, int y, const std::vector<std::wstring>& items, int selected);
    void drawRoster(HDC hdc, int x, int y);
    void drawParty(HDC hdc, int x, int y);
    void drawShopInfo(HDC hdc, int x, int y);
    void drawCreateScreen(HDC hdc, int x, int y);

    void addToParty();
    void removeFromParty();
    void toggleAlive();
    void levelUpInn();
    void reviveTemple();

    void beginCreateCharacter();
    void finalizeCharacter();

    Character::Job getSelectedJob() const;
    const RaceEntry* getSelectedRaceEntry() const;
    std::wstring toWide(const std::string& value) const;
    std::wstring statLine(const Character::Stats& stats) const;
};
