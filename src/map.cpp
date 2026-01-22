#include "map.h"

int Map::width() {
    return MAP_WIDTH_VALUE;
}

int Map::height() {
    return MAP_HEIGHT_VALUE;
}

int Map::floors() {
    return MAP_FLOORS_VALUE;
}

int Map::getTile(int floor, int x, int y) {
    if (floor < 0 || floor >= MAP_FLOORS_VALUE) {
        return 1;
    }
    if (x < 0 || y < 0 || x >= MAP_WIDTH_VALUE || y >= MAP_HEIGHT_VALUE) {
        return 1;
    }
    return MAP_DATA[floor][y * MAP_WIDTH_VALUE + x];
}

bool Map::tryWarp(int floor, int x, int y, int& outFloor, int& outX, int& outY) {
    for (int i = 0; i < MAP_WARP_COUNT; i++) {
        if (MAP_WARPS[i].fromFloor == floor && MAP_WARPS[i].fromX == x && MAP_WARPS[i].fromY == y) {
            outFloor = MAP_WARPS[i].toFloor;
            outX = MAP_WARPS[i].toX;
            outY = MAP_WARPS[i].toY;
            return true;
        }
    }
    return false;
}
