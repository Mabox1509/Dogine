//[INCLUDES]
#include "../pch.h"
#include "../framework.h"
#include "../dogine.h"
#include <cerrno>


//[NAMESPACE]
namespace Dogine
{
	Surface::Surface(int _w, int _h)
	{
		w = _w;
		h = _h;

		//GENERATE FRAME BUFFER
		glGenFramebuffers(1, &id);
		glBindFramebuffer(GL_FRAMEBUFFER, id);

        // 2. GENERATE COLOR BUFFER
        glGenTextures(1, &color_id);
        glBindTexture(GL_TEXTURE_2D, color_id);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_id, 0);



        //GENERATE RENDER BUFFER
        glGenRenderbuffers(1, &render_id);
        glBindRenderbuffer(GL_RENDERBUFFER, render_id);

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_id);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // 4. CHECK FRAMEBUFFER STATUS
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            Log::Error("ERROR: Framebuffer is not complete!");
        }

        // 5. UNBIND EVERYTHING
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        Log::Message("New surface with id %d", id);
	}
    Surface::~Surface()
    {
        Log::Message("Surface with id %d deleted", id);

        glDeleteFramebuffers(1, &id);
        glDeleteTextures(1, &color_id);
        glDeleteRenderbuffers(1, &render_id);


    }

    void Surface::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
    }
    void Surface::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    int Surface::GetWidth()
    {
        return w;
    }
    int Surface::GetHeight()
    {
        return h;
    }
}