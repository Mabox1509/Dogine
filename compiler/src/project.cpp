//[INCLUDES]
#include "../inc/project.h"
#include <iostream>

#include "../inc/utils/filesys.h"
#include "../inc/utils/string_utils.h"

#include "../inc/systems/dogres.h"

#include "../inc/json.hpp"
#include "../inc/embed.h"



//[CONSTS]
const char* default_main = "//[INCLUDES]\n"
                            "#include <iostream>\n"
                            "//#include <dogine>\n\n\n"
                            "//[MAIN]\n"
                            "int main(int argc, char* argv[])\n"
                            "{\n"
                            "\tstd::cout << \"Hello world\" << std::endl;\n"
                            "\treturn 0;\n"
                            "}\n\0";

const char* default_project = "{\n"
                              "\t\"executable_name\" : \"My game\",\n"
                              "\t\"target\" : \"linux\",\n"
                              "\t\"mode\" : \"debug\",\n\n"

                              "\t\"defines\" :\n"
                              "\t{\n"
                              "\t \n"
                              "\t}\n"
                              "\t \n"


                              "}\n\0";
                            
//[FUNCTIONS]
std::string GetCompiler()
{
    switch (Project::os_target)
    {
        case Project::OsTarget::Windows:
#if defined(__linux__) || defined(__APPLE__)
            // En Linux/Mac compilamos para Windows con un cross-compiler
            return "x86_64-w64-mingw32-g++";
#else
            // En Windows, compilador normal
            return "g++";
#endif
        case Project::OsTarget::Linux:
            return "g++";
        default:
            return "g++"; // fallback
    }
}

std::string GetOs()
{
    return Project::os_target == Project::OsTarget::Windows ? "windows" : "linux";
}
std::string GetBuildType()
{
    return Project::build_type == Project::BuildType::Debug ? "debug" : "relase";
}

std::string GetDirectory(const std::string& path)
{
    size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos)
        return ""; // no directory part
    return path.substr(0, pos);
}
void CreateDirectories(const std::string& _fullpath)
{
    //Split
    auto _names = StringUtils::Split(_fullpath, '/');

    //Create paths
    std::string _path = "";
    for(size_t _i = 0;_i < _names.size();_i++)
    {
        _path += _names[_i];

        if (!FileSys::DirectoryExists(_path))
            FileSys::CreateDirectory(_path);

        _path += "/";
    }
}


void CreateOrRegenerateLibs()
{
    //Main path
    if(!FileSys::DirectoryExists("libs"))
        CreateDirectories("libs");



    //Copy files
    for (const auto& [path, info] : embed_files)
    {
        auto _subdir = GetDirectory(path);
        std::string _spath = "libs/" + path;

        if(FileSys::FileExists(_spath))
            continue;

        if(!FileSys::DirectoryExists("libs/" + _subdir))
            CreateDirectories("libs/" + _subdir);

        FileSys::WriteBinary(_spath, std::vector<char>(info.begin(), info.end()));
    }
}

