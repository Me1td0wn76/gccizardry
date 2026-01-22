#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>
#include <cstdlib>
#include <vector>

#include "Vec3.h"
#include "Matrix4.h"
#include "Renderer.h"
#include "Dungeon.h"
#include "Player.h"
#include "battle.h"
#include "map.h"
#include "ground.h"

namespace {
constexpr int INITIAL_WIDTH = 1024;
constexpr int INITIAL_HEIGHT = 768;
constexpr wchar_t WINDOW_CLASS_NAME[] = L"DRPGWireframeWindow";

Renderer gRenderer(INITIAL_WIDTH, INITIAL_HEIGHT);
Dungeon gDungeon;
Player gPlayer(1, 1);
bool gRunning = true;
Battle gBattle;
std::wstring gStatusMessage;
Ground gGround;
bool gInGround = true;
std::vector<Character> gActiveParty;
}

// ダンジョンのワイヤーフレームを描画
void renderWallCube(Renderer& renderer, const Matrix4& viewMatrix, int gx, int gz) {
    Vec3 vertices[8] = {
        Vec3(gx,     0, gz),
        Vec3(gx + 1, 0, gz),
        Vec3(gx + 1, 0, gz + 1),
        Vec3(gx,     0, gz + 1),
        Vec3(gx,     1, gz),
        Vec3(gx + 1, 1, gz),
        Vec3(gx + 1, 1, gz + 1),
        Vec3(gx,     1, gz + 1)
    };

    for (int i = 0; i < 8; i++) {
        vertices[i] = viewMatrix.transform(vertices[i]);
    }

    renderer.drawLine3D(vertices[0], vertices[1]);
    renderer.drawLine3D(vertices[1], vertices[2]);
    renderer.drawLine3D(vertices[2], vertices[3]);
    renderer.drawLine3D(vertices[3], vertices[0]);

    renderer.drawLine3D(vertices[4], vertices[5]);
    renderer.drawLine3D(vertices[5], vertices[6]);
    renderer.drawLine3D(vertices[6], vertices[7]);
    renderer.drawLine3D(vertices[7], vertices[4]);

    renderer.drawLine3D(vertices[0], vertices[4]);
    renderer.drawLine3D(vertices[1], vertices[5]);
    renderer.drawLine3D(vertices[2], vertices[6]);
    renderer.drawLine3D(vertices[3], vertices[7]);
}

void renderDoor(Renderer& renderer, const Matrix4& viewMatrix, int gx, int gz, bool open) {
    if (open) {
        Vec3 frame[8] = {
            Vec3(gx + 0.25f, 0, gz + 0.45f),
            Vec3(gx + 0.75f, 0, gz + 0.45f),
            Vec3(gx + 0.75f, 0, gz + 0.55f),
            Vec3(gx + 0.25f, 0, gz + 0.55f),
            Vec3(gx + 0.25f, 1, gz + 0.45f),
            Vec3(gx + 0.75f, 1, gz + 0.45f),
            Vec3(gx + 0.75f, 1, gz + 0.55f),
            Vec3(gx + 0.25f, 1, gz + 0.55f)
        };
        for (int i = 0; i < 8; i++) {
            frame[i] = viewMatrix.transform(frame[i]);
        }
        renderer.drawLine3D(frame[0], frame[1]);
        renderer.drawLine3D(frame[1], frame[2]);
        renderer.drawLine3D(frame[2], frame[3]);
        renderer.drawLine3D(frame[3], frame[0]);
        renderer.drawLine3D(frame[4], frame[5]);
        renderer.drawLine3D(frame[5], frame[6]);
        renderer.drawLine3D(frame[6], frame[7]);
        renderer.drawLine3D(frame[7], frame[4]);
        renderer.drawLine3D(frame[0], frame[4]);
        renderer.drawLine3D(frame[1], frame[5]);
        renderer.drawLine3D(frame[2], frame[6]);
        renderer.drawLine3D(frame[3], frame[7]);
        return;
    }

    Vec3 slab[4] = {
        Vec3(gx, 0, gz + 0.5f),
        Vec3(gx + 1, 0, gz + 0.5f),
        Vec3(gx + 1, 1, gz + 0.5f),
        Vec3(gx, 1, gz + 0.5f)
    };
    for (int i = 0; i < 4; i++) {
        slab[i] = viewMatrix.transform(slab[i]);
    }
    renderer.drawLine3D(slab[0], slab[1]);
    renderer.drawLine3D(slab[1], slab[2]);
    renderer.drawLine3D(slab[2], slab[3]);
    renderer.drawLine3D(slab[3], slab[0]);
}

