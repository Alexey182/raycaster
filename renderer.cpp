#include "renderer.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <boost/json.hpp>


void ShaderProgram::buildShader(const std::string &src, unsigned int &shader_id, unsigned int shader_type)
{
    shader_id = glCreateShader(shader_type);
    const char *sources[1];
    int lengths[1];
    char error[1024];
    int success = 0;
    sources[0] = src.c_str();
    lengths[0] = src.length();
    glShaderSource(shader_id, 1, sources, lengths);
    glCompileShader(shader_id);
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(shader_id, sizeof(error), nullptr, error);
        throw ShaderException(std::string("Build error: ") + error);
    }
}

void ShaderProgram::loadShaders(const std::string &vertex_shader, const std::string &fragment_shader)
{
    try {
        buildShader(vertex_shader, _vertexShader, GL_VERTEX_SHADER);
        buildShader(fragment_shader, _fragmentShader, GL_FRAGMENT_SHADER);

        int success = 0;
        char error[1024];
        _shaderProgram = glCreateProgram();
        glAttachShader(_shaderProgram, _vertexShader);
        glAttachShader(_shaderProgram, _fragmentShader);
        glLinkProgram(_shaderProgram);
        glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(_shaderProgram, GL_LINK_STATUS, nullptr, error);
            throw ShaderException(std::string("Link error: ") + error);
        }
    } catch(ShaderException &se) {
        std::cerr << se.what() << std::endl;
        exit(1);
    }
}

void ShaderProgram::use()
{
    glUseProgram(_shaderProgram);
}

Renderer::Renderer()
{
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glVertexAttribPointer(0,2, GL_INT, GL_FALSE, sizeof(Point), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)&(((Point*)0)->_nx));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)&(((Point*)0)->_u));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3,1, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)&(((Point*)0)->_dist));
    glEnableVertexAttribArray(3);

    addTexture(0, "default.png");
    addTexture(1, "texture.jpg");
    addTexture(2, "texture1.jpg");
    addTexture(3, "texture2.jpg");
}

void Renderer::Prepare(void *data, int size)
{
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    _bytesInBuffer = size;
}

void Renderer::addTexture(int texture_id, const std::string &filename)
{
    unsigned int tex;
    sf::Image texImage;
    glGenTextures(1,  &tex);
    _textures[texture_id] = tex;
    glBindTexture(GL_TEXTURE_2D, tex);
    texImage.loadFromFile(filename);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texImage.getSize().x, texImage.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage.getPixelsPtr());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Renderer::useTexture(int texture_id)
{ 
    _shaderProgram.use();
    glBindTexture(GL_TEXTURE_2D, _textures[texture_id]); 
    glUniform1i(glGetUniformLocation(_shaderProgram.getHandle(), "myTexture"), 0);
}

void Renderer::Render()
{
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBindVertexArray(_VAO);
    glDrawArrays(GL_LINES, 0, _bytesInBuffer / sizeof(Point));
    _bytesInBuffer = 0;
}

void Renderer::loadShaders(const std::string &vertex_shader, const std::string &fragment_shader)
{
    _shaderProgram.loadShaders(vertex_shader, fragment_shader);
}
void Renderer::loadShaders(const boost::json::value &json_object)
{
    if(!json_object.is_object()) {
        return;
    }

    const auto &obj = json_object.as_object();

    try {
        auto vertex_shader =  getShaderStr(obj.at("vertex_shader"));
        auto fragment_shader = getShaderStr(obj.at("fragment_shader"));

        loadShaders(vertex_shader, fragment_shader);
    } catch(std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
}

std::string Renderer::getShaderStr(const boost::json::value &v)
{
    if(!v.is_array()) {
        return nullptr;
    }

    const auto lines = v.as_array();

    std::string result;

    for(auto &l : lines) {
        result += l.as_string();
    }

    return result;
}
