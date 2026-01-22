#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <algorithm>
#include <cmath>
#include <vector>

#include "Vec3.h"

// ワイヤーフレームレンダラー
class Renderer {
public:
    Renderer(int w, int h)
        : width(std::max(1, w)),
          height(std::max(1, h)),
          nearPlane(0.05f),
          farPlane(100.0f),
          fovDegrees(70.0f),
          wireColor(RGB(60, 220, 180)),
          backgroundColor(RGB(8, 8, 12)) {
            viewportX = 0;
            viewportY = 0;
            viewportWidth = width;
            viewportHeight = height;
        }

    void setViewport(int w, int h) {
        width = std::max(1, w);
        height = std::max(1, h);
        viewportX = 0;
        viewportY = 0;
        viewportWidth = width;
        viewportHeight = height;
    }

    void setViewportRect(int x, int y, int w, int h) {
        viewportX = std::max(0, x);
        viewportY = std::max(0, y);
        viewportWidth = std::max(1, w);
        viewportHeight = std::max(1, h);
    }

    void setFov(float degrees) {
        fovDegrees = std::clamp(degrees, 30.0f, 120.0f);
    }

    void setWireColor(COLORREF color) { wireColor = color; }
    void setBackgroundColor(COLORREF color) { backgroundColor = color; }

    void beginFrame() { lines.clear(); }

    void drawLine3D(const Vec3& start, const Vec3& end) {
        POINT a{}, b{};
        if (!project(start, a) || !project(end, b)) {
            return;
        }

        lines.push_back({a, b});
    }

    void present(HDC hdc) {
        if (!hdc) {
            return;
        }

        RECT rect{0, 0, width, height};
        HBRUSH bg = CreateSolidBrush(backgroundColor);
        FillRect(hdc, &rect, bg);
        DeleteObject(bg);

        RECT viewRect{viewportX, viewportY, viewportX + viewportWidth, viewportY + viewportHeight};
        HBRUSH viewBrush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &viewRect, viewBrush);
        DeleteObject(viewBrush);

        HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(50, 50, 60));
        HGDIOBJ oldBorderPen = SelectObject(hdc, borderPen);
        HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
        Rectangle(hdc, viewportX, viewportY, viewportX + viewportWidth, viewportY + viewportHeight);
        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldBorderPen);
        DeleteObject(borderPen);

        HPEN pen = CreatePen(PS_SOLID, 1, wireColor);
        HGDIOBJ oldPen = SelectObject(hdc, pen);

        for (const auto& line : lines) {
            MoveToEx(hdc, line.start.x, line.start.y, nullptr);
            LineTo(hdc, line.end.x, line.end.y);
        }

        SelectObject(hdc, oldPen);
        DeleteObject(pen);
    }

private:
    struct LineSegment {
        POINT start;
        POINT end;
    };

    bool project(const Vec3& point, POINT& out) const {
        if (point.z < nearPlane || point.z > farPlane) {
            return false;
        }

        const float aspect = static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight);
        const float fovRad = fovDegrees * 3.14159265f / 180.0f;
        const float f = 1.0f / std::tan(fovRad * 0.5f);

        const float ndcX = (point.x * f / aspect) / point.z;
        const float ndcY = (point.y * f) / point.z;

        out.x = viewportX + static_cast<int>((ndcX + 1.0f) * 0.5f * viewportWidth);
        out.y = viewportY + static_cast<int>((1.0f - ndcY) * 0.5f * viewportHeight);
        return true;
    }

    int width;
    int height;
    int viewportX;
    int viewportY;
    int viewportWidth;
    int viewportHeight;
    float nearPlane;
    float farPlane;
    float fovDegrees;
    COLORREF wireColor;
    COLORREF backgroundColor;
    std::vector<LineSegment> lines;
};
