#include "fruit_ninja.h"
#include "Algorithms/Line_Bersenham.h"
#include "Algorithms/Circle_Midpoint.h"
#include <cmath>
#include <algorithm>
#include <string>

FruitNinjaGame g_fruitNinjaGame;

FruitNinjaGame::FruitNinjaGame()
    : state(GameState::MENU), score(0), lives(3), combo(0), gameTime(0),
    spawnInterval(1.2), minSpawnInterval(0.4),
    rng(static_cast<unsigned int>(time(nullptr))), lastSpawnTime(0),
    mouseDown(false), lastCutTime(0),
    startButton(Point(300, 400), 200, 60, "Start Game", RGB(100, 200, 100)),
    menuButton(Point(300, 400), 200, 60, "Back to Menu", RGB(100, 150, 200))
{
    initFont();
    initFruitSkins();
}


void FruitNinjaGame::initFont() {
    // 5x7 像素字体位图定义

    // 数字 0-9
    fontBitmaps['0'] = {
        {1,1,1,1,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,1}
    };

    fontBitmaps['1'] = {
        {0,0,1,0,0},
        {0,1,1,0,0},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {0,1,1,1,0}
    };

    fontBitmaps['2'] = {
        {1,1,1,1,1},
        {0,0,0,0,1},
        {0,0,0,0,1},
        {1,1,1,1,1},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,1,1,1,1}
    };

    fontBitmaps['3'] = {
        {1,1,1,1,1},
        {0,0,0,0,1},
        {0,0,0,0,1},
        {0,1,1,1,1},
        {0,0,0,0,1},
        {0,0,0,0,1},
        {1,1,1,1,1}
    };

    fontBitmaps['4'] = {
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,1},
        {0,0,0,0,1},
        {0,0,0,0,1},
        {0,0,0,0,1}
    };

    fontBitmaps['5'] = {
        {1,1,1,1,1},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,1,1,1,1},
        {0,0,0,0,1},
        {0,0,0,0,1},
        {1,1,1,1,1}
    };

    fontBitmaps['6'] = {
        {1,1,1,1,1},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,1,1,1,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,1}
    };

    fontBitmaps['7'] = {
        {1,1,1,1,1},
        {0,0,0,0,1},
        {0,0,0,0,1},
        {0,0,0,1,0},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {0,0,1,0,0}
    };

    fontBitmaps['8'] = {
        {1,1,1,1,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,1}
    };

    fontBitmaps['9'] = {
        {1,1,1,1,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,1},
        {0,0,0,0,1},
        {0,0,0,0,1},
        {1,1,1,1,1}
    };

    // 字母 A-Z
    fontBitmaps['A'] = {
        {0,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1}
    };

    fontBitmaps['B'] = {
        {1,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,0}
    };

    fontBitmaps['C'] = {
        {0,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,0,0,0,1},
        {0,1,1,1,0}
    };

    fontBitmaps['D'] = {
        {1,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,0}
    };

    fontBitmaps['E'] = {
        {1,1,1,1,1},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,1,1,1,0},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,1,1,1,1}
    };

    fontBitmaps['F'] = {
        {1,1,1,1,1},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,1,1,1,0},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,0,0,0,0}
    };

    fontBitmaps['G'] = {
        {0,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,0},
        {1,0,1,1,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {0,1,1,1,0}
    };

    fontBitmaps['H'] = {
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1}
    };

    fontBitmaps['I'] = {
        {1,1,1,1,1},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {1,1,1,1,1}
    };

    fontBitmaps['J'] = {
        {0,0,0,0,1},
        {0,0,0,0,1},
        {0,0,0,0,1},
        {0,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {0,1,1,1,0}
    };

    fontBitmaps['K'] = {
        {1,0,0,0,1},
        {1,0,0,1,0},
        {1,0,1,0,0},
        {1,1,0,0,0},
        {1,0,1,0,0},
        {1,0,0,1,0},
        {1,0,0,0,1}
    };

    fontBitmaps['L'] = {
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,1,1,1,1}
    };

    fontBitmaps['M'] = {
        {1,0,0,0,1},
        {1,1,0,1,1},
        {1,0,1,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1}
    };

    fontBitmaps['N'] = {
        {1,0,0,0,1},
        {1,1,0,0,1},
        {1,0,1,0,1},
        {1,0,0,1,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1}
    };

    fontBitmaps['O'] = {
        {0,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {0,1,1,1,0}
    };

    fontBitmaps['P'] = {
        {1,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,0},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,0,0,0,0}
    };

    fontBitmaps['Q'] = {
        {0,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,1,0,1},
        {1,0,0,1,0},
        {0,1,1,0,1}
    };

    fontBitmaps['R'] = {
        {1,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,0},
        {1,0,1,0,0},
        {1,0,0,1,0},
        {1,0,0,0,1}
    };

    fontBitmaps['S'] = {
        {0,1,1,1,1},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {0,1,1,1,0},
        {0,0,0,0,1},
        {0,0,0,0,1},
        {1,1,1,1,0}
    };

    fontBitmaps['T'] = {
        {1,1,1,1,1},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {0,0,1,0,0}
    };

    fontBitmaps['U'] = {
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {0,1,1,1,0}
    };

    fontBitmaps['V'] = {
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {0,1,0,1,0},
        {0,1,0,1,0},
        {0,0,1,0,0}
    };

    fontBitmaps['W'] = {
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,1,0,1},
        {1,0,1,0,1},
        {1,1,0,1,1},
        {1,0,0,0,1}
    };

    fontBitmaps['X'] = {
        {1,0,0,0,1},
        {1,0,0,0,1},
        {0,1,0,1,0},
        {0,0,1,0,0},
        {0,1,0,1,0},
        {1,0,0,0,1},
        {1,0,0,0,1}
    };

    fontBitmaps['Y'] = {
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {0,1,0,1,0},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {0,0,1,0,0}
    };

    fontBitmaps['Z'] = {
        {1,1,1,1,1},
        {0,0,0,0,1},
        {0,0,0,1,0},
        {0,0,1,0,0},
        {0,1,0,0,0},
        {1,0,0,0,0},
        {1,1,1,1,1}
    };

    // 小写字母 a-z
    fontBitmaps['a'] = {
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,1,1,1,0},
        {0,0,0,0,1},
        {0,1,1,1,1},
        {1,0,0,0,1},
        {0,1,1,1,1}
    };

    fontBitmaps['b'] = {
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,0}
    };

    fontBitmaps['c'] = {
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,1,1,1,0},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,0,0,0,1},
        {0,1,1,1,0}
    };

    fontBitmaps['d'] = {
        {0,0,0,0,1},
        {0,0,0,0,1},
        {0,1,1,1,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {0,1,1,1,1}
    };

    fontBitmaps['e'] = {
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,1,1,1,0},
        {1,0,0,0,1},
        {1,1,1,1,1},
        {1,0,0,0,0},
        {0,1,1,1,0}
    };

    fontBitmaps['f'] = {
        {0,0,1,1,0},
        {0,1,0,0,1},
        {0,1,0,0,0},
        {1,1,1,0,0},
        {0,1,0,0,0},
        {0,1,0,0,0},
        {0,1,0,0,0}
    };

    fontBitmaps['g'] = {
        {0,0,0,0,0},
        {0,1,1,1,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {0,1,1,1,1},
        {0,0,0,0,1},
        {0,1,1,1,0}
    };

    fontBitmaps['h'] = {
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1}
    };

    fontBitmaps['i'] = {
        {0,1,0,0,0},
        {0,0,0,0,0},
        {0,1,0,0,0},
        {0,1,0,0,0},
        {0,1,0,0,0},
        {0,1,0,0,0},
        {0,0,1,0,0}
    };

    fontBitmaps['j'] = {
        {0,0,0,1,0},
        {0,0,0,0,0},
        {0,0,1,1,0},
        {0,0,0,1,0},
        {0,0,0,1,0},
        {1,0,0,1,0},
        {0,1,1,0,0}
    };

    fontBitmaps['k'] = {
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,0,0,1,0},
        {1,0,1,0,0},
        {1,1,0,0,0},
        {1,0,1,0,0},
        {1,0,0,1,0}
    };

    fontBitmaps['l'] = {
        {0,1,0,0,0},
        {0,1,0,0,0},
        {0,1,0,0,0},
        {0,1,0,0,0},
        {0,1,0,0,0},
        {0,1,0,0,0},
        {0,0,1,0,0}
    };

    fontBitmaps['m'] = {
        {0,0,0,0,0},
        {0,0,0,0,0},
        {1,1,0,1,0},
        {1,0,1,0,1},
        {1,0,1,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1}
    };

    fontBitmaps['n'] = {
        {0,0,0,0,0},
        {0,0,0,0,0},
        {1,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1}
    };

    fontBitmaps['o'] = {
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {0,1,1,1,0}
    };

    fontBitmaps['p'] = {
        {0,0,0,0,0},
        {1,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,0},
        {1,0,0,0,0},
        {1,0,0,0,0}
    };

    fontBitmaps['q'] = {
        {0,0,0,0,0},
        {0,1,1,1,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {0,1,1,1,1},
        {0,0,0,0,1},
        {0,0,0,0,1}
    };

    fontBitmaps['r'] = {
        {0,0,0,0,0},
        {0,0,0,0,0},
        {1,0,1,1,0},
        {1,1,0,0,1},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,0,0,0,0}
    };

    fontBitmaps['s'] = {
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,1,1,1,0},
        {1,0,0,0,0},
        {0,1,1,1,0},
        {0,0,0,0,1},
        {1,1,1,1,0}
    };

    fontBitmaps['t'] = {
        {0,1,0,0,0},
        {0,1,0,0,0},
        {1,1,1,0,0},
        {0,1,0,0,0},
        {0,1,0,0,0},
        {0,1,0,0,1},
        {0,0,1,1,0}
    };

    fontBitmaps['u'] = {
        {0,0,0,0,0},
        {0,0,0,0,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,1,1},
        {0,1,1,0,1}
    };

    fontBitmaps['v'] = {
        {0,0,0,0,0},
        {0,0,0,0,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {0,1,0,1,0},
        {0,0,1,0,0}
    };

    fontBitmaps['w'] = {
        {0,0,0,0,0},
        {0,0,0,0,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,1,0,1},
        {1,0,1,0,1},
        {0,1,0,1,0}
    };

    fontBitmaps['x'] = {
        {0,0,0,0,0},
        {0,0,0,0,0},
        {1,0,0,0,1},
        {0,1,0,1,0},
        {0,0,1,0,0},
        {0,1,0,1,0},
        {1,0,0,0,1}
    };

    fontBitmaps['y'] = {
        {0,0,0,0,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {0,1,1,1,1},
        {0,0,0,0,1},
        {0,1,1,1,0}
    };

    fontBitmaps['z'] = {
        {0,0,0,0,0},
        {0,0,0,0,0},
        {1,1,1,1,1},
        {0,0,0,1,0},
        {0,0,1,0,0},
        {0,1,0,0,0},
        {1,1,1,1,1}
    };

    // 特殊字符
    fontBitmaps[' '] = {
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0}
    };

    fontBitmaps[':'] = {
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,1,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,1,0,0},
        {0,0,0,0,0}
    };

    fontBitmaps['!'] = {
        {0,0,1,0,0},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {0,0,0,0,0},
        {0,0,1,0,0},
        {0,0,0,0,0}
    };

    fontBitmaps['?'] = {
        {0,1,1,1,0},
        {1,0,0,0,1},
        {0,0,0,0,1},
        {0,0,1,1,0},
        {0,0,1,0,0},
        {0,0,0,0,0},
        {0,0,1,0,0}
    };

    fontBitmaps['.'] = {
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,1,0,0}
    };

    fontBitmaps[','] = {
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,1,0,0},
        {0,1,0,0,0}
    };

    fontBitmaps['-'] = {
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {1,1,1,1,1},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0}
    };

    fontBitmaps['+'] = {
        {0,0,0,0,0},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {1,1,1,1,1},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {0,0,0,0,0}
    };

    fontBitmaps['='] = {
        {0,0,0,0,0},
        {0,0,0,0,0},
        {1,1,1,1,1},
        {0,0,0,0,0},
        {1,1,1,1,1},
        {0,0,0,0,0},
        {0,0,0,0,0}
    };
}

