#ifndef _PROJECT_H
#define _PROJECT_H
//[INCLUDES]
#include <string>
#include <map>


//[NAMESPACE]
namespace Project 
{
    //[TYPES]
    enum class OsTarget
    {
        Windows,
        Linux
    };
    enum class BuildType
    {
        Debug,
        Relase
    };

    //[VARIABLES]
    extern std::string executable_name;
    extern OsTarget os_target;
    extern BuildType build_type;
    extern std::map<std::string, std::string> defines;


    //[FUNCTIONS]
    void Create(const std::string& _name);

    void Load();

    void Compile();
    void Run();

    void Clean();
}
#endif //_PROJECT_H