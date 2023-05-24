#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <string>
#include <memory>
#include <map>
#include <glad/glad.h>

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/Image.hpp>

namespace boost::json {
    class value;
}

struct __attribute__((__packed__)) Point {
    int _x,_y;
    float _nx,_ny;
    float _u,_v;
    float _dist;
};

class Renderer
{
public:
    Renderer();
    void Render();
    void Prepare(void *data, int size);
    void addTexture(int texture_id, const std::string &filename);
    void useTexture(int texture_id);
    void loadShaders(const std::string &vertex_shader, const std::string &fragment_shader);
    void loadShaders(const boost::json::value &json_object);

private:
    bool buildShader(const std::string &src, unsigned int &shader_id, unsigned int shader_type);
    std::string getShaderStr(const boost::json::value &v);

private:
    unsigned int _shaderProgram;
    unsigned int _vertexShader;
    unsigned int _fragmentShader;
    unsigned int _myTexture;
    unsigned int _VAO, _VBO;
    Point *_lines;
    int _bytesInBuffer = 0;
    std::map<int, unsigned int> _textures;
    friend class Map;
};

#endif