void FruitNinjaGame::initFruitSkins() {
    fruitSkins[0] = { RGB(205, 50, 50), RGB(120, 20, 20), RGB(255, 215, 215) };   // 苹果
    fruitSkins[1] = { RGB(90, 180, 60), RGB(70, 90, 40), RGB(220, 255, 220) };    // 奇异果
    fruitSkins[2] = { RGB(255, 180, 70), RGB(180, 90, 30), RGB(255, 240, 200) };  // 橙子
    fruitSkins[3] = { RGB(40, 160, 90), RGB(20, 90, 40), RGB(200, 255, 200) };    // 西瓜
}

void FruitNinjaGame::init() {
    fruits.clear();
    fruitHalves.clear();
    particles.clear();
    slashPoints.clear();
    score = 0;
    lives = 3;
    combo = 0;
    gameTime = 0;
    lastSpawnTime = 0;
    lastCutTime = 0;
    spawnInterval = 0.9;
}

void FruitNinjaGame::startGame() {
    init();
    state = GameState::PLAYING;
}

void FruitNinjaGame::update(double deltaTime) {
    if (state != GameState::PLAYING) return;

    gameTime += deltaTime;
    // 动态调整游戏难度 - 随时间增加速度和生成频率
    double speedMultiplier = 1.0 + (gameTime * 0.03); // 每秒钟增加2%速度
    double spawnMultiplier = 1.0 - (gameTime * 0.01); // 减少生成间隔
    speedMultiplier = std::min(speedMultiplier, 2.5); // 最大2.5倍速度
    spawnMultiplier = std::max(spawnMultiplier, 0.5); // 最小0.4倍间隔

    // 生成新水果，整体节奏放缓并让屏幕更干净
    int activeFruitCount = static_cast<int>(std::count_if(fruits.begin(), fruits.end(),
        [](const Fruit& f) { return f.active; }));
    double difficultyInterval = std::max(minSpawnInterval, spawnInterval - gameTime * 0.01);
    double dynamicInterval = difficultyInterval + activeFruitCount * 0.02;
    if (activeFruitCount < 4 && gameTime - lastSpawnTime > dynamicInterval) {
        spawnFruit(speedMultiplier);
        lastSpawnTime = gameTime;
    }

    // 更新水果位置
    for (auto& fruit : fruits) {
        if (!fruit.active) continue;

        // 使用Bezier曲线计算抛物线轨迹
        if (fruit.trajectory.size() >= 3) {
            fruit.trajectoryTime += deltaTime * fruit.speedMultiplier;
            if (fruit.trajectoryTime > 1.0) {
                fruit.trajectoryTime = 1.0;
                fruit.active = false;
            }

            double t = fruit.trajectoryTime;
            double u = 1 - t;
            Point p0 = fruit.trajectory[0];
            Point p1 = fruit.trajectory[1];
            Point p2 = fruit.trajectory[2];

            fruit.position.x = static_cast<int>(u * u * p0.x + 2 * u * t * p1.x + t * t * p2.x);
            fruit.position.y = static_cast<int>(u * u * p0.y + 2 * u * t * p1.y + t * t * p2.y);
        }

        fruit.rotation += fruit.rotationSpeed * deltaTime;

        // 检查水果是否掉出屏幕
        if (fruit.position.y > 650 || fruit.trajectoryTime >= 1.0) {
            if (!fruit.isCut && fruit.type != 4) {
                lives--;
                if (lives <= 0) {
                    state = GameState::GAME_OVER;
                }
            }
            fruit.active = false;
        }
    }

    // 更新被切开的水果
    for (auto& half : fruitHalves) {
        half.velocity.y += 980 * deltaTime; 
        half.position.x += static_cast<int>(half.velocity.x * deltaTime);
        half.position.y += static_cast<int>(half.velocity.y * deltaTime);
        half.rotation += half.rotationSpeed * deltaTime;

        if (half.position.y > 650) {
            half.active = false;
        }
    }

    // 更新粒子 - 优化粒子数量
    for (auto it = particles.begin(); it != particles.end(); ) {
        it->life -= deltaTime * 4.0; // 加快粒子消失速度
        it->position.x += static_cast<int>(it->velocity.x * deltaTime);
        it->position.y += static_cast<int>(it->velocity.y * deltaTime);
        it->velocity.y += 600 * deltaTime; // 增加粒子重力

        if (it->life <= 0) {
            it = particles.erase(it);
        }
        else {
            ++it;
        }
    }

    // 清理无效对象
    fruits.erase(std::remove_if(fruits.begin(), fruits.end(),
        [](const Fruit& f) { return !f.active; }), fruits.end());
    fruitHalves.erase(std::remove_if(fruitHalves.begin(), fruitHalves.end(),
        [](const Fruit& f) { return !f.active; }), fruitHalves.end());

    // 连击超时
    if (combo > 0 && gameTime * 1000 - lastCutTime > 2000) {
        combo = 0;
    }
}

