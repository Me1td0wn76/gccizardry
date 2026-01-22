#pragma once

#include <string>
#include <windows.h>

#include "Renderer.h"

class Battle {
public:
    enum class Command {
        None,
        Attack,
        Guard,
        Spell,
        Item,
        Run
    };

    Battle();

    void enter();
    void exit();
    bool isActive() const;

    void handleCommand(Command command);
    const std::wstring& getMessage() const;
    const std::wstring& getEnemyName() const;
    int getEnemyHP() const;

    void render(Renderer& renderer) const;
    void drawPanel(HDC hdc, const RECT& panelRect) const;

private:
    bool active;
    std::wstring message;
    std::wstring enemyName;
    int enemyHP;
};
