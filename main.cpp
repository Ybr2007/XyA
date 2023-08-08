#include <iostream>
#include <fstream>
#include <XyA.hpp>


using namespace XyA;

int main(int arg_num, char* args[])
{
    // if (arg_num != 2)
    // {
    //     printf("\033[1;31mError: Expected source file path\033[0m\n");
    //     return -1;
    // }
    // char* source_file_name = args[1];
    const char* source_file_name = "Y://C++//XyA//source.xya";

    std::ifstream f(source_file_name, std::ios::in);  

    if(!f.is_open())
    {
        printf("\033[1;31mError: Can not open the source file '%s'\033[0m\n", source_file_name);
        return -1;
    }

    std::stringstream buffer;  
    buffer << f.rdbuf();  
    std::string source = buffer.str();

    f.close();

    Core xya_core;
    try
    {
        xya_core.execute(source);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    catch(...)
    {
        std::cerr << "Error\n";
    }

}