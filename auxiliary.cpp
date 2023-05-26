#include <auxiliary.hpp>
#include <fstream>

namespace json = boost::json;

bool JSONReader::read(const std::string &filename)
{
    char buf[1024];
    json::stream_parser map_parser;
    json::error_code ec;
    std::ifstream map_stream(filename, std::ifstream::binary);

    while(!map_stream.eof()) {
        map_stream.read(buf, sizeof(buf));
        map_parser.write(buf, map_stream.gcount(), ec);
    }

    map_stream.close();
    map_parser.finish(ec);

    if(ec) {
        std::cout << "Unable to parse JSON file " << filename << std::endl;
        return false;
    }

    _jsonData = map_parser.release();
    return true;
}

std::string JSONReader::getStrFromArray(const boost::json::value &v)
{
    const auto lines = v.as_array();
    std::string result;

    for(auto &l : lines) {
        result += l.as_string();
    }

    return result;
}

JSONReader& JSONReader::getInstance()
{
    static JSONReader inst;
    return inst;
}