void FruitNinjaGame::render(GraphicsEngine& g) {

    switch (state) {
    case GameState::MENU:
        drawMenu(g);
        break;
    case GameState::PLAYING:
        // 优化绘制顺序：先绘制小的、简单的对象
        for (const auto& particle : particles) {
            if (particle.life > 0.2f) {
                Object* pObj = new Object();
                pObj->setColor(GetRValue(particle.color), GetGValue(particle.color), GetBValue(particle.color));
                pObj->points.push_back(Pixel(particle.position.x, particle.position.y, particle.color));
                g.pushObject(pObj);
            }
        }

        // 使用简化绘制模式
        for (const auto& fruit : fruits) {
            if (fruit.active) {
                drawFruitOptimized(g, fruit);
            }
        }

        for (const auto& half : fruitHalves) {
            if (half.active) {
                drawFruitOptimized(g, half);
            }
        }

        // 简化切割轨迹绘制
        if (slashPoints.size() >= 2) {
            Object* slashObj = new Object();
            slashObj->setColor(255, 0, 0);

            // 进一步减少轨迹点
            for (size_t i = 1; i < slashPoints.size(); i += 3) {
                if (i < slashPoints.size()) {
                    auto segment = lineBresenham(slashPoints[i - 1], slashPoints[i]);
                    for (const auto& p : segment) {
                        if (segment.size() < 20) { // 限制每段长度
                            slashObj->points.push_back(Pixel(p.x, p.y, RGB(255, 0, 0)));
                        }
                    }
                }
            }
            g.pushObject(slashObj);
        }

        drawUI(g);
        break;
    case GameState::GAME_OVER:
        drawGameOver(g);
        break;
    }
}

