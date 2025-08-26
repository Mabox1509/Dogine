#pragma once

//[INCLUDES]
#include "./dogine.hpp"
#include <string>
#include <memory>

//[NAMESPACE]
namespace Dogine
{
    namespace Resources
    {
        //[FUNCTIONS]
        void Load();
        std::shared_ptr<Texture> GetTexture(const std::string& _name);
        std::shared_ptr<Sprite> GetSprite(const std::string& _name);
        std::shared_ptr<Shader> GetShader(const std::string& _name);
        std::shared_ptr<Audio> GetAudio(const std::string& _name);
        std::shared_ptr<Mesh> GetMesh(const std::string& _name);
        std::string GetString(const std::string& _name);
        std::vector<char> GetBin(const std::string& _name);
    }
}