void renderDungeon(Renderer& renderer, const Dungeon& dungeon, const Player& player) {
    Matrix4 viewMatrix = player.getViewMatrix();
    
    for (int gz = 0; gz < dungeon.getHeight(); gz++) {
        for (int gx = 0; gx < dungeon.getWidth(); gx++) {
            if (dungeon.isDoor(gx, gz)) {
                renderDoor(renderer, viewMatrix, gx, gz, dungeon.isDoorOpen(gx, gz));
                continue;
            }

            if (dungeon.isWall(gx, gz)) {
                renderWallCube(renderer, viewMatrix, gx, gz);
            }
        }
    }
}

void updateWindowTitle(HWND hwnd) {
    std::wostringstream stream;
    stream << std::fixed << std::setprecision(2);
    if (gInGround) {
        stream << L"DRPG Town";
    } else if (gBattle.isActive()) {
        stream << L"DRPG Battle | Commands: J/K/L/I/R";
    } else {
        stream << L"DRPG Wireframe | F" << gDungeon.getFloor() + 1;
        stream << L" | Grid (" << gPlayer.getGridX() << L", " << gPlayer.getGridZ() << L")";
        stream << L" | Dir " << gPlayer.getDirection();
        stream << L" | W/S move, A/D turn, E door, U/N stairs, B battle, Esc quit";
    }

    SetWindowTextW(hwnd, stream.str().c_str());
}

std::wstring toWide(const std::string& value) {
    return std::wstring(value.begin(), value.end());
}

bool isKeyPressedOnce(int vkey) {
    static bool previous[256] = {};
    bool pressed = (GetAsyncKeyState(vkey) & 0x8000) != 0;
    bool fired = pressed && !previous[vkey];
    previous[vkey] = pressed;
    return fired;
}

void handleGroundInput() {
    int keys[] = {
        VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, VK_RETURN, VK_ESCAPE, VK_BACK,
        'A', 'B', 'C', 'D', 'E', 'L', 'R', 'V', 'X'
    };
    for (int vkey : keys) {
        if (isKeyPressedOnce(vkey)) {
            gGround.handleInput(vkey, true);
        }
    }
}

void handleTileEffects() {
    int x = gPlayer.getGridX();
    int z = gPlayer.getGridZ();

    if (gDungeon.isWarp(x, z)) {
        int outFloor = gDungeon.getFloor();
        int outX = x;
        int outZ = z;
        if (Map::tryWarp(gDungeon.getFloor(), x, z, outFloor, outX, outZ)) {
            gDungeon.setFloor(outFloor);
            gPlayer.setGridPosition(outX, outZ);
            gStatusMessage = L"WARP!";
        } else {
            gStatusMessage = L"WARP FAILED";
        }
        return;
    }

    if (gDungeon.isFall(x, z)) {
        if (gDungeon.canGoDown()) {
            gDungeon.setFloor(gDungeon.getFloor() + 1);
            gStatusMessage = L"FALL DOWN";
        } else {
            gStatusMessage = L"NO LOWER FLOOR";
        }
        return;
    }

    if (gDungeon.isTrap(x, z)) {
        gStatusMessage = L"TRAP!";
        gBattle.enter();
    }
}

void processInput() {
    if (gInGround) {
        return;
    }

    if (gBattle.isActive()) {
        if (isKeyPressedOnce('J')) {
            gBattle.handleCommand(Battle::Command::Attack);
        }
        if (isKeyPressedOnce('K')) {
            gBattle.handleCommand(Battle::Command::Guard);
        }
        if (isKeyPressedOnce('L')) {
            gBattle.handleCommand(Battle::Command::Spell);
        }
        if (isKeyPressedOnce('I')) {
            gBattle.handleCommand(Battle::Command::Item);
        }
        if (isKeyPressedOnce('R')) {
            gBattle.handleCommand(Battle::Command::Run);
        }
        if (isKeyPressedOnce(VK_ESCAPE) || isKeyPressedOnce('Q')) {
            PostQuitMessage(0);
        }
        return;
    }

    if (isKeyPressedOnce('B')) {
        gBattle.enter();
        return;
    }

    if (isKeyPressedOnce('W')) {
        if (gPlayer.moveForward(gDungeon)) {
            handleTileEffects();
            if ((std::rand() % 100) < 20) {
                gBattle.enter();
            }
        }
    }
    if (isKeyPressedOnce('S')) {
        if (gPlayer.moveBackward(gDungeon)) {
            handleTileEffects();
            if ((std::rand() % 100) < 20) {
                gBattle.enter();
            }
        }
    }
    if (isKeyPressedOnce('A')) {
        gPlayer.turnLeft();
    }
    if (isKeyPressedOnce('D')) {
        gPlayer.turnRight();
    }
    if (isKeyPressedOnce('E')) {
        int dx = 0;
        int dz = 0;
        gPlayer.getForwardDelta(dx, dz);
        int targetX = gPlayer.getGridX() + dx;
        int targetZ = gPlayer.getGridZ() + dz;
        if (gDungeon.isDoor(targetX, targetZ)) {
            gDungeon.toggleDoor(targetX, targetZ);
        }
    }
    if (isKeyPressedOnce('U')) {
        if (gDungeon.isStairs(gPlayer.getGridX(), gPlayer.getGridZ()) && gDungeon.canGoUp()) {
            gDungeon.setFloor(gDungeon.getFloor() - 1);
            gStatusMessage = L"UP FLOOR";
        }
    }
    if (isKeyPressedOnce('N')) {
        if (gDungeon.isStairs(gPlayer.getGridX(), gPlayer.getGridZ()) && gDungeon.canGoDown()) {
            gDungeon.setFloor(gDungeon.getFloor() + 1);
            gStatusMessage = L"DOWN FLOOR";
        }
    }
    if (gDungeon.isStairs(gPlayer.getGridX(), gPlayer.getGridZ())) {
        if (!gDungeon.canGoUp() && gDungeon.canGoDown()) {
            gDungeon.setFloor(gDungeon.getFloor() + 1);
            gStatusMessage = L"DOWN FLOOR";
        } else if (gDungeon.canGoUp() && !gDungeon.canGoDown()) {
            gDungeon.setFloor(gDungeon.getFloor() - 1);
            gStatusMessage = L"UP FLOOR";
        }
    }
    if (isKeyPressedOnce(VK_ESCAPE) || isKeyPressedOnce('Q')) {
        PostQuitMessage(0);
    }
}

