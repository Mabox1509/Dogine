//[INCLUDES]
#include <dogine/dogine.hpp>
#include <dogine/graphics.hpp>
#include <dogine/log.hpp>

#include <iostream>
#include <cstring>

//[PRIVATE TYPES]

//[PRIVATE VARIABLES]

//[PRIVATE FUNCTIONS]


//[NAMESPACE]
namespace Dogine
{
    Texture::Texture(int _width, int _height, Warpmode _warp, bool _interpolation, bool _mipmaps)
    {
        mipmaps = _mipmaps;

        //ALLOC RAM BUFFER
        pixels = new color_t[_width * _height];
        width = _width;
        height = _height;

        warp = _warp;
        interpolation = _interpolation;
        mipmaps = _mipmaps;


        //ALLOC VRAM
        glGenTextures(1, &id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, id);

        GLint _min_filter;
        GLint _max_filter;

        if (interpolation)
        {
            if (mipmaps)
                _min_filter = GL_NEAREST_MIPMAP_LINEAR;
            else
                _min_filter = GL_LINEAR;
            
            _max_filter = GL_LINEAR;
        }
        else
        {
            if (mipmaps)
                _min_filter = GL_NEAREST_MIPMAP_NEAREST;
            else
                _min_filter = GL_NEAREST;
            
            _max_filter = GL_NEAREST;
        }

        GLint _warpmode = (warp == Warpmode::Clamp ? GL_CLAMP_TO_EDGE : (warp == Warpmode::Mirror ? GL_MIRRORED_REPEAT : GL_REPEAT));

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _max_filter);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _warpmode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _warpmode);


        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        if(mipmaps)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        glBindTexture(GL_TEXTURE_2D, 0);

        //Log::Message("New texture with id %d", id);
    }
    Texture::~Texture()
    {
        //UNBIND
        GLint _max_slots = 0;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &_max_slots);

        for(GLuint _i = 0;_i < _max_slots;_i++)
        {
            GLint _bound = 0;
            glGetIntegeri_v(GL_TEXTURE_BINDING_2D, _i, &_bound);

            if ((GLuint)_bound == id)
            {
                glActiveTexture(GL_TEXTURE0 + _i);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }

        //DELETE
        glDeleteTextures(1, &id);
        delete[] pixels;
        //Log::Message("Texture with id %d deleted", id);
    }

    //void TexUnit(Shader& _shader, const char* _uniform, GLuint unit);

    void Texture::UpdateContext(){}

    void Texture::SetPixel(int _x, int _y, color_t _pixel)
    {
        if (_x < 0 || _x >= width || _y < 0 || _y >= height)
            throw std::out_of_range("Pixel coordinates out of bounds");


        size_t _seek = (_y * width) + _x;

        // Asignar el valor combinado al buffer
        pixels[_seek] = _pixel;
    }
    color_t Texture::GetPixel(int _x, int _y)
    {
        if (_x < 0 || _x >= width || _y < 0 || _y >= height)
            throw std::out_of_range("Pixel coordinates out of bounds");
        
        size_t _seek = (_y * width) + _x;

        // Devolver el valor del pixel en esa posición
        return pixels[_seek];
    }
    void Texture::Apply()
    {
        Graphics::BindTexture(0, this);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        if (mipmaps)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        Graphics::BindTexture(0, static_cast<Texture*>(nullptr));
    }

    int Texture::GetWidth()
    {
        return width;
    }
    int Texture::GetHeight()
    {
        return height;
    }
    GLuint Texture::GetID()
    {
        return id;
    }
}