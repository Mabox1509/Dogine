#ifndef _DOGRES_H
#define _DOGRES_H
//[INCLUDES]
#include <string>
#include <vector>
#include <array>

//[NAMESPACE]
namespace Dogres 
{
    //[TYPES]
    enum class AssetType
    {
        Texture,
        Audio,
        Model,
        Text,
        Bin,
    };
    extern const std::array<std::string, 5> types_ext;


    //[FUNCTIONS]
    AssetType DefineType(const std::string& _path);

    void ProcessTexture(const std::string& in_path, const std::string& out_path);
    void ProcessAudio(const std::string& in_path, const std::string& out_path);
    void ProcessModel(const std::string& in_path, const std::string& out_path);
    void ProcessText(const std::string& in_path, const std::string& out_path);
    void ProcessBin(const std::string& in_path, const std::string& out_path);

}
#endif //_DOGRES_H