bool CompileAssets()
{
    bool _change = false;

    std::cout << "Compiling assets..." << std::endl;
    auto _assets_paths = FileSys::ListFiles("assets", true);
    for (size_t _i = 0;_i < _assets_paths.size();_i++)
    {
        //GET PATHS
        const std::string& _path = _assets_paths[_i];
        const std::string& _rel_path = StringUtils::Replace(_path, "assets/", "");

        if(StringUtils::EndsWith(_rel_path, ".meta")) continue;
            

        //GET TYPE
        auto _type = Dogres::DefineType(_rel_path);
        auto _ext = Dogres::types_ext[(size_t)_type];

        //REPLACE EXTENSION
        std::string _out_path = _rel_path;
        size_t _dot = _out_path.find_last_of('.');
        if (_dot != std::string::npos)
            _out_path = _out_path.substr(0, _dot) + "." + _ext;
        else
            _out_path += "." + _ext;

            

        //CHECK IF CACHE EXIST
        _out_path = "cache/assets/" + _out_path;
        if(FileSys::FileExists(_out_path))
        {
            //If exist, then compare times
            auto _rawtime = FileSys::GetLastWriteTime(_path);
            auto _cachetime = FileSys::GetLastWriteTime(_out_path);

            if (_cachetime >= _rawtime)
            {
                std::string _meta_path = _path + ".meta";
                if(FileSys::FileExists(_meta_path))
                {
                    auto _metatime = FileSys::GetLastWriteTime(_meta_path);

                    if(_cachetime >= _metatime)
                    {
                        std::cout << "Skiping asset: " << _rel_path << std::endl;
                        continue;
                    }
                }
                else
                {
                    std::cout << "Skiping asset: " << _rel_path << std::endl;
                continue;
                }
            }
        }
        std::cout << "Compiling asset: " << _rel_path << std::endl;
        _change = true;
            
        //CREATE SUB FOLDERS
        std::string _out_dir = GetDirectory(_out_path);
        CreateDirectories(_out_dir);


        //PROCCESS DATA
        switch (_type)
        {
            case Dogres::AssetType::Texture:
            Dogres::ProcessTexture(_path,_out_path);
        break;
            
            case Dogres::AssetType::Audio :
            Dogres::ProcessAudio(_path,_out_path);
        break;

            case Dogres::AssetType::Model :
            Dogres::ProcessModel(_path,_out_path);
        break;

            case Dogres::AssetType::Text:
            Dogres::ProcessText(_path,_out_path);
        break;
        
            case Dogres::AssetType::Bin:
            Dogres::ProcessBin(_path,_out_path);
        break;
        }
    }
    std::cout << " " << std::endl;

    return _change;
}
void CompileScripts()
{
    std::cout << "Compiling program..." << std::endl;


    //PATH
    std::string _basepath = "cache/build/" + GetOs() + "/" + GetBuildType();


    //Flags
    std::string _optflag = (Project::build_type == Project::BuildType::Debug) ? "-O0 -g" : "-O3";
    std::string _extra_args = "-Wall -std=c++20"; // aquí puedes añadir más args
    _extra_args += " -Ilibs/include";

    for (const auto& [key, value] : Project::defines)
    {
        _extra_args += " -D" + key + "=" + value;
    }
    if (Project::build_type == Project::BuildType::Debug)
        _extra_args += " -DIS_DEBUG";
    else
        _extra_args += " -DIS_RELEASE";


    auto _code_paths = FileSys::ListFiles("code/src", true);
    for (size_t _i = 0; _i < _code_paths.size(); _i++)
    {
        const std::string& _path = _code_paths[_i];
        const std::string& _rel_path = StringUtils::Replace(_path, "code/src/", "");

        if (!StringUtils::EndsWith(_rel_path, ".cpp")) 
            continue;


            
        //CHECK IF CACHE EXIST
        std::string _out_path = _basepath + "/" + StringUtils::Replace(_rel_path, ".cpp", ".o");
        auto _outdir = GetDirectory(_out_path);
        CreateDirectories(_outdir);

        if(FileSys::FileExists(_out_path))
        {
            //If exist, then compare times
            auto _rawtime = FileSys::GetLastWriteTime(_path);
            auto _cachetime = FileSys::GetLastWriteTime(_out_path);

            if (_cachetime >= _rawtime)
            {
                std::cout << "Skiping script: " << _rel_path << std::endl;
                continue;
            }
        }
        std::cout << "Compiling script: " << _rel_path << std::endl;
        
        //CREATE SUB FOLDERS
        const auto& _folders = StringUtils::Split(_out_path, '/');
        std::string _current_path;
        for (size_t i = 0; i < _folders.size() - 1; ++i) // exclude filename
        {
            _current_path += _folders[i] + "/";
            if (!FileSys::DirectoryExists(_current_path))
            {
                FileSys::CreateDirectory(_current_path);
            }
        }
        
        //COMPILE
        std::string _cmd = GetCompiler() + " -c " + _optflag + " " + _extra_args +
                   " \"" + _path + "\" -o \"" + _out_path + "\"";
        std::cout << _cmd << std::endl;
        int _retcomp = std::system(_cmd.c_str());
        if (_retcomp != 0)
        {
            std::cerr << "Failed compiling " << _rel_path << std::endl;
            exit(1);
        }
    }
    std::cout << " " << std::endl;
}
void BuildAssetBundle(const std::string& _out)
{
    std::cout << "Creating asset bundle..." << std::endl;

    //Get assets cache
    auto _assets_paths = FileSys::ListFiles("cache/assets", true);
    std::vector<std::string> _rel_paths;
    _rel_paths.reserve(_assets_paths.size());
    
    //Calculate file size
    size_t _header_size = 8; //Number of entrys && header size
    size_t _data_size = 0;
    for(size_t _i = 0;_i < _assets_paths.size();_i++)
    {
        //Path
        auto _path = _assets_paths[_i];
        auto _rel_path = StringUtils::Replace(_path, "cache/assets/", "");
        _rel_paths.push_back(_rel_path);

        //Size in data
        _header_size += _rel_path.size() + 1;
        _header_size += sizeof(size_t) * 2; //File position & size


        //Size in data
        _data_size += FileSys::GetFileSize(_path);
    }

    //Generate macro file
    size_t _final_size = _header_size + _data_size;
    char* _buffer = new char[_final_size];
    std::memset(_buffer, 0x00, _final_size);

    size_t _seekh, _seekb;
    _seekh = 8;
    _seekb = _header_size;

    size_t _asset_num = _assets_paths.size();

    std::memcpy(_buffer, &_header_size, 4);
    std::memcpy(_buffer+4, &_asset_num, 4);

    for(size_t _i = 0;_i < _assets_paths.size();_i++)
    {
        //Path
        auto _path = _assets_paths[_i];
        auto _rel_path = _rel_paths[_i];

        auto _file = FileSys::ReadBinary(_path);
        size_t _filesize = _file.size();

        //Write path
        std::memcpy(_buffer + _seekh, _rel_path.c_str(), _rel_path.size()+1); _seekh += (_rel_path.size()+1);
        std::memcpy(_buffer + _seekh, &_seekb, sizeof(size_t));  _seekh += sizeof(size_t);
        std::memcpy(_buffer + _seekh, &_filesize, sizeof(size_t));  _seekh += sizeof(size_t);

        //Write fille
        std::memcpy(_buffer + _seekb, _file.data(), _filesize);  _seekb += _filesize;
    }

    FileSys::WriteBinary(_out, std::vector<char>(_buffer, _buffer + _final_size));
    delete[] _buffer;

    std::cout << " " << std::endl;
}
void BuildExecutable()
{
    std::cout << "Generating executable..." << std::endl;

    //Create path
    auto _os = GetOs();
    auto _btype = GetBuildType();
    std::string _basepath = "build/" + _os + "/" + _btype;
    CreateDirectories(_basepath);

    //Get object files
    auto _object_paths = FileSys::ListFiles("cache/build/" + _os + "/" + _btype, true);

    //Flags
    std::string _inputflags = "";
    for (size_t _i = 0; _i < _object_paths.size(); _i++)
    {
        _inputflags += _object_paths[_i] + " ";
    }

    std::string _extra_args = "-o " + _basepath + "/" + Project::executable_name;
    if(Project::os_target == Project::OsTarget::Windows)
        _extra_args += ".exe";

    if(Project::os_target == Project::OsTarget::Windows)
    {

    }
    else if(Project::os_target == Project::OsTarget::Linux)
    {
        _extra_args += " libs/linux/libdogine.a";
        _extra_args += " libs/linux/libportaudio.a";
    }
    _extra_args += " -lglfw -lGL -lm -ldl -lz"; //  -lglfw -lGL -lm -ldl -lz
    _extra_args += " -lrt -lasound -pthread";

    //Copy resources
    std::string _respath = _basepath + "/assets.res";
    if(FileSys::FileExists(_respath))
        FileSys::DeleteFile(_respath);
    
    FileSys::CopyFile("cache/assets.res", _respath);


    //Link
    std::string _cmd = GetCompiler() + " " + _inputflags + _extra_args;
    std::cout << _cmd << std::endl;
    int _retcomp = std::system(_cmd.c_str());
    if (_retcomp != 0)
    {
        std::cerr << "Failed linking " << std::endl;
        exit(1);
    }


    std::cout << " " << std::endl;
}

