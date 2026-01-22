@echo off
echo ビルド中...
g++ -std=c++17 -O2 src/main.cpp src/battle.cpp src/enemy.cpp src/character.cpp src/ground.cpp src/map.cpp src/map.c -lgdi32 -luser32 -o drpg.exe
if %errorlevel% equ 0 (
    echo ビルド成功！
    echo.
    echo 実行: drpg.exe
) else (
    echo ビルドエラー
)
pause