void updateViewportLayout(int width, int height) {
    int margin = 16;
    int viewHeight = static_cast<int>(height * 0.68f);
    int viewWidth = width - margin * 2;
    gRenderer.setViewportRect(margin, margin, viewWidth, viewHeight - margin);
}

void drawWizardryUI(HDC hdc, int width, int height) {
    int margin = 16;
    int viewHeight = static_cast<int>(height * 0.68f);
    RECT panelRect{margin, viewHeight, width - margin, height - margin};
    HBRUSH panelBrush = CreateSolidBrush(RGB(12, 12, 18));
    FillRect(hdc, &panelRect, panelBrush);
    DeleteObject(panelBrush);

    HPEN panelBorder = CreatePen(PS_SOLID, 1, RGB(40, 60, 70));
    HGDIOBJ oldPen = SelectObject(hdc, panelBorder);
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
    Rectangle(hdc, panelRect.left, panelRect.top, panelRect.right, panelRect.bottom);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(panelBorder);

    HFONT font = CreateFontW(
        18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
        FF_DONTCARE, L"Consolas");
    HGDIOBJ oldFont = SelectObject(hdc, font);
    SetTextColor(hdc, RGB(150, 220, 210));
    SetBkMode(hdc, TRANSPARENT);

    std::wostringstream hud;
    if (gBattle.isActive()) {
        hud << L"ENEMY: " << gBattle.getEnemyName() << L"  HP: " << gBattle.getEnemyHP();
        hud << L"  ";
        if (!gBattle.getMessage().empty()) {
            hud << gBattle.getMessage() << L"  ";
        }
        hud << L"J Attack  K Guard  L Spell  I Item  R Run";
    } else {
        hud << L"FLOOR: " << (gDungeon.getFloor() + 1);
        hud << L"  GRID: [" << gPlayer.getGridX() << L"," << gPlayer.getGridZ() << L"]";
        hud << L"  DIR: " << gPlayer.getDirection();
        hud << L"  W/S Move  A/D Turn  E Door  U/N Stairs  B Battle  Esc Quit";
        if (gDungeon.isWarp(gPlayer.getGridX(), gPlayer.getGridZ())) {
            hud << L"  WARP";
        } else if (gDungeon.isFall(gPlayer.getGridX(), gPlayer.getGridZ())) {
            hud << L"  FALL";
        } else if (gDungeon.isTrap(gPlayer.getGridX(), gPlayer.getGridZ())) {
            hud << L"  TRAP";
        } else if (gDungeon.isStairs(gPlayer.getGridX(), gPlayer.getGridZ())) {
            hud << L"  STAIRS";
        }
        if (!gStatusMessage.empty()) {
            hud << L"  | " << gStatusMessage;
        }
    }

    RECT textRect = panelRect;
    textRect.left += 14;
    textRect.top += 10;
    DrawTextW(hdc, hud.str().c_str(), -1, &textRect, DT_LEFT | DT_TOP | DT_SINGLELINE);

    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

void drawBattleUI(HDC hdc, int width, int height) {
    RECT rect{0, 0, width, height};
    HBRUSH bg = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(hdc, &rect, bg);
    DeleteObject(bg);

    HPEN border = CreatePen(PS_SOLID, 1, RGB(40, 60, 70));
    HGDIOBJ oldPen = SelectObject(hdc, border);
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));

    RECT top{20, 20, width - 20, 110};
    RECT mid{20, 120, width - 20, 200};
    RECT bottom{20, 210, width - 20, height - 20};

    Rectangle(hdc, top.left, top.top, top.right, top.bottom);
    Rectangle(hdc, mid.left, mid.top, mid.right, mid.bottom);
    Rectangle(hdc, bottom.left, bottom.top, bottom.right, bottom.bottom);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(border);

    HFONT font = CreateFontW(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                             FF_DONTCARE, L"Consolas");
    HGDIOBJ oldFont = SelectObject(hdc, font);
    SetTextColor(hdc, RGB(150, 220, 210));
    SetBkMode(hdc, TRANSPARENT);

    std::wostringstream enemies;
    enemies << L"1> " << gBattle.getEnemyName() << L" (1)";
    TextOutW(hdc, top.left + 10, top.top + 10, enemies.str().c_str(), static_cast<int>(enemies.str().size()));

    std::wstring message = gBattle.getMessage();
    if (message.empty()) {
        message = L"ENCOUNTER!";
    }
    TextOutW(hdc, mid.left + 10, mid.top + 10, message.c_str(), static_cast<int>(message.size()));

    TextOutW(hdc, bottom.left + 10, bottom.top + 10, L"#  NAME        CLASS   LV  HP", 32);
    if (gActiveParty.empty()) {
        TextOutW(hdc, bottom.left + 10, bottom.top + 34, L"(No party members)", 21);
    } else {
        for (size_t i = 0; i < gActiveParty.size() && i < 6; i++) {
            const Character& c = gActiveParty[i];
            std::wostringstream line;
            line << (i + 1) << L"  "
                 << toWide(c.getName()) << L"  "
                 << toWide(Character::jobToString(c.getJob())) << L"  "
                 << c.getLevel() << L"  " << c.getHP();
            TextOutW(hdc, bottom.left + 10, bottom.top + 34 + static_cast<int>(i) * 20,
                     line.str().c_str(), static_cast<int>(line.str().size()));
        }
    }

    std::wstring cmds = L"J:Attack  K:Guard  L:Spell  I:Item  R:Run";
    TextOutW(hdc, bottom.left + 10, bottom.bottom - 24, cmds.c_str(), static_cast<int>(cmds.size()));

    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