//[NAMESPACE]
namespace Project 
{
    //[VARIABLES]
    std::string executable_name;
    OsTarget os_target;
    BuildType build_type;
    std::map<std::string, std::string> defines;
    

    //[FUNCTIONS]
    void Create(const std::string& _name)
    {
        //Create directorys
        FileSys::CreateDirectory(_name);

        FileSys::CreateDirectory(_name + "/assets");
            FileSys::CreateDirectory(_name + "/assets/textures");
            FileSys::CreateDirectory(_name + "/assets/shaders");
            FileSys::CreateDirectory(_name + "/assets/models");
            FileSys::CreateDirectory(_name + "/assets/audios");
            FileSys::CreateDirectory(_name + "/assets/texts");
            FileSys::CreateDirectory(_name + "/assets/bins");

        FileSys::CreateDirectory(_name + "/code");
            FileSys::CreateDirectory(_name + "/code/src");
            FileSys::CreateDirectory(_name + "/code/inc");

        FileSys::CreateDirectory(_name + "/build");

        FileSys::CreateDirectory(_name + "/cache");
                FileSys::CreateDirectory(_name + "/cache/build");
                FileSys::CreateDirectory(_name + "/cache/assets");
        
        
        //Create files
        FileSys::WriteString(_name + "/project.json", std::string(default_project));
        FileSys::WriteString(_name + "/code/src/main.cpp", std::string(default_main));
        
        FileSys::CreateDirectory(_name + "/libs");
        

        std::cout << "Project \"" << _name << "\" created" << std::endl;
    }

