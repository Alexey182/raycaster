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
           std::cerr << e.what() << std::endl;
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
