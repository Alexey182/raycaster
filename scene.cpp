#include <cmath>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <boost/json.hpp>

namespace json = boost::json;

#include "scene.hpp"

Color::Color(int index) 
    : _r(0.0), _g(0.0), _b(0.0)
{
    switch(index) {
        case 0:
            _r = 0.7;
            break;
        case 1:
            _g = 0.7;
            break;
        case 2:
            _b = 0.7;
            break;
        case 3:
            _r = 0.7;
            _g = 0.7;
            break;
        case 4:
            _r = 0.7;
            _b = 0.7;
            break;
    }
}

Wall::Wall(float x1, float y1, float x2, float y2, int texture_id)
    : _x1((x2 > x1) ? x1 : x2)
    , _y1((y2 > y1) ? y1 : y2)
    , _x2((x2 > x1) ? x2 : x1)
    , _y2((y2 > y1) ? y2 : y1)
    , _texture_id(texture_id)
{
    _a = y2 - y1;
    _b = x1 - x2;
    _c = y1*x2 - x1*y2;
    _length = std::sqrt(std::pow(_y2-_y1, 2) + std::pow(_x2-_x1, 2));
    std::cout << "Length: " << _length << std::endl;
}


Camera::Camera()
    : _x(0.0), _y(0.0), _angle(0.0)
{
}

void Camera::Rotate(float angle)
{
    _angle += angle;
}

void Camera::Move(float step)
{
    _x += step * std::sin(_angle);
    _y += step * std::cos(_angle);
}

void Camera::SideMove(float step)
{
    _x += step * std::sin(_angle + M_PI / 2);
    _y += step * std::cos(_angle + M_PI / 2);
}

Map::Map(Camera &cam, Renderer& r)
    : _cam(cam)
    , _renderer(r)
{
}


float Map::findNearestIntersectionPoint(float angle, Normal &norm, float &pos, int &texture_id)
{

    float camV = std::sin(_cam._angle + angle);
    float camW = std::cos(_cam._angle + angle);
    float distance = NAN;

    for(auto &w : _walls) {
        float denominator = w._a * camV + w._b * camW;
        if(denominator == 0) {
            continue;
        }
        float t = (-w._a * _cam._x - w._b* _cam._y - w._c) / denominator;
        if(t > 0) {
            float x = _cam._x + t*camV;
            float y = _cam._y + t*camW;
            if( ((w._x1 < x) && (w._x2 > x)) || ((w._y1 < y) && (w._y2 > y)) ) {
                float dist = std::sqrt(std::pow(x - _cam._x, 2) + std::pow(y - _cam._y, 2));
                if(std::isnan(distance) || (dist < distance) ) {
                    if( (w._x2 - w._x1) > (w._y2 - w._y1) ){
                        pos = w._length * (x - w._x1) / (w._x2 - w._x1);
                    } else {
                        pos = w._length * (y - w._y1) / (w._y2 - w._y1);
                    }
                    distance = dist;
                    norm.x = w._a;
                    norm.y = w._b;
                    texture_id = w._texture_id;
                }
            }
        }
    }

    return distance * cos(angle);
}

void Map::render(int screen_width, int screen_height)
{
    float angle_of_view = M_PI / 3.0; // 60 degrees
    float step = angle_of_view / (float)screen_width;
    int col = 0;
    int line_count = 0;
    int texture_id, old_texture_id = -1;
    Point vertices[1500];
    Point line[2];

    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    for(float angle = -angle_of_view/2; angle < angle_of_view/2; angle+=step) {
        Normal norm;
        float pos;
        float dist = findNearestIntersectionPoint(angle, norm, pos, texture_id);
        if(!std::isnan(dist)) {
            int line_height = screen_height / (dist + 0.1);
            vertices[line_count*2] = {  col, (screen_height - line_height) / 2, norm.x, norm.y, pos * 2, 0.0, dist };
            vertices[line_count*2 + 1] = {  col, screen_height - (screen_height - line_height) / 2, norm.x, norm.y, pos * 2, 2.0, dist };
            if( (old_texture_id != -1) && (old_texture_id != texture_id) ) {
                _renderer.Prepare((void*)vertices, line_count * sizeof(Point) * 2);
                _renderer.useTexture(old_texture_id);
                _renderer.Render();
                vertices[0] = vertices[line_count*2];
                vertices[1] = vertices[line_count*2 + 1];
                line_count = 0;
            }
            line_count++;
            old_texture_id = texture_id;
        }
        col++;
    }

    if(line_count > 0) {
        _renderer.Prepare((void*)vertices, line_count * sizeof(Point) * 2);
        _renderer.useTexture(old_texture_id);
        _renderer.Render();
    }
}

void Map::addWall(const Wall &wall)
{
    _walls.emplace_back(wall);
}