    void Load()
    {
        //Check project file
        if(!FileSys::FileExists("project.json"))
        {
            std::cout << "Project file not found" << std::endl;
            exit(1);
        }
    
        //Load project file
        auto _file = FileSys::ReadString("project.json");

        //Parse
        nlohmann::json _config;
        try
        {
            _config = nlohmann::json::parse(_file);
        } 
        catch (const std::exception& e)
        {
            std::cerr << "Failed to parse project.json: " << e.what() << std::endl;
            exit(1);
        }
        
        //Read data
        if (!_config.contains("executable_name") || !_config["executable_name"].is_string())
        {
            std::cerr << "Error: Missing or invalid \"executable_name\" in project.json\n";
            exit(1);
        }
        executable_name = _config["executable_name"];

        // ---- Target Platform ----
        if (!_config.contains("target") || !_config["target"].is_string())
        {
            std::cerr << "Error: Missing or invalid \"target\" in project.json\n";
            exit(1);
        }
        std::string target = _config["target"];
        if (target == "windows")
            os_target = OsTarget::Windows;
        else if (target == "linux")
            os_target = OsTarget::Linux;
        else
        {
            std::cerr << "Error: Unknown target \"" << target << "\" (expected: windows or linux)\n";
            exit(1);
        }

        // ---- Build Mode ----
        if (!_config.contains("mode") || !_config["mode"].is_string())
        {
            std::cerr << "Error: Missing or invalid \"mode\" in project.json\n";
            exit(1);
        }
        std::string mode = _config["mode"];
        if (mode == "debug")
            build_type = BuildType::Debug;
        else if (mode == "relase") // <- cuidado con typo, probablemente quisiste "release"
            build_type = BuildType::Relase;
        else
        {
            std::cerr << "Error: Unknown mode \"" << mode << "\" (expected: debug or relase)\n";
            exit(1);
        }

        // ---- Defines ----
        defines.clear();
        if (_config.contains("defines"))
        {
            if (!_config["defines"].is_object())
            {
                std::cerr << "Error: \"defines\" must be a JSON object.\n";
                exit(1);
            }

            for (auto& [key, val] : _config["defines"].items())
            {
                if (!val.is_string())
                {
                    std::cerr << "Warning: define \"" << key << "\" has non-string value, skipping.\n";
                    continue;
                }
                defines[key] = val;
            }
        }
    }
    void Compile()
    {
        std::cout << "Starting compilation..." << std::endl << std::endl;
        CreateOrRegenerateLibs();

        //COMPILATION
        bool _assets = CompileAssets();   //ASSETS
        CompileScripts(); //SCRIPTS (.cpp)
        
        std::string _assets_out = "cache/assets.res";
        if(_assets || !FileSys::FileExists(_assets_out))
            BuildAssetBundle(_assets_out);

        //GENERATE EXECUTABLE
        BuildExecutable();
    }
    void Run()
    {
        auto _os = GetOs();
        auto _btype = GetBuildType();
        std::string _exec = "build/" + _os + "/" + _btype + "/" + Project::executable_name;

        bool isWindowsTarget = Project::os_target == Project::OsTarget::Windows;
        if (isWindowsTarget)
            _exec += ".exe";

        std::string cmd;

    #if defined(__linux__)
        if (isWindowsTarget)
            cmd = "wine \"" + _exec + "\"";
        else
            cmd = "./\"" + _exec + "\"";
    #elif defined(_WIN32)
        cmd = "\"" + _exec + "\"";
    #else
        cmd = "\"" + _exec + "\""; // fallback
    #endif

        std::cout << "Running: " << cmd << std::endl;

        int ret = std::system(cmd.c_str());
        if (ret != 0)
            std::cerr << "Execution failed with code " << ret << std::endl;
    }
    void Clean()
    {
        if(FileSys::DirectoryExists("cache"))
            FileSys::DeleteDirectory("cache", true);

        if(FileSys::DirectoryExists("libs"))
            FileSys::DeleteDirectory("libs", true);

        if(FileSys::DirectoryExists("build"))
            FileSys::DeleteDirectory("build", true);
    }
}