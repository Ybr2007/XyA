#include <iostream>
#include <XyA.hpp>


using namespace XyA;

int main()
{
    std::ifstream f("Y://C++//XyA//source.xya", std::ios::in);  

    if(!f.is_open())
    {
        printf("Can't open the source file");
        exit(-1);
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

