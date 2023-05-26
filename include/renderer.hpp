#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <exception>
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

class Texture
{
public:
    Texture(std::string& filename);
private:
    unsigned int _texture;
};

class ShaderException : public std::exception
{
public:
    ShaderException(std::string msg) : _msg(msg) {}
    virtual const char* what() const noexcept override { return _msg.c_str(); }
private:
    std::string _msg;
};

class ShaderProgram
{
public:
    ShaderProgram() = default;
    ShaderProgram(ShaderProgram &&) = default;
    ShaderProgram& operator=(ShaderProgram &&) = default;
    void loadShaders(const std::string &vertex_shader, const std::string &fragment_shader);
    unsigned int getHandle() { return _shaderProgram; }
    void use();
    void dump() { std::cout << _vertexShader << ", " << _fragmentShader << ", " << _shaderProgram << std::endl; }
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

private:
    void buildShader(const std::string &src, unsigned int &shader_id, unsigned int shader_type);

private:
    unsigned int _vertexShader;
    unsigned int _fragmentShader;
    unsigned int _shaderProgram;
};

class Renderer
{
public:
    Renderer(ShaderProgram &sp);
    void Render();
    void Prepare(void *data, int size);
    void addTexture(int texture_id, const std::string &filename);
    void useTexture(int texture_id);

private:
    ShaderProgram _shaderProgram;
    unsigned int _myTexture;
    unsigned int _VAO, _VBO;
    Point *_lines;
    int _bytesInBuffer = 0;
    std::map<int, unsigned int> _textures;
    friend class Map;
};

#endif