void FruitNinjaGame::handleMouseDown(Point p) {
    if (state == GameState::MENU) {
        if (startButton.contains(p)) {
            startGame();
        }
        return;
    }

    if (state == GameState::GAME_OVER) {
        if (menuButton.contains(p)) {
            state = GameState::MENU;
        }
        return;
    }

    if (state == GameState::PLAYING) {
        mouseDown = true;
        lastMousePos = p;
        slashPoints.clear();
        slashPoints.push_back(p);
    }
}

void FruitNinjaGame::handleMouseUp(Point p) {
    if (state == GameState::PLAYING && mouseDown) {
        mouseDown = false;
        if (slashPoints.size() >= 2) {
            slash(slashPoints.front(), slashPoints.back(), nullptr);
        }
        slashPoints.clear();
    }
}

void FruitNinjaGame::handleMouseMove(Point p) {
    if (state == GameState::PLAYING && mouseDown) {
        slashPoints.push_back(p);

        // 限制轨迹点数量，避免过多点导致性能问题
        if (slashPoints.size() > 20) {
            slashPoints.erase(slashPoints.begin());
        }

        // 实时碰撞检测
        if (slashPoints.size() >= 2) {
            Point from = slashPoints[slashPoints.size() - 2];
            Point to = slashPoints[slashPoints.size() - 1];

            for (auto& fruit : fruits) {
                if (fruit.active && !fruit.isCut &&
                    checkCollision(from, to, fruit.position, static_cast<int>(fruit.collisionRadius))) {
                    slash(from, to, &fruit);
                    break;
                }
            }
        }

        lastMousePos = p;
    }
}

