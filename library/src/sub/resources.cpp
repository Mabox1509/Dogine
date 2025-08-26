//[INCLUDES]
#include <dogine/dogine.hpp>
#include <dogine/resources.hpp>
#include <dogine/log.hpp>
#include <dogine/string.hpp>
#include <dogine/compression.hpp>

#include <unistd.h>
#include <limits.h>

#include <filesystem>
#include <sys/stat.h>
#include <cstring>
#include <fstream>
#include <map>

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

//[PRIVATE TYPES]
typedef struct file_t
{
    uint64_t position;
    uint64_t size;
} file_t;

//[PRIVATE VARIABLES]
std::string resources_path;

std::unordered_map<std::string, file_t> files;
std::map<std::string, std::weak_ptr<Dogine::Texture>> textures;
std::map<std::string, std::weak_ptr<Dogine::Sprite>> sprites;
std::map<std::string, std::weak_ptr<Dogine::Shader>> shaders;
std::map<std::string, std::weak_ptr<Dogine::Audio>> audios;
std::map<std::string, std::weak_ptr<Dogine::Mesh>> mesh;
//std::map<std::string, std::weak_ptr<Dogine::Asset::Audio>> audio;

//[PRIVATE FUNCTIONS]
std::vector<char> GetRawData(const std::string& _path)
{
    // Verificar que el archivo exista en el map
    auto it = files.find(_path);
    if (it == files.end())
    {
        throw std::runtime_error("Asset not found: " + _path);
    }

    const file_t& entry = it->second;

    // Abrir el macro archivo
    std::ifstream _file(resources_path, std::ios::binary);
    if (!_file.is_open())
    {
        throw std::runtime_error("Unable to open resources file: " + resources_path);
    }

    // Reservar vector con tamaño exacto
    std::vector<char> data(entry.size);

    // Movernos a la posición y leer los bytes
    _file.seekg(entry.position, std::ios::beg);
    _file.read(data.data(), entry.size);

    // Verificar que se leyó todo
    if (!_file)
    {
        throw std::runtime_error("Failed reading asset data: " + _path);
    }

    _file.close();
    return data;
}

std::string GetExePath()
{
#if defined(_WIN32)
    char path[MAX_PATH];
    DWORD length = GetModuleFileNameA(NULL, path, MAX_PATH);
    if (length == 0) return ""; // Error
    std::string exePath(path, length);
    return exePath.substr(0, exePath.find_last_of("\\/"));
#else
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    if (count <= 0) return ""; // Error
    std::string exePath(path, count);
    return exePath.substr(0, exePath.find_last_of('/'));
#endif
}

//[NAMESPACE]
namespace Dogine
{
    namespace Resources
    {

