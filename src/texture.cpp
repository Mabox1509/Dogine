//[INCLUDES]
#include "../pch.h"
#include "../framework.h"
#include "../dogine.h"
#include <cerrno>



//[SLOTS TABLE]
GLuint slots[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };


//[NAMESPACE]
namespace Dogine
{
    //[CLASS]
    Texture::Texture(int _w, int _h, GLuint _filter = GL_LINEAR, GLuint _warp = GL_REPEAT)
    {
        //ALLOC RAM BUFFER
        size_t _size = (_w * _h) * sizeof(int);

        buffer = (unsigned int*)malloc(_size);
        memset(buffer, 0, _size);
        w = _w;
        h = _h;


        //ALLOC VRAM
        glGenTextures(1, &id);
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _filter);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        Log::Message("New texture with id %d", id);
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &id);
        free(buffer);
        Log::Message("Texture with id %d deleted", id);
    }


    void Texture::TexUnit(Shader& _shader, const char* _uniform, GLuint unit)
    {
        GLuint tex_uni = glGetUniformLocation(_shader.shader_id, _uniform);
        _shader.Activate();

        glUniform1i(tex_uni, unit);
    }


    void Texture::Bind(GLuint _slot)
    {
        if(slots[_slot] == id)
        {
            return;
        }

        glActiveTexture(GL_TEXTURE0 + _slot);
        glBindTexture(GL_TEXTURE_2D, id);
        slots[_slot] = id;
    }
    void Texture::Unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }


    void Texture::SetPixel(int _x, int _y, uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
    {
        int _seek = (((h - 1) - _y) * w) + _x;

        // Combinar los canales en un solo int (suponiendo un formato RGBA)
        int color = (_a << 24) | (_b << 16) | (_g << 8) | _r;

        // Asignar el valor combinado al buffer
        buffer[_seek] = color;
    }
    unsigned int Texture::GetPixel(int _x, int _y)
    {
        int _seek = (_y * w) + _x;

        return buffer[_seek];
    }
    void Texture::Apply()
    {
        Bind(1);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
        glGenerateMipmap(GL_TEXTURE_2D);

        Unbind();
    }

    int Texture::GetWidth() { return w; }
    int Texture::GetHeight() { return h; }
}