void drawGroundUI(HDC hdc, int width, int height) {
    gGround.render(hdc, width, height);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_SIZE: {
            int newWidth = std::max(1, static_cast<int>(LOWORD(lParam)));
            int newHeight = std::max(1, static_cast<int>(HIWORD(lParam)));
            gRenderer.setViewport(newWidth, newHeight);
            return 0;
        }
        case WM_CHAR:
            if (gInGround) {
                gGround.handleChar(static_cast<wchar_t>(wParam));
            }
            return 0;
        case WM_DESTROY:
            gRunning = false;
            PostQuitMessage(0);
            return 0;
        default:
            break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    std::srand(static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;

    if (!RegisterClassW(&wc)) {
        return -1;
    }

    RECT rect{0, 0, INITIAL_WIDTH, INITIAL_HEIGHT};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hwnd = CreateWindowExW(
        0,
        WINDOW_CLASS_NAME,
        L"DRPG Wireframe",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    if (!hwnd) {
        return -1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg{};
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (gRunning) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                gRunning = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!gRunning) {
            break;
        }

        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> delta = now - lastTime;
        lastTime = now;

        if (gInGround) {
            handleGroundInput();
            gGround.update();
            if (gGround.shouldEnterDungeon()) {
                gActiveParty = gGround.getPartyMembers();
                gGround.clearEnterDungeon();
                gInGround = false;
            }
        } else {
            processInput();
        }

        RECT clientRect{};
        GetClientRect(hwnd, &clientRect);

        HDC hdc = GetDC(hwnd);
        if (gInGround) {
            gRenderer.setViewport(clientRect.right, clientRect.bottom);
            gRenderer.beginFrame();
            gRenderer.present(hdc);
            drawGroundUI(hdc, clientRect.right, clientRect.bottom);
        } else if (gBattle.isActive()) {
            drawBattleUI(hdc, clientRect.right, clientRect.bottom);
        } else {
            updateViewportLayout(clientRect.right, clientRect.bottom);
            gRenderer.beginFrame();
            renderDungeon(gRenderer, gDungeon, gPlayer);
            gRenderer.present(hdc);
            drawWizardryUI(hdc, clientRect.right, clientRect.bottom);
        }
        ReleaseDC(hwnd, hdc);

        updateWindowTitle(hwnd);

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return static_cast<int>(msg.wParam);
}
