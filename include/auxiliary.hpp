#ifndef AUXILIARY_HPP
#define AUXILIARY_HPP

#include <exception>
#include <iostream>
#include <boost/json.hpp>

class JSONReader
{
public:
    static JSONReader& getInstance();
    bool read(const std::string &filename);
    template<class Shader> void injectShader(Shader &shader) {
       try {
           auto vertex_shader = getStrFromArray(_jsonData.as_object().at("vertex_shader"));
           auto fragment_shader = getStrFromArray(_jsonData.as_object().at("fragment_shader"));
           shader.loadShaders(vertex_shader, fragment_shader);
       } catch(std::exception &e) {
           std::cerr << __FUNCTION__ << ": " << e.what() << std::endl;
           exit(1);
       }
    }

    template<class Renderer> void injectTextures(Renderer &renderer) {
        try {
            auto textures = _jsonData.as_object().at("textures").as_array();
            for(auto &t : textures) {
                renderer.addTexture(t.at(0).as_int64(), t.at(1).as_string().c_str());
            }
        } catch(std::exception &e) {
            std::cerr << __FUNCTION__ << ": " << e.what() << std::endl;
            exit(1);
        }
    }

    template<class Map> void injectWalls(Map &map) {
        try {
            auto walls = _jsonData.as_object().at("walls").as_array();
            for(auto &w : walls) {
                map.addWall({(float)w.as_array().at(0).as_double(),(float)w.as_array().at(1).as_double(),
                        (float)w.as_array().at(2).as_double(), (float)w.as_array().at(3).as_double(), (int)w.as_array().at(4).as_int64()});
            }
        } catch(std::exception &e) {
            std::cerr << __FUNCTION__ << ": " << e.what() << std::endl;
            exit(1);
        }
    }

    JSONReader(const JSONReader&) = delete;
    void operator=(const JSONReader&) = delete;

private:
    std::string getStrFromArray(const boost::json::value &v);
    JSONReader() {}

private:
    boost::json::value _jsonData;
};

#endif
