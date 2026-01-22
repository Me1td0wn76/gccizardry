#pragma once
#include "Vec3.h"
#include <cmath>

// 4x4行列クラス
class Matrix4 {
public:
    float m[4][4];

    Matrix4() {
        identity();
    }

    void identity() {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                m[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }

    // 回転行列（Y軸周り）
    static Matrix4 rotationY(float angle) {
        Matrix4 mat;
        float c = std::cos(angle);
        float s = std::sin(angle);
        
        mat.m[0][0] = c;
        mat.m[0][2] = s;
        mat.m[2][0] = -s;
        mat.m[2][2] = c;
        
        return mat;
    }

    // 平行移動行列
    static Matrix4 translation(float x, float y, float z) {
        Matrix4 mat;
        mat.m[0][3] = x;
        mat.m[1][3] = y;
        mat.m[2][3] = z;
        return mat;
    }

    // ベクトル変換
    Vec3 transform(const Vec3& v) const {
        float x = v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + m[0][3];
        float y = v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + m[1][3];
        float z = v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + m[2][3];
        return Vec3(x, y, z);
    }

    // 行列の乗算
    Matrix4 operator*(const Matrix4& other) const {
        Matrix4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[i][j] = 0;
                for (int k = 0; k < 4; k++) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }
};