void FruitNinjaGame::slash(Point from, Point to, Fruit* guaranteedHit) {
    auto processFruit = [&](Fruit& fruit) {
        if (!fruit.active || fruit.isCut) return false;

        fruit.isCut = true;

        if (fruit.type == 4) { // 炸弹
            lives = 0;
            state = GameState::GAME_OVER;
            spawnParticles(fruit.position, RGB(0, 0, 0), 12);
        }
        else {
            int points = (fruit.type + 1) * 10 * (combo + 1);
            score += points;
            combo++;
            lastCutTime = static_cast<int>(gameTime * 1000);

            for (int i = 0; i < 2; i++) {
                Fruit half = fruit;
                half.position = fruit.position;
                half.velocity = Point((i == 0 ? -160 : 160), -280);
                half.rotation = fruit.rotation;
                half.rotationSpeed = fruit.rotationSpeed * 1.1;
                half.type = fruit.type;
                half.isHalf = true;
                half.halfDirection = (i == 0 ? -1 : 1);
                half.isCut = true;
                half.active = true;
                half.trajectory.clear();
                half.trajectoryTime = 0;
                half.collisionRadius = fruit.collisionRadius * 0.6;
                half.position.x += half.halfDirection * static_cast<int>(half.radius * 0.4);
                fruitHalves.push_back(half);
            }

            COLORREF fruitColors[] = { RGB(255, 100, 100), RGB(100, 255, 100),
                                    RGB(100, 100, 255), RGB(255, 255, 100) };
            spawnParticles(fruit.position, fruitColors[fruit.type], 8);
        }

        fruit.active = false;
        return true;
    };

    if (guaranteedHit) {
        processFruit(*guaranteedHit);
    }

    int minX = std::min(from.x, to.x);
    int maxX = std::max(from.x, to.x);
    int minY = std::min(from.y, to.y);
    int maxY = std::max(from.y, to.y);

    for (auto& fruit : fruits) {
        if (&fruit == guaranteedHit) continue;
        if (!fruit.active || fruit.isCut) continue;

        int radius = static_cast<int>(fruit.collisionRadius);
        if (fruit.position.x + radius < minX - 30 || fruit.position.x - radius > maxX + 30 ||
            fruit.position.y + radius < minY - 30 || fruit.position.y - radius > maxY + 30) {
            continue;
        }

        if (checkCollision(from, to, fruit.position, radius)) {
            processFruit(fruit);
        }
    }
}

