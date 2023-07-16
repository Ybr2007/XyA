#pragma once
#include <fstream>
#include <YJson/Object.h>


namespace YJson
{
    Object* loads(std::string_view jsonString);

    Object* load(const std::string& filePath);

    Object* load(std::ifstream f);

    std::string dumps(const Object* obj, std::size_t indent=0, bool removeFollowingZerosForNumber=true);

    void dump(std::string filePath, const Object* obj, std::size_t indent=0, bool removeFollowingZerosForNumber=true);
    
    void dump(std::ofstream f, const Object* obj, std::size_t indent=0, bool removeFollowingZerosForNumber=true);
}