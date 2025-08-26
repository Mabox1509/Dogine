//[INCLUDES]
#include "../../inc/systems/dogres.h"
#include <iostream>
#include <unordered_map>
#include <iomanip>
#include <vector>
#include <sndfile.h>
#include <cmath>

#include "../../inc/utils/filesys.h"
#include "../../inc/utils/string_utils.h"
#include "../../inc/utils/compression.h"

#include "../../inc/json.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "../../inc/stb_image.h"


//[TYPES]
struct WavData
{
    int sampleRate;
    int bitDepth;       // 8, 16, 32 bits
    int channels;
    int64_t samples;
    double duration;
    std::vector<std::vector<float>> channelData; // [canal][muestra]
};





//[FUNCTIONS]
WavData LoadWav(const std::string& path)
{
    SF_INFO sfinfo{};
    SNDFILE* sndfile = sf_open(path.c_str(), SFM_READ, &sfinfo);
    if (!sndfile)
    {
        throw std::runtime_error("Error opening sound file: " + path);
    }

    // Info básica
    WavData wav;
    wav.sampleRate = sfinfo.samplerate;
    wav.channels   = sfinfo.channels;
    wav.samples    = sfinfo.frames;
    wav.duration   = wav.samples / static_cast<double>(wav.sampleRate);

    // Determinar bit depth en bytes
    int depthBytes = 0;
    if ((sfinfo.format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_16) depthBytes = 2;
    else if ((sfinfo.format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_32) depthBytes = 4;
    else if ((sfinfo.format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_U8) depthBytes = 1;
    else
        throw std::runtime_error("Unsupported WAV bit depth in: " + path);

    wav.bitDepth = depthBytes * 8;

    // Leer datos crudos
    std::vector<char> raw(wav.samples * wav.channels * depthBytes);
    sf_read_raw(sndfile, raw.data(), raw.size());
    sf_close(sndfile);

    // Separar canales a float [-1, 1]
    wav.channelData.resize(wav.channels, std::vector<float>(wav.samples));

    for (int c = 0; c < wav.channels; c++)
    {
        for (int s = 0; s < wav.samples; s++)
        {
            size_t offset = s * (wav.channels * depthBytes) + c * depthBytes;
            int sampleInt = 0;

            if (depthBytes == 1)
                sampleInt = static_cast<unsigned char>(raw[offset]) - 128;
            else if (depthBytes == 2)
                sampleInt = *reinterpret_cast<const int16_t*>(&raw[offset]);
            else if (depthBytes == 4)
                sampleInt = *reinterpret_cast<const int32_t*>(&raw[offset]);

            float sampleFloat = 0.0f;
            if (depthBytes == 1) sampleFloat = sampleInt / 128.0f;
            else if (depthBytes == 2) sampleFloat = sampleInt / 32768.0f;
            else if (depthBytes == 4) sampleFloat = sampleInt / 2147483648.0f;

            wav.channelData[c][s] = sampleFloat;
        }
    }

    return wav;
}
double LogBase(double value, double base)
{
    if (value <= 0.0 || base <= 0.0 || base == 1.0)
        throw std::invalid_argument("Value and base must be > 0 and base ≠ 1");

    return std::log(value) / std::log(base);
}
inline double gaussian(double x, double s)
{
    return std::exp(-std::pow(x * s, 2.0));
}

float Sample(double time, const std::vector<float>& samples)
{
    if (samples.empty())
        return 0.0f;

    int i = static_cast<int>(std::floor(time));
    double frac = time - i;

    // Si es índice exacto, devuelve directo
    if (frac == 0.0)
    {
        if (i >= 0 && i < (int)samples.size())
            return samples[i];
        return 0.0f;
    }

    // Acumular pesos y suma
    double sum = 0.0;
    double wsum = 0.0;

    for (int k = -2; k <= 2; ++k)
    {
        int idx = i + k;
        if (idx < 0 || idx >= (int)samples.size()) continue;

        double dist = (k - frac); // distancia con parte fraccional
        double w = gaussian(dist, 1.0); // usa s=1.0, ajustable

        sum += samples[idx] * w;
        wsum += w;
    }

    if (wsum == 0.0)
        return 0.0f;

    return static_cast<float>(sum / wsum);
}


//[NAMESPACE]
namespace Dogres 
{
    //[VARIABLES]
    const std::array<std::string, 5> types_ext = 
    {
        "txc",
        "snc",
        "mdc",
        "txtc",
        "bnc"
    };

    //[FUNCTIONS]
    AssetType DefineType(const std::string& _path)
    {
        std::string _path_lower = StringUtils::ToLower(_path);

        if (StringUtils::EndsWith(_path_lower, ".png") || StringUtils::EndsWith(_path_lower, ".jpg"))
        {
            return AssetType::Texture;
        }
        else if (StringUtils::EndsWith(_path_lower, ".wav"))
        {
            return AssetType::Audio;
        }
        else if (StringUtils::EndsWith(_path_lower, ".obj") || StringUtils::EndsWith(_path_lower, ".ply"))
        {
            return AssetType::Model;
        }
        else if (StringUtils::EndsWith(_path_lower, ".txt") || StringUtils::EndsWith(_path_lower, ".json") || StringUtils::EndsWith(_path_lower, ".shader"))
        {
            return AssetType::Text;
        }
        return AssetType::Bin;
    }
    

    void ProcessTexture(const std::string& in_path, const std::string& out_path)
    {
        //READ PNG
        int _width, _height, _channels;
        unsigned char* _raw_data = stbi_load(in_path.c_str(), &_width, &_height, &_channels, 4);

        //LOAD META
        if(!FileSys::FileExists(in_path+ ".meta"))
        {
            std::cerr << "Not metadata file founded for: " << in_path << std::endl;
            exit(1);
        }
        auto _file = FileSys::ReadString(in_path + ".meta");

        nlohmann::json _meta;
        try
        {
            _meta = nlohmann::json::parse(_file);
        } 
        catch (const std::exception& e)
        {
            std::cerr << "Failed to parse .meta " << e.what() << std::endl;
            exit(1);
        }

        uint8_t _warp_mode = 0;
        bool _mipmaps = false;
        bool _interpolation = false;

        //Read warp mode
        if (!_meta.contains("warp_mode") || !_meta["warp_mode"].is_string())
        {
            std::cerr << "Missing or invalid required field 'warp_mode' in metadata." << std::endl;
            exit(1);
        }
        std::string _warp_str = _meta.value("warp_mode", "clamp");
        if (_warp_str == "clamp") _warp_mode = 0;
        else if (_warp_str == "repeat") _warp_mode = 1;
        else if (_warp_str == "mirror") _warp_mode = 2;
        else
        {
            std::cerr << "Unknown warp_mode: " << _warp_str << std::endl;
            exit(1);
        }

        //Other flags
        if (!_meta.contains("mipmaps") || !_meta["mipmaps"].is_boolean())
        {
            std::cerr << "Missing or invalid required field 'mipmaps' in metadata." << std::endl;
            exit(1);
        }
        _mipmaps = _meta.value("mipmaps", false);

        if (!_meta.contains("interpolation") || !_meta["interpolation"].is_boolean())
        {
            std::cerr << "Missing or invalid required field 'interpolation' in metadata." << std::endl;
            exit(1);
        }
        _interpolation = _meta.value("interpolation", false);



        //GET PALLETE
        size_t _pixel_count = _width * _height;
        const uint32_t* _pixels = reinterpret_cast<const uint32_t*>(_raw_data);

        std::unordered_map<uint32_t, size_t> _palette;
        std::vector<uint32_t> _pallete_indexs;
        for (size_t i = 0; i < _pixel_count; ++i)
        {
            uint32_t _color = _pixels[i];
            if (_palette.find(_color) == _palette.end())
            {
                size_t _index = _palette.size();
                _palette[_color] = _index;

                _pallete_indexs.push_back(_color);

                /*std::cout << "New color founded: 0x" 
                << std::hex << std::uppercase << std::setfill('0') << std::setw(8) 
                << _color << std::dec << std::endl;*/
            }
        }
        uint8_t _bytessize = std::max<uint8_t>(1, ceil(LogBase(_palette.size(), 256)));


        //SERIALIZE
        size_t _seek = 0;

        size_t _out_size = 16;
        _out_size += (_palette.size() * 4);
        _out_size += (_pixel_count * _bytessize);

        char* _out_buffer = new char[_out_size];


        memcpy(_out_buffer + _seek, &_width, sizeof(int)); _seek += sizeof(int);
        memcpy(_out_buffer + _seek, &_height, sizeof(int)); _seek += sizeof(int);

        memcpy(_out_buffer + _seek, &_warp_mode, sizeof(uint8_t)); _seek += sizeof(uint8_t);
        memcpy(_out_buffer + _seek, &_mipmaps, sizeof(bool)); _seek += sizeof(bool);
        memcpy(_out_buffer + _seek, &_interpolation, sizeof(bool)); _seek += sizeof(bool);

        uint8_t _byte_size_u8 = static_cast<uint8_t>(_bytessize);
        memcpy(_out_buffer + _seek, &_byte_size_u8, sizeof(uint8_t)); _seek += sizeof(uint8_t);
        uint32_t _palette_count = static_cast<uint32_t>(_palette.size());
        memcpy(_out_buffer + _seek, &_palette_count, sizeof(uint32_t)); _seek += sizeof(uint32_t);
        for (const auto& _color : _pallete_indexs)
        {
            memcpy(_out_buffer + _seek, &_color, sizeof(uint32_t));
            _seek += sizeof(uint32_t);
        }

        //PALLETE
        for (size_t i = 0; i < _pixel_count; ++i)
        {
            uint32_t _color = _pixels[i];
            uint32_t _index = static_cast<uint32_t>(_palette[_color]);

            // Escribir el índice en _bytessize bytes (little-endian)
            for (size_t b = 0; b < _bytessize; ++b)
            {
                _out_buffer[_seek++] = (_index >> (b * 8)) & 0xFF;
            }
        }
        
        //SAVE
        FileSys::WriteBinary(out_path, Compression::Compress(std::vector<char>(_out_buffer, _out_buffer + _out_size)));
        
        //FREE
        free(_raw_data);
        delete[] _out_buffer;
    }
    void ProcessAudio(const std::string& in_path, const std::string& out_path)
    {
        //LOAD META
        if(!FileSys::FileExists(in_path+ ".meta"))
        {
            std::cerr << "Not metadata file founded for: " << in_path << std::endl;
            exit(1);
        }
        nlohmann::json _meta;
        try
        {
            _meta = nlohmann::json::parse(FileSys::ReadString(in_path + ".meta"));
        } 
        catch (const std::exception& e)
        {
            std::cerr << "Failed to parse .meta " << e.what() << std::endl;
            exit(1);
        }

        // Bit depth
        if (!_meta.contains("bit_depth") || !_meta["bit_depth"].is_number_unsigned())
        {
            std::cerr << "Missing or invalid required field 'bit_depth' in metadata." << std::endl;
            exit(1);
        }
        uint8_t _bitdepth = _meta.value("bit_depth", 8);
        if (_bitdepth != 8 && _bitdepth != 16 && _bitdepth != 32)
        {
            std::cerr << "Invalid bit_depth: " << static_cast<int>(_bitdepth)
                    << ". Allowed values: 8, 16, 32." << std::endl;
            exit(1);
        }

        // Sample rate
        if (!_meta.contains("sample_rate") || !_meta["sample_rate"].is_number_unsigned())
        {
            std::cerr << "Missing or invalid required field 'sample_rate' in metadata." << std::endl;
            exit(1);
        }
        uint32_t _sample_rate = _meta.value("sample_rate", 44100u);

        // Loop points
        double _loop_start = 0.0;
        if (_meta.contains("loop_start") && _meta["loop_start"].is_number())
            _loop_start = _meta["loop_start"].get<double>();

        // Base frequency
        double _base_freq = 440.0;
        if (_meta.contains("base_freq") && _meta["base_freq"].is_number())
            _base_freq = _meta["base_freq"].get<double>();


        //READ .wav
        auto _wav = LoadWav(in_path);



        //GEN WAVES
        uint64_t _fixed_samples = _wav.duration *  _sample_rate;
        uint64_t _looppoint = _loop_start * _fixed_samples;
        std::vector<std::vector<char>> _samples;

        //std::cout << "Audio freq: " << (int)_wav.sampleRate << "  Fixed freq: " << (int)_sample_rate << std::endl; 
        //std::cout << "Audio samples: " << (int)_wav.samples << "  Fixed samples: " << (int)_fixed_samples << std::endl; 
        for(int _c = 0;_c < _wav.channels;_c++)
        {
            std::vector<char> _channel;
            _channel.resize(_fixed_samples * (_bitdepth / 8));
            for(int _s = 0;_s < _fixed_samples;_s++)
            {
                //Sample audio
                double _t = (_wav.sampleRate / (double)_sample_rate) * _s;
                _t = std::min(_t, (double)_wav.samples);
                float _sample = Sample(_t, _wav.channelData[_c]);
                _sample = std::clamp(_sample, -1.0f, 1.0f);

                //WRITE SAMPLE
                size_t _seek = _s * (_bitdepth / 8);

                if (_bitdepth == 8)
                {
                    // 8-bit PCM unsigned (0–255)
                    int8_t _val = (int8_t)(_sample * 127.0f);
                    std::memcpy(&_channel[_seek], &_val, sizeof(_val));
                }
                else if (_bitdepth == 16)
                {
                    int16_t _val = static_cast<int16_t>(_sample * 32767.0f);
                    std::memcpy(&_channel[_seek], &_val, sizeof(_val));
                }
                else if (_bitdepth == 32)
                {
                    int32_t _val = static_cast<int32_t>(_sample * 2147483647.0f);
                    std::memcpy(&_channel[_seek], &_val, sizeof(_val));
                }

                //Write sample
                //_channel.push_back(_sample);
            }

            _samples.push_back(_channel);
        }

        //WRITE FILE
        size_t _osize = 64; //Samp, Freq, Bit, Loop, Base
        _osize += _samples[0].size() * _wav.channels;
        char* _buffer = new char[_osize];
        size_t _seek = 0;

        std::memcpy(_buffer + _seek, &_fixed_samples, sizeof(uint64_t)); _seek += sizeof(uint64_t); //Samples per channel
        std::memcpy(_buffer + _seek, &_sample_rate, sizeof(uint32_t)); _seek += sizeof(uint32_t); //Freq
        std::memcpy(_buffer + _seek, &_bitdepth, sizeof(uint8_t)); _seek += sizeof(uint8_t); //Depth
        std::memcpy(_buffer + _seek, &_looppoint, sizeof(uint64_t)); _seek += sizeof(uint64_t); //Loop
        std::memcpy(_buffer + _seek, &_base_freq, sizeof(double)); _seek += sizeof(double); //Base freq
        std::memcpy(_buffer + _seek, &_wav.channels, sizeof(int)); _seek += sizeof(int); //Channels


        _seek = 64;
        uint8_t _ssize = _bitdepth / 8;
        for(int _s = 0; _s < _fixed_samples; _s++)
        {
            for(int _c = 0; _c < _wav.channels; _c++)
            {
                size_t _src_seek = _s * _ssize; // offset en bytes dentro del canal
                const void* src = _samples[_c].data() + _src_seek;

                std::memcpy(_buffer + _seek, src, _ssize);
                _seek += _ssize;

            }
        }

        //SAVE
        FileSys::WriteBinary(out_path, std::vector<char>(_buffer, _buffer + _seek));
        delete[] _buffer;

    }
    void ProcessModel(const std::string& in_path, const std::string& out_path){}
    void ProcessText(const std::string& in_path, const std::string& out_path)
    {
        //READ DATA
        auto _data = FileSys::ReadBinary(in_path);

        //COMPRESS
        auto _compress = Compression::Compress(_data);

        //Save
        FileSys::WriteBinary(out_path, _compress);
    }
    void ProcessBin(const std::string& in_path, const std::string& out_path)
    {
        //Just copy the data...
        FileSys::CopyFile(in_path, out_path);
    }
}