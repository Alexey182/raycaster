#include "renderer.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cmath>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <boost/json.hpp>

#include "scene.hpp"

namespace json = boost::json;

json::value parseJSON(const std::string &json_filename)
{
    char buf[1024];
    json::stream_parser map_parser;
    json::error_code ec;
    std::cout << std::filesystem::canonical(json_filename).parent_path() << std::endl;
    std::ifstream map_stream(json_filename, std::ifstream::binary);

    while(!map_stream.eof()) {
        map_stream.read(buf, sizeof(buf));
        map_parser.write(buf, map_stream.gcount(), ec);
    }

    map_stream.close();
    map_parser.finish(ec);

    if(ec) {
        std::cout << "Unable to parse JSON file " << json_filename << std::endl;
        return nullptr;
    }

    return map_parser.release();
}

std::string getShaderStr(const json::value &v)
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

int main(int argc, char **argv)
{
    if(argc < 2) {
        std::cerr << "Missing map file" << std::endl;
        return 1;
    }
    std::cout << "Map file: " << argv[1] << std::endl;
    auto const json_value = parseJSON(argv[1]);
    std::cout << getShaderStr(json_value.as_object().at("vertex_shader")) << std::endl;
    Camera cam;
    int screen_width = 640;
    int screen_height = 480;
    sf::Window wnd(sf::VideoMode(screen_width, screen_height), "RayCaster", sf::Style::Titlebar | sf::Style::Close, sf::ContextSettings(32));
    wnd.setVerticalSyncEnabled(true);
    wnd.setActive(true);
    gladLoadGL();

    Renderer r;
    r.loadShaders(json_value);
    Map m(cam, r);

    std::cout << "GL Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "GL Version: " << glGetString(GL_VERSION) << std::endl;

    bool running = true;
    float rotation_step = 0.0;
    float move_step = 0.0;
    float sidemove_step = 0.0;

    
    while(running) {
        sf::Event event;

        while(wnd.pollEvent(event)) {
            if(event.type == sf::Event::Closed)
            {
                running = false;
                std::cout << "Closed event" << std::endl;
            } else if(event.type == sf::Event::Resized) {
                screen_width = event.size.width;
                screen_height = event.size.height;
                glViewport(0,0, screen_width, screen_height);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(0, screen_width, 0, screen_height, -1, 1);
                glMatrixMode(GL_MODELVIEW);
                std::cout << "Resize event" << std::endl;
            } else if(event.type == sf::Event::KeyPressed) {
                switch(event.key.code) {
                    case sf::Keyboard::Left:
                        rotation_step = -0.05;
                        break;
                    case sf::Keyboard::Right:
                        rotation_step = 0.05;
                        break;
                    case sf::Keyboard::Up:
                    case sf::Keyboard::W:
                        move_step = 0.05;
                        break;
                    case sf::Keyboard::Down:
                    case sf::Keyboard::S:
                        move_step = -0.05;
                        break;
                    case sf::Keyboard::A:
                        sidemove_step = -0.05;
                        break;
                    case sf::Keyboard::D:
                        sidemove_step = 0.05;
                        break;
                }
            } else if(event.type == sf::Event::KeyReleased) {
                switch(event.key.code) {
                    case sf::Keyboard::Left:
                    case sf::Keyboard::Right:
                        rotation_step = 0.0;
                        break;
                    case sf::Keyboard::Up:
                    case sf::Keyboard::Down:
                    case sf::Keyboard::W:
                    case sf::Keyboard::S:
                        move_step = 0.0;
                        break;
                    case sf::Keyboard::A:
                    case sf::Keyboard::D:
                        sidemove_step = 0.0;
                        break;
                }
            }
        }

        if(rotation_step != 0.0) {
            cam.Rotate(rotation_step);
        }

        if(move_step != 0.0) {
            cam.Move(move_step);
        }

        if(sidemove_step != 0.0) {
            cam.SideMove(sidemove_step);
        }
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glColor3f(0.6,0.2,0.0);
        m.render(screen_width, screen_height);
        //r.Render();
        wnd.display();
    }

    return 0;
}
