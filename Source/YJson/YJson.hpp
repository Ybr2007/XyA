#pragma once
#include <fstream>
#include <YJson/Object.hpp>
#include <YJson/Parser.hpp>


namespace YJson
{
    Object* loads(std::string_view jsonString)
    {
        Parser parser;
        return parser.parse(jsonString);
    }

    Object* load(const std::string& filePath)
    {
        std::ifstream f(filePath, std::ios::in);  

        if(!f.is_open())
        {
            throw Exception("Failed to open file.");
        }

        std::stringstream buffer;  
        buffer << f.rdbuf();  
        std::string jsonString = buffer.str();

        f.close();

        return loads(jsonString);
    }

    Object* load(std::ifstream f)
    {
        std::stringstream buffer;  
        buffer << f.rdbuf();  
        std::string jsonString = buffer.str();

        return loads(jsonString);
    }

    std::string dumps(const Object* obj, std::size_t indent=0,
                        bool removeFollowingZerosForNumber=true)
    {
        return obj->toString(indent, removeFollowingZerosForNumber);
    }

    void dump(std::string filePath, const Object* obj, std::size_t indent=0, 
                        bool removeFollowingZerosForNumber=true)
    {
        std::ofstream f(filePath, std::ios::out);
        f << dumps(obj, indent, removeFollowingZerosForNumber);
        f.close();
    }
    
    void dump(std::ofstream f, const Object* obj, std::size_t indent=0,
                        bool removeFollowingZerosForNumber=true)
    {
        f << dumps(obj, indent, removeFollowingZerosForNumber);
    }
}