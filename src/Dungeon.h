#pragma once
#include <vector>
#include "map.h"

// ダンジョンマップ
class Dungeon {
private:
    int width;
    int height;
    int floors;
    int currentFloor;
    std::vector<int> baseTiles; // 0=通路, 1=壁, 2=ワープ, 3=落下, 4=トラップ
    std::vector<int> doorTiles; // 0=なし, 1=閉じたドア, 2=開いたドア

public:
    Dungeon() : width(Map::width()), height(Map::height()), floors(Map::floors()), currentFloor(0) {
        baseTiles.resize(width * height * floors, 1);
        doorTiles.resize(width * height * floors, 0);

        for (int f = 0; f < floors; f++) {
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    baseTiles[index(f, x, y)] = Map::getTile(f, x, y);
                }
            }

            setDoor(f, 2, 4, true);
            setDoor(f, 6, 4, true);
        }
    }

    void setDoor(int floor, int x, int y, bool closed) {
        if (isInside(floor, x, y)) {
            doorTiles[index(floor, x, y)] = closed ? 1 : 2;
        }
    }

    int getTile(int x, int y) const {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            return baseTiles[index(currentFloor, x, y)];
        }
        return 1; // 範囲外は壁
    }

    bool isWall(int x, int y) const {
        int tile = getTile(x, y);
        return tile == 1 || isDoorClosed(x, y);
    }

    bool isWarp(int x, int y) const { return getTile(x, y) == 2; }
    bool isFall(int x, int y) const { return getTile(x, y) == 3; }
    bool isTrap(int x, int y) const { return getTile(x, y) == 4; }
    bool isStairs(int x, int y) const { return getTile(x, y) == 6; }

    bool isDoor(int x, int y) const {
        if (!isInside(currentFloor, x, y)) {
            return false;
        }
        return doorTiles[index(currentFloor, x, y)] != 0;
    }

    bool isDoorClosed(int x, int y) const {
        if (!isInside(currentFloor, x, y)) {
            return false;
        }
        return doorTiles[index(currentFloor, x, y)] == 1;
    }

    bool isDoorOpen(int x, int y) const {
        if (!isInside(currentFloor, x, y)) {
            return false;
        }
        return doorTiles[index(currentFloor, x, y)] == 2;
    }

    void toggleDoor(int x, int y) {
        if (!isDoor(x, y)) {
            return;
        }
        int& door = doorTiles[index(currentFloor, x, y)];
        door = (door == 1) ? 2 : 1;
    }

    int getFloor() const { return currentFloor; }
    int getFloorCount() const { return floors; }

    void setFloor(int floor) {
        if (floor < 0) {
            currentFloor = 0;
        } else if (floor >= floors) {
            currentFloor = floors - 1;
        } else {
            currentFloor = floor;
        }
    }

    bool canGoUp() const { return currentFloor > 0; }
    bool canGoDown() const { return currentFloor + 1 < floors; }

    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    int index(int floor, int x, int y) const {
        return (floor * height + y) * width + x;
    }

    bool isInside(int floor, int x, int y) const {
        if (floor < 0 || floor >= floors) {
            return false;
        }
        if (x < 0 || x >= width || y < 0 || y >= height) {
            return false;
        }
        return true;
    }
};