bool FruitNinjaGame::checkCollision(Point p1, Point p2, Point center, int radius) {
    // 线段与圆的碰撞检测
    Point d = Point(p2.x - p1.x, p2.y - p1.y);
    Point f = Point(p1.x - center.x, p1.y - center.y);

    double a = d.x * d.x + d.y * d.y;
    double b = 2 * (f.x * d.x + f.y * d.y);
    double c = f.x * f.x + f.y * f.y - radius * radius;

    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return false;

    discriminant = sqrt(discriminant);
    double t1 = (-b - discriminant) / (2 * a);
    double t2 = (-b + discriminant) / (2 * a);

    return (t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1);
}

void FruitNinjaGame::spawnFruit(double speedMultiplier) {
    Fruit fruit;
    std::uniform_real_distribution<double> bombDist(0.0, 1.0);
    std::uniform_int_distribution<int> regularTypeDist(0, 3);
    std::uniform_int_distribution<int> posDist(80, 520);
    std::uniform_int_distribution<int> horizontalDist(-320, 320);
    std::uniform_int_distribution<int> peakDist(60, 100);
    //std::uniform_real_distribution<double> rotDist(-3.14, 3.14);
    //std::uniform_real_distribution<double> rotSpeedDist(-5.0, 5.0);

    // 设置速度倍数
    fruit.speedMultiplier = speedMultiplier;

    if (bombDist(rng) > 0.88) {
        fruit.type = 4;
        fruit.shape = FruitShape::ROUND;
        fruit.radius = 18;
        fruit.collisionRadius = 18;
    }
    else {
        fruit.type = regularTypeDist(rng);
        switch (fruit.type) {
        case 0:
            fruit.shape = FruitShape::ROUND;
            fruit.radius = 22;
            break;
        case 1:
            fruit.shape = FruitShape::OVAL;
            fruit.radius = 24;
            break;
        case 2:
            fruit.shape = FruitShape::RECTANGLE;
            fruit.radius = 26;
            break;
        case 3:
            fruit.shape = FruitShape::TRIANGLE;
            fruit.radius = 24;
            break;
        default:
            fruit.shape = FruitShape::ROUND;
            fruit.radius = 20;
            break;
        }
        fruit.collisionRadius = fruit.radius;
    }

    int startX = posDist(rng);
    int endX = std::clamp(startX + horizontalDist(rng), 40, 760);
    int peakX = (startX + endX) / 2;
    int peakY = peakDist(rng);

    // 让起始点更低，从屏幕下方出现
    int startY = 620;
    int endY = 620;

    fruit.position = Point(startX, startY);
    // 增加旋转速度
    std::uniform_real_distribution<double> rotSpeedDist(-12.0, 12.0);
    fruit.rotationSpeed = rotSpeedDist(rng) * speedMultiplier;

    fruit.isHalf = false;
    fruit.halfDirection = 0;
    fruit.isCut = false;

    fruit.trajectory.clear();
    fruit.trajectory.push_back(Point(startX, startY)); // 起点
    fruit.trajectory.push_back(Point(peakX, peakY));  // 顶点
    fruit.trajectory.push_back(Point(endX, endY)); // 终点

    fruit.trajectoryTime = 0;
    fruits.push_back(fruit);
}
void FruitNinjaGame::spawnParticles(Point position, COLORREF color, int count) {
    std::uniform_real_distribution<double> angleDist(0, 2 * 3.14159);
    std::uniform_real_distribution<double> speedDist(50, 200);

    for (int i = 0; i < count; i++) {
        double angle = angleDist(rng);
        double speed = speedDist(rng);
        Point velocity(static_cast<int>(cos(angle) * speed), static_cast<int>(sin(angle) * speed));
        particles.push_back(Particle(position, velocity, color));
    }
}

