#pragma once
#include "Vec3.h"
#include "Matrix4.h"
#include "Dungeon.h"
#include <cmath>

// プレイヤー（カメラ）
class Player {
private:
    int gridX;
    int gridZ;
    int direction; // 0: 北(+Z), 1: 東(+X), 2: 南(-Z), 3: 西(-X)
    const float eyeHeight = 0.5f;

public:
    Player(int startGridX = 1, int startGridZ = 1)
        : gridX(startGridX), gridZ(startGridZ), direction(0) {}

    // カメラのビュー行列を取得
    Matrix4 getViewMatrix() const {
        const float angle = getAngle();
        Matrix4 rotation = Matrix4::rotationY(-angle);
        Matrix4 translation = Matrix4::translation(-getX(), -getY(), -getZ());
        return rotation * translation;
    }

    // 前進
    bool moveForward(const Dungeon& dungeon) {
        int dx = 0;
        int dz = 0;
        getForwardDelta(dx, dz);

        int nextX = gridX + dx;
        int nextZ = gridZ + dz;
        if (!dungeon.isWall(nextX, nextZ)) {
            gridX = nextX;
            gridZ = nextZ;
            return true;
        }
        return false;
    }

    // 後退
    bool moveBackward(const Dungeon& dungeon) {
        int dx = 0;
        int dz = 0;
        getBackwardDelta(dx, dz);

        int nextX = gridX + dx;
        int nextZ = gridZ + dz;
        if (!dungeon.isWall(nextX, nextZ)) {
            gridX = nextX;
            gridZ = nextZ;
            return true;
        }
        return false;
    }

    // 左回転
    void turnLeft() {
        direction = (direction + 3) % 4;
    }

    // 右回転
    void turnRight() {
        direction = (direction + 1) % 4;
    }

    float getX() const { return static_cast<float>(gridX) + 0.5f; }
    float getY() const { return eyeHeight; }
    float getZ() const { return static_cast<float>(gridZ) + 0.5f; }

    float getAngle() const {
        return static_cast<float>(direction) * 1.57079633f;
    }

    int getGridX() const { return gridX; }
    int getGridZ() const { return gridZ; }
    int getDirection() const { return direction; }

    void setGridPosition(int x, int z) {
        gridX = x;
        gridZ = z;
    }

    void getForwardDelta(int& dx, int& dz) const {
        dx = 0;
        dz = 0;
        switch (direction) {
            case 0: dz = 1; break;
            case 1: dx = 1; break;
            case 2: dz = -1; break;
            case 3: dx = -1; break;
        }
    }

    void getBackwardDelta(int& dx, int& dz) const {
        dx = 0;
        dz = 0;
        switch (direction) {
            case 0: dz = -1; break;
            case 1: dx = -1; break;
            case 2: dz = 1; break;
            case 3: dx = 1; break;
        }
    }

    // 方向ベクトルを取得
    Vec3 getForwardDirection() const {
        float angle = getAngle();
        return Vec3(std::sin(angle), 0, std::cos(angle));
    }
};
