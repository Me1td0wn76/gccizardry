#include "battle.h"

#include <sstream>
#include <cstdlib>
#include "enemy.h"

Battle::Battle() : active(false), message(L""), enemyName(L""), enemyHP(0) {}

void Battle::enter() {
    active = true;
    int count = getEnemyCount();
    int index = 0;
    if (count > 0) {
        index = std::rand() % count;
    }
    const EnemyData& enemy = getEnemyData(index);
    enemyName = enemy.name;
    enemyHP = enemy.hp;
    message = L"ENEMY APPEARS!";
}

void Battle::exit() {
    active = false;
    message.clear();
    enemyName.clear();
    enemyHP = 0;
}

bool Battle::isActive() const {
    return active;
}

void Battle::handleCommand(Command command) {
    switch (command) {
        case Command::Attack:
            message = L"ATTACK!";
            break;
        case Command::Guard:
            message = L"GUARD!";
            break;
        case Command::Spell:
            message = L"SPELL!";
            break;
        case Command::Item:
            message = L"ITEM!";
            break;
        case Command::Run:
            message = L"RUN AWAY!";
            exit();
            break;
        default:
            break;
    }
}

const std::wstring& Battle::getMessage() const {
    return message;
}

const std::wstring& Battle::getEnemyName() const {
    return enemyName;
}

int Battle::getEnemyHP() const {
    return enemyHP;
}

void Battle::render(Renderer& renderer) const {
    Vec3 enemy[8] = {
        Vec3(-0.8f, 0.0f, 2.4f),
        Vec3(0.8f, 0.0f, 2.4f),
        Vec3(0.8f, 0.0f, 3.8f),
        Vec3(-0.8f, 0.0f, 3.8f),
        Vec3(-0.8f, 1.4f, 2.4f),
        Vec3(0.8f, 1.4f, 2.4f),
        Vec3(0.8f, 1.4f, 3.8f),
        Vec3(-0.8f, 1.4f, 3.8f)
    };

    renderer.drawLine3D(enemy[0], enemy[1]);
    renderer.drawLine3D(enemy[1], enemy[2]);
    renderer.drawLine3D(enemy[2], enemy[3]);
    renderer.drawLine3D(enemy[3], enemy[0]);

    renderer.drawLine3D(enemy[4], enemy[5]);
    renderer.drawLine3D(enemy[5], enemy[6]);
    renderer.drawLine3D(enemy[6], enemy[7]);
    renderer.drawLine3D(enemy[7], enemy[4]);

    renderer.drawLine3D(enemy[0], enemy[4]);
    renderer.drawLine3D(enemy[1], enemy[5]);
    renderer.drawLine3D(enemy[2], enemy[6]);
    renderer.drawLine3D(enemy[3], enemy[7]);
}

void Battle::drawPanel(HDC hdc, const RECT& panelRect) const {
    HFONT font = CreateFontW(
        18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
        FF_DONTCARE, L"Consolas");
    HGDIOBJ oldFont = SelectObject(hdc, font);
    SetTextColor(hdc, RGB(150, 220, 210));
    SetBkMode(hdc, TRANSPARENT);

    std::wostringstream hud;
    if (!enemyName.empty()) {
        hud << L"ENEMY: " << enemyName << L"  HP: " << enemyHP << L"  ";
    }
    hud << L"COMMAND: J Attack  K Guard  L Spell  I Item  R Run";
    if (!message.empty()) {
        hud << L"  |  " << message;
    }

    RECT textRect = panelRect;
    textRect.left += 14;
    textRect.top += 10;
    DrawTextW(hdc, hud.str().c_str(), -1, &textRect, DT_LEFT | DT_TOP | DT_SINGLELINE);

    SelectObject(hdc, oldFont);
    DeleteObject(font);
}
