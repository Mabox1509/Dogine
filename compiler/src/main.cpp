//[INCLUDES]
#include <iostream>
#include <vector>

#include "../inc/utils/filesys.h"
#include "../inc/project.h"

//[CONSTS]
const char* help_message = "Usage: doginec [command] <args...>\n"
                            "  new <name>     Creates a new project\n"
                            "  build          Compiles the project\n"
                            "  run            Compiles & run\n"
                            "  clear          Clear cache files\n";

//[MAIN]
int main(int argc, char* argv[])
{
    //CHECK ARGUMENTS
    if (argc < 2)
    {
        std::cerr << "No arguments provided" << std::endl;
        return 1;
    }

    std::vector<std::string> _args(argv, argv + argc);
    const std::string& _command = _args[1];
    

    //PARSE & EXECUTE
    if(_command == "help")
    {
        std::cout << help_message << std::endl;
    }
    else if(_command == "new")
    {
        //Check arguments cound
        if (_args.size() < 3)
        {
            std::cerr << "Error: No project name provided.\n";
            std::cerr << "Usage: doginec new <name>\n";
            return 1;
        }

        //Check if directory don't exist
        const std::string& _name = _args[2];
        if (FileSys::DirectoryExists(_name))
        {
            std::cerr << "Error: Directory '" << _name << "' already exists.\n";
            return 1;
        }

        //Create project
        Project::Create(_name);
    }
    else if(_command == "build")
    {
        Project::Load();
        Project::Compile();
    }
    else if(_command == "run")
    {
        Project::Load();
        Project::Compile();
        Project::Run();
    }
    else if(_command == "clean")
    {
        Project::Load();
        Project::Clean();
    }
    else
    {
        std::cerr << "Invalid command" << std::endl;
        return 1;
    }
    

    return 0;
}