        //[FUNCTIONS]
        void Load()
        {
            //Get resources path
            resources_path = GetExePath() + "/assets.res";

            //Open resources
            std::ifstream _file(resources_path, std::ios::binary);
            if (!_file.is_open())
            {
                Log::Message("Unable to find resources file");
                exit(1);
            }

            //Read header
            _file.seekg(0);
            
            uint32_t _hsize; //The header size (unused)
            uint32_t _ennum; //The number of entrys
            _file.read(reinterpret_cast<char*>(&_hsize), sizeof(_hsize));
            _file.read(reinterpret_cast<char*>(&_ennum), sizeof(_ennum));

            for(size_t _i = 0;_i < _ennum;_i++)
            {
                std::string _path;
                char _c;

                // Read until '\0'
                while (_file.get(_c) && _c != '\0')
                {
                    _path += _c;
                }

                uint64_t _position, _size;
                _file.read(reinterpret_cast<char*>(&_position), sizeof(_size));
                _file.read(reinterpret_cast<char*>(&_size), sizeof(_size));

                //Add 2 dictionary
                file_t _entry;
                _entry.position = _position;
                _entry.size = _size;

                files[_path] = _entry;
            }


            //Close
            _file.close();
        }
        std::shared_ptr<Texture> GetTexture(const std::string& _name)
        {
            //GET PATH
            std::string _path = "textures/" + _name + ".txc";

            //CHECK CACHE
            auto _cache = textures.find(_name);
            if(_cache != textures.end())
            {
                if(!_cache->second.expired())
                    return _cache->second.lock();
            }

            //CHECK FILE
            auto _file = files.find(_path);
            if (_file == files.end())
            {
                Log::Error("Texture not found: %s", _path.c_str());
                return nullptr;
            }

            //LOAD & DECOMPRESS
            auto _data = GetRawData(_path);
            auto _raw = Compression::Decompress(_data);

            

            //READ HEADER
            size_t _seek = 0;

            int _width, _height;
            uint8_t _warp, _bytedepth;
            bool _mipmap, _interpolation;
            uint32_t _colors_size;

            std::memcpy(&_width, _raw.data() + _seek, sizeof(int)); _seek += sizeof(int); //Read width
            std::memcpy(&_height, _raw.data() + _seek, sizeof(int)); _seek += sizeof(int); //Read height
            std::memcpy(&_warp, _raw.data() + _seek, sizeof(uint8_t)); _seek += sizeof(uint8_t); //Read warp
            std::memcpy(&_mipmap, _raw.data() + _seek, sizeof(bool)); _seek += sizeof(bool); //Read mipmap
            std::memcpy(&_interpolation, _raw.data() + _seek, sizeof(bool)); _seek += sizeof(bool); //Read interpolation

            std::memcpy(&_bytedepth, _raw.data() + _seek, sizeof(uint8_t)); _seek += sizeof(uint8_t); //Read byte depth
            std::memcpy(&_colors_size, _raw.data() + _seek, sizeof(uint32_t)); _seek += sizeof(uint32_t); //Read byte depth

            //GENERATE TEXTURE
            Dogine::Warpmode _wrp = (_warp == 0 ? Dogine::Warpmode::Clamp : (_warp == 1 ? Dogine::Warpmode::Repeat : Dogine::Warpmode::Mirror));
            std::shared_ptr<Texture> _tex = std::make_shared<Texture>(_width, _height, _wrp, _interpolation, _mipmap);

            //READ PALLETE
            std::unordered_map<uint32_t, color_t> _palette;
            for(uint32_t _i = 0;_i < _colors_size;_i++)
            {
                color_t _color;
                std::memcpy(&_color, _raw.data() + _seek, sizeof(color_t)); _seek += sizeof(color_t); //Read color
                _palette[_i] = _color;
            }


            //READ PIXELS
            for(int _y = 0;_y < _height;_y++)
            {
                for(int _x = 0;_x < _width;_x++)
                {
                    uint32_t _index = 0;
                    std::memcpy(&_index, _raw.data() + _seek, _bytedepth); _seek += _bytedepth; //Read index

                    auto it = _palette.find(_index);
                    if (it != _palette.end()) {
                        _tex->SetPixel(_x, _y, it->second);
                    } else {
                        Log::Error("Invalid palette index %u", _index);
                    }

                }
            }

            //APPLY
            _tex->Apply();
            textures[_name] = _tex;

            return _tex;
        }
        std::shared_ptr<Sprite> GetSprite(const std::string& _name)
        {
            //CHECK CACHE
            auto _cache = sprites.find(_name);
            if(_cache != sprites.end())
            {
                if(!_cache->second.expired())
                    return _cache->second.lock();
            }

            //GET PATH
            std::string _path = "sprites/" + _name + ".bnc";

            //LOAD
            auto _raw = GetRawData(_path);
            std::string _data (_raw.begin(), _raw.end());

            //PARSE
            auto _lines = Dogine::StringUtils::Split(_data, '\n');
            if(_lines.size() < 2)
            {
                Dogine::Log::Error("Invalid sprite file: %s", _path.c_str());
                return nullptr;
            }
            
            std::string _texture_name = _lines[0];
            auto _texture = GetTexture(_texture_name);
            if(!_texture) return nullptr;

            std::shared_ptr<Sprite> _sprite = std::make_shared<Sprite>(_texture);
            for(size_t _i = 1;_i < _lines.size();_i++)
            {
                auto _params = Dogine::StringUtils::Split(_lines[_i], ' ');
                if(_params.size() < 4) continue;

                int x = std::stoi(_params[0]);
                int y = std::stoi(_params[1]);
                int w = std::stoi(_params[2]);
                int h = std::stoi(_params[3]);

                glm::vec2 pivot(0.5f, 0.5f);
                float ppu = 100.0f;

                if(_params.size() >= 6)
                {
                    pivot.x = std::stof(_params[4]);
                    pivot.y = std::stof(_params[5]);
                }
                if(_params.size() >= 7)
                {
                    ppu = std::stof(_params[6]);
                }

                _sprite->AddFrame(x, y, w, h, pivot, ppu);
            }
            
            //APPLY
            sprites[_name] = _sprite;
            return _sprite;
        }
        std::shared_ptr<Shader> GetShader(const std::string& _name)
        {
            //CHECK CACHE
            auto _cache = shaders.find(_name);
            if(_cache != shaders.end())
            {
                if(!_cache->second.expired())
                    return _cache->second.lock();
            }

            //GET PATH
            std::string _path = "shaders/" + _name;

            //GET SOURCES
            std::string _vertsrc;
            std::string _fragsrc;
            try
            {
                auto _vertcomp = GetRawData(_path + "/vertex.txtc");
                auto _fragcomp = GetRawData(_path + "/fragment.txtc");

                auto _vertdat = Compression::Decompress(_vertcomp);
                auto _fragdat = Compression::Decompress(_fragcomp);

                _vertsrc = std::string(_vertdat.begin(), _vertdat.end());
                _fragsrc = std::string(_fragdat.begin(), _fragdat.end());
            }
            catch(const std::exception& e)
            {
                Dogine::Log::Error("Error loading shader: %s", e.what());
            }
        
            //CREATE SHADER
            std::shared_ptr<Shader> _shader = std::make_shared<Shader>(_vertsrc.c_str(), _fragsrc.c_str());

            //APPLY
            shaders[_name] = _shader;

            return _shader;
        }
        std::shared_ptr<Audio> GetAudio(const std::string& _name)
        {
            //GET PATH
            std::string _path = "audios/" + _name + ".snc";

            //CHECK CACHE
            auto _cache = audios.find(_name);
            if(_cache != audios.end())
            {
                if(!_cache->second.expired())
                    return _cache->second.lock();
            }

            //CHECK FILE
            auto _file = files.find(_path);
            if (_file == files.end())
            {
                Log::Error("Texture not found: %s", _path.c_str());
                return nullptr;
            }

            //LOAD
            auto _raw = GetRawData(_path);
            size_t _seek;

            //READ HEADER
            uint64_t _samples;
            uint32_t _freq;
            uint8_t _bitdepth;
            uint64_t _loop;
            double _basefreq;
            int _channels;

            std::memcpy(&_samples, _raw.data() + _seek, sizeof(_samples));  _seek += sizeof(_samples);
            std::memcpy(&_freq, _raw.data() + _seek, sizeof(_freq));    _seek += sizeof(_freq);
            std::memcpy(&_bitdepth, _raw.data() + _seek, sizeof(_bitdepth));    _seek += sizeof(_bitdepth);
            std::memcpy(&_loop, _raw.data() + _seek, sizeof(_loop));    _seek += sizeof(_loop);
            std::memcpy(&_basefreq, _raw.data() + _seek, sizeof(_basefreq));    _seek += sizeof(_basefreq);
            std::memcpy(&_channels, _raw.data() + _seek, sizeof(_channels));    _seek += sizeof(_channels);

            //READ SAMPLES
            size_t _buffersize = (_samples * (_bitdepth / 8)) * _channels;
            char* _samples_data = new char[_buffersize];
            _seek = 64;
            std::memcpy(_samples_data, _raw.data() + _seek, _buffersize); _seek += _buffersize;


            //CREATE AUDIO
            std::shared_ptr<Audio> _audio = std::make_shared<Audio>(_samples_data, _buffersize, _bitdepth, _freq, _channels);
            _audio->base_freq = _basefreq;
            _audio->SetLoopPoints(_loop);
            audios[_name] = _audio;
            _audio->name = _name;

            delete[] _samples_data;
            return _audio;
        }
        std::shared_ptr<Mesh> GetMesh(const std::string& _name)
        {
            Log::Warning("Mesh assets not implemented yet");
            Log::Error("Sorry for the inconvenience xD");
            return nullptr;
        }
        std::string GetString(const std::string& _name)
        {
            //GET PATH
            std::string _path = "texts/" + _name + ".txtc";

            //CHECK FILE
            auto _file = files.find(_path);
            if (_file == files.end())
            {
                Log::Error("Text not found: %s", _path.c_str());
                return "";
            }

            //LOAD & DECOMPRESS
            auto _data = GetRawData(_path);
            auto _raw = Compression::Decompress(_data);

            //RETURN
            return std::string(_raw.begin(), _raw.end());
        }
        std::vector<char> GetBin(const std::string& _name)
        {
            //GET PATH
            std::string _path = "bins/" + _name + ".bnc";

            //CHECK FILE
            auto _file = files.find(_path);
            if (_file == files.end())
            {
                Log::Error("Texture not found: %s", _path.c_str());
                return {};
            }

            //LOAD
            auto _data = GetRawData(_path);

            //RETURN
            return _data;
        }
    }
}