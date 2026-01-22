#pragma once

#define MAP_WIDTH_VALUE 10
#define MAP_HEIGHT_VALUE 10
#define MAP_FLOORS_VALUE 10

#ifdef __cplusplus
extern "C" {
#endif

typedef struct WarpPoint {
    int fromFloor;
    int fromX;
    int fromY;
    int toFloor;
    int toX;
    int toY;
} WarpPoint;

extern int MAP_DATA[MAP_FLOORS_VALUE][MAP_WIDTH_VALUE * MAP_HEIGHT_VALUE];
extern WarpPoint MAP_WARPS[];
extern int MAP_WARP_COUNT;

#ifdef __cplusplus
}

class Map {
public:
    static int width();
    static int height();
    static int floors();
    static int getTile(int floor, int x, int y);
    static bool tryWarp(int floor, int x, int y, int& outFloor, int& outX, int& outY);
};
#endif
