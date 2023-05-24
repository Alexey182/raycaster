#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>

#include "renderer.hpp"

class Camera;

struct Normal {
    float x,y;
};

class Color {
public:
    Color(float r=0.0, float g=0.0, float b=0.0) : _r(r), _g(g), _b(b) {}
    Color(int index);
    Color& operator /=(float divider) { _r/=divider; _g/=divider; _b/=divider; return *this; }
private:
    float _r, _g, _b;
    friend class Map;
};

class Wall {
public:
    Wall(float x1, float y1, float x2, float y2, int texture_id);
    Wall() = delete;
private:
    float _x1, _y1, _x2, _y2;
    float _a, _b, _c;
    int _texture_id;
    float _length;
    friend class Map;
};

class Camera
{
public:
    Camera();
    void Rotate(float angle);
    void Move(float step);
    void SideMove(float step);

private:
    float _x, _y, _angle;
    friend class Map;
};

class Map {
public:
    Map(Camera &cam, Renderer &r);
    float findNearestIntersectionPoint(float angle, Normal &norm, float &pos, int &texture_id);
    void render(int screen_width, int screen_height);
    void loadMap(const char *filename);
private:
    Map() = delete;
    std::vector<Wall> _walls;
    Camera& _cam;
    Renderer& _renderer;
};

#endif
