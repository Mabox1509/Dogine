//[INCLUDES]
#include <dogine/dogine.hpp>
#include <dogine/window.hpp>
#include <dogine/log.hpp>

#include <iostream>
#include <cstring>
#include <glm/gtc/matrix_transform.hpp>

//[PRIVATE TYPES]

//[PRIVATE VARIABLES]

//[PRIVATE FUNCTIONS]


//[NAMESPACE]
namespace Dogine
{
    Surface::Surface(int _width, int _height)
    {
        width = _width;
        height = _height;

        //GENERATE FRAME BUFFER
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // 2. GENERATE COLOR BUFFER
        glGenTextures(1, &color_id);
        glBindTexture(GL_TEXTURE_2D, color_id);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_id, 0);



        //GENERATE RENDER BUFFER
        glGenRenderbuffers(1, &render_id);
        glBindRenderbuffer(GL_RENDERBUFFER, render_id);

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_id);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // 4. CHECK FRAMEBUFFER STATUS
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Framebuffer is not complete");


        // 5. UNBIND EVERYTHING
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        Log::Message("New surface with id %d", fbo);
    }
    Surface::~Surface()
    {
        //Unbind FBO
        GLint _current_fbo = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_current_fbo);
        if ((GLuint)_current_fbo == fbo)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        //Unbind texture
        GLint _max_slots = 0;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &_max_slots);

        for(GLuint _i = 0;_i < _max_slots;_i++)
        {
            GLint _bound = 0;
            glGetIntegeri_v(GL_TEXTURE_BINDING_2D, _i, &_bound);

            if ((GLuint)_bound == color_id)
            {
                glActiveTexture(GL_TEXTURE0 + _i);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }

        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &color_id);
        glDeleteRenderbuffers(1, &render_id);

        //Log
        Log::Message("Surface with id %d deleted", fbo);
    }



    int Surface::GetWidth()
    {
        return width;
    }
    int Surface::GetHeight()
    {
        return height;
    }


    GLuint Surface::GetFbo()
    {
        return fbo;
    }
    GLuint Surface::GetTexture()
    {
        return color_id;
    }
    GLuint Surface::GetRender()
    {
        return render_id;
    }

    void Surface::Copy(Surface* _dst, bool _copy_depth, bool _copy_stencil)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _dst->GetFbo());


        GLbitfield mask = 0;
        mask |= GL_COLOR_BUFFER_BIT;
        if(_copy_depth)   mask |= GL_DEPTH_BUFFER_BIT;
        if(_copy_stencil) mask |= GL_STENCIL_BUFFER_BIT;

        glBlitFramebuffer(
            0, 0, width, height,
            0, 0, _dst->GetWidth(), _dst->GetHeight(),
            mask,
            GL_NEAREST // o GL_LINEAR si quieres suavizado
        );

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
    void Surface::Save(const std::string& _path)
    {
        //Save texture to file (as PNG)
        GLubyte* pixels = new GLubyte[3 * width * height];
        glBindTexture(GL_TEXTURE_2D, color_id);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        //Free
        delete[] pixels;
    }
    glm::mat4 Surface::GetOrthoMatrix()
    {
        return glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f);
    }
}