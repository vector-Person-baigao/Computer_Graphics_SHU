#ifndef FRUIT_NINJA_H
#define FRUIT_NINJA_H

#include "GraphicsEngine.h"
#include "Point.h"
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include <string>
#include <map>
#include <array>

enum class GameState {
    MENU,
    PLAYING,
    GAME_OVER
};

enum class FruitShape {
    ROUND,
    OVAL,
    TRIANGLE,
    RECTANGLE
};

struct Fruit {
    Point position;
    Point velocity;
    double rotation;
    double rotationSpeed;
    int type; // 0-3: 不同水果，4: 炸弹
    bool isCut;
    bool active;
    std::vector<Point> trajectory;
    double trajectoryTime;
    FruitShape shape;
    bool isHalf;
    int halfDirection;
    double radius;
    double collisionRadius;
    double speedMultiplier;

    Fruit() : position(0, 0), velocity(0, 0), rotation(0), rotationSpeed(0),
        type(0), isCut(false), active(true), trajectoryTime(0),
        shape(FruitShape::ROUND), isHalf(false), halfDirection(0),
        radius(20.0), collisionRadius(20.0), speedMultiplier(1.0) {
    }
};

struct Particle {
    Point position;
    Point velocity;
    double life;
    COLORREF color;

    Particle(Point p, Point v, COLORREF c) : position(p), velocity(v), life(1.0), color(c) {}
};

struct Button {
    Point position;
    int width;
    int height;
    std::string text;
    COLORREF color;

    Button(Point p, int w, int h, const std::string& t, COLORREF c)
        : position(p), width(w), height(h), text(t), color(c) {
    }

    bool contains(Point p) const {
        return p.x >= position.x && p.x <= position.x + width &&
            p.y >= position.y && p.y <= position.y + height;
    }
};

class FruitNinjaGame {
private:
    GameState state;
    int score;
    int lives;
    int combo;
    double gameTime;
    double spawnInterval;
    double minSpawnInterval;


    std::vector<Fruit> fruits;
    std::vector<Fruit> fruitHalves;
    std::vector<Particle> particles;
    std::vector<Point> slashPoints;

    std::mt19937 rng;
    double lastSpawnTime;
    bool mouseDown;
    Point lastMousePos;
    int lastCutTime;

    // 按钮
    Button startButton;
    Button menuButton;

    // 字体位图
    std::map<char, std::vector<std::vector<bool>>> fontBitmaps;
    struct FruitSkin {
        COLORREF base;
        COLORREF accent;
        COLORREF highlight;
    };
    std::array<FruitSkin, 4> fruitSkins;

public:
    FruitNinjaGame();
    void init();
    void update(double deltaTime);
    void render(GraphicsEngine& g);
    void handleMouseDown(Point p);
    void handleMouseUp(Point p);
    void handleMouseMove(Point p);
    void startGame();
    bool isActive() const { return state != GameState::MENU; }

private:
    void slash(Point from, Point to, Fruit* guaranteedHit = nullptr);
    bool checkCollision(Point p1, Point p2, Point center, int radius);
    void spawnFruit(double speedMultiplier = 1.0);
    void spawnParticles(Point position, COLORREF color, int count = 8);
    void drawFruit(GraphicsEngine& g, const Fruit& fruit);
    void drawFruitOptimized(GraphicsEngine& g, const Fruit& fruit);
    void drawUI(GraphicsEngine& g);
    void drawMenu(GraphicsEngine& g);
    void drawGameOver(GraphicsEngine& g);
    void drawButton(GraphicsEngine& g, const Button& button);
    void drawText(GraphicsEngine& g, Point position, const std::string& text, COLORREF color);
    void drawSimpleText(GraphicsEngine& g, Point position, const std::string& text, COLORREF color);

    // 新增字体相关方法
    void initFont();
    void initFruitSkins();
    void drawChar(GraphicsEngine& g, Point position, char c, COLORREF color);
    void drawBitmapText(GraphicsEngine& g, Point position, const std::string& text, COLORREF color);
    bool pointInsideFruitShape(const Fruit& fruit, double x, double y) const;
    COLORREF sampleFruitColor(const Fruit& fruit, double nx, double ny) const;

};

extern FruitNinjaGame g_fruitNinjaGame;

#endif