bool FruitNinjaGame::pointInsideFruitShape(const Fruit& fruit, double x, double y) const {
    double r = std::max(1.0, fruit.radius);

    switch (fruit.shape) {
    case FruitShape::ROUND:
        return x * x + y * y <= r * r;
    case FruitShape::OVAL: {
        double a = r * 1.3;
        double b = r * 0.8;
        return (x * x) / (a * a) + (y * y) / (b * b) <= 1.0;
    }
    case FruitShape::RECTANGLE:
        return std::abs(x) <= r * 0.9 && std::abs(y) <= r * 0.6;
    case FruitShape::TRIANGLE: {
        double height = r * 1.5;
        double shiftedY = y + height / 2.0;
        if (shiftedY < 0 || shiftedY > height) return false;
        double halfWidth = (height - shiftedY) * (r * 0.9) / height;
        return std::abs(x) <= halfWidth;
    }
    default:
        return true;
    }
}

COLORREF FruitNinjaGame::sampleFruitColor(const Fruit& fruit, double nx, double ny) const {
    double radial = std::min(1.0, std::sqrt(nx * nx + ny * ny));

    if (fruit.type == 4) { // 炸弹
        int shade = static_cast<int>(80 + (1.0 - radial) * 70);
        shade = std::clamp(shade, 40, 180);
        return RGB(shade, shade, shade);
    }

    size_t skinIndex = static_cast<size_t>(std::clamp(fruit.type, 0, 3));
    const FruitSkin& skin = fruitSkins[skinIndex];
    double shadeFactor = std::clamp(1.0 - radial * 0.5, 0.35, 1.0);

    int r = static_cast<int>(GetRValue(skin.base) * shadeFactor);
    int g = static_cast<int>(GetGValue(skin.base) * shadeFactor);
    int b = static_cast<int>(GetBValue(skin.base) * shadeFactor);

    double highlight = std::clamp(0.8 - (nx + ny) * 0.3, 0.0, 1.0);
    r = std::min(255, static_cast<int>(r + highlight * (GetRValue(skin.highlight) - r)));
    g = std::min(255, static_cast<int>(g + highlight * (GetGValue(skin.highlight) - g)));
    b = std::min(255, static_cast<int>(b + highlight * (GetBValue(skin.highlight) - b)));

    if (fruit.type == 1) { // 奇异果籽
        double seedPattern = std::fmod(std::abs(nx * 7.0) + std::abs(ny * 7.0), 1.0);
        if (seedPattern < 0.08 && radial < 0.85) {
            return skin.accent;
        }
    }
    else if (fruit.type == 2) { // 橙子纹理
        double segment = std::fmod((std::atan2(ny, nx) + 3.14159) * 3.0, 1.0);
        if (segment < 0.18) {
            r = (r + GetRValue(skin.accent)) / 2;
            g = (g + GetGValue(skin.accent)) / 2;
            b = (b + GetBValue(skin.accent)) / 2;
        }
    }
    else if (fruit.type == 3) { // 西瓜条纹
        double stripe = std::fmod((nx + 1.0) * 4.0, 1.0);
        if (stripe < 0.25) {
            r = (r + GetRValue(skin.accent)) / 2;
            g = (g + GetGValue(skin.accent)) / 2;
            b = (b + GetBValue(skin.accent)) / 2;
        }
    }

    r = std::clamp(r, 0, 255);
    g = std::clamp(g, 0, 255);
    b = std::clamp(b, 0, 255);

    return RGB(static_cast<byte>(r), static_cast<byte>(g), static_cast<byte>(b));
}

// 优化水果绘制 - 使用更高效的算法
void FruitNinjaGame::drawFruitOptimized(GraphicsEngine& g, const Fruit& fruit) {
    Object* fruitObj = new Object();
    fruitObj->setColor(255, 255, 255);

    int radius = static_cast<int>(fruit.type == 4 ? 18 : fruit.radius);
    double cosR = cos(fruit.rotation);
    double sinR = sin(fruit.rotation);

    // 优化：减少采样点，使用更粗的网格
    int step = (radius > 20) ? 2 : 1; // 大水果用更疏的采样

    for (int dx = -radius; dx <= radius; dx += step) {
        for (int dy = -radius; dy <= radius; dy += step) {
            // 快速边界检查
            if (dx * dx + dy * dy > radius * radius * 1.1) continue;

            bool inside = pointInsideFruitShape(fruit, static_cast<double>(dx), static_cast<double>(dy));
            if (!inside) continue;

            if (fruit.isHalf) {
                if (fruit.halfDirection < 0 && dx > -2) continue;
                if (fruit.halfDirection > 0 && dx < 2) continue;
            }

            int rx = static_cast<int>(dx * cosR - dy * sinR);
            int ry = static_cast<int>(dx * sinR + dy * cosR);

            // 简化颜色计算
            double nx = static_cast<double>(dx) / std::max(1.0, fruit.radius);
            double ny = static_cast<double>(dy) / std::max(1.0, fruit.radius);
            COLORREF pixelColor = sampleFruitColor(fruit, nx, ny);

            fruitObj->points.push_back(
                Pixel(fruit.position.x + rx, fruit.position.y + ry, pixelColor));
        }
    }

    g.pushObject(fruitObj);
}


