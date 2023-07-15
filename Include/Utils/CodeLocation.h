#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define FILE_LINE std::string(__FILE__) + "(" + std::string(TOSTRING(__LINE__)) + ")"