void FruitNinjaGame::drawFruit(GraphicsEngine& g, const Fruit& fruit) {
    drawFruitOptimized(g, fruit);
}

void FruitNinjaGame::drawChar(GraphicsEngine& g, Point position, char c, COLORREF color) {
    // 查找字符位图
    auto it = fontBitmaps.find(c);
    if (it == fontBitmaps.end()) {
        // 如果找不到字符，使用问号代替
        it = fontBitmaps.find('?');
        if (it == fontBitmaps.end()) return;
    }

    const auto& bitmap = it->second;
    Object* charObj = new Object();
    charObj->setColor(GetRValue(color), GetGValue(color), GetBValue(color));

    // 绘制位图
    for (int y = 0; y < 7; y++) {
        for (int x = 0; x < 5; x++) {
            if (bitmap[y][x]) {
                charObj->points.push_back(Pixel(position.x + x, position.y + y, color));
            }
        }
    }

    g.pushObject(charObj);
}

void FruitNinjaGame::drawBitmapText(GraphicsEngine& g, Point position, const std::string& text, COLORREF color) {
    for (size_t i = 0; i < text.length(); i++) {
        drawChar(g, Point(position.x + i * 6, position.y), text[i], color);
    }
}



void FruitNinjaGame::drawUI(GraphicsEngine& g) {
    // 使用新的位图字体绘制UI
    std::string scoreText = "Score:" + std::to_string(score);
    drawBitmapText(g, Point(50, 50), scoreText, RGB(255, 100, 100));

    std::string livesText = "Lives:" + std::to_string(lives);
    drawBitmapText(g, Point(50, 100), livesText, RGB(255, 100, 100));

    if (combo > 1) {
        std::string comboText = "Combo:x" + std::to_string(combo);
        drawBitmapText(g, Point(50, 150), comboText, RGB(255, 100, 100));
    }
}

void FruitNinjaGame::drawMenu(GraphicsEngine& g) {
    // 使用新的位图字体绘制菜单
    drawBitmapText(g, Point(350, 150), "FRUIT NINJA", RGB(255, 100, 100));

    // 绘制开始按钮
    drawButton(g, startButton);

}

void FruitNinjaGame::drawGameOver(GraphicsEngine& g) {
    // 使用新的位图字体绘制游戏结束界面
    drawBitmapText(g, Point(350, 150), "GAME OVER", RGB(255, 0, 0));

    std::string finalScore = "Score:" + std::to_string(score);
    drawBitmapText(g, Point(320, 170), finalScore, RGB(255, 255, 255));

    // 绘制返回主菜单按钮
    drawButton(g, menuButton);
}

void FruitNinjaGame::drawButton(GraphicsEngine& g, const Button& button) {
    // 绘制按钮背景
    Object* bgObj = new Object();
    bgObj->setColor(GetRValue(button.color), GetGValue(button.color), GetBValue(button.color));

    for (int x = button.position.x; x < button.position.x + button.width; x++) {
        for (int y = button.position.y; y < button.position.y + button.height; y++) {
            bgObj->points.push_back(Pixel(x, y, button.color));
        }
    }
    g.pushObject(bgObj);

    // 绘制按钮文字
    drawBitmapText(g, Point(button.position.x + 20, button.position.y + 20), button.text, RGB(255, 255, 255));
}

// 保留原有的简单文本绘制函数作为备选
void FruitNinjaGame::drawSimpleText(GraphicsEngine& g, Point position, const std::string& text, COLORREF color) {
    drawBitmapText(g, position, text, color);
}

void FruitNinjaGame::drawText(GraphicsEngine& g, Point position, const std::string& text, COLORREF color) {
    drawBitmapText(g, position, text, color);
}
