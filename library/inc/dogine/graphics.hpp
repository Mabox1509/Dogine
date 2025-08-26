#pragma once

//[INCLUDES]
#include "./dogine.hpp"

#include <glm/glm.hpp>
#include <vector>

//[NAMESPACE]
namespace Dogine
{
    namespace Graphics
    {
        //[FUNCTIONS]      
        //Set camera matrix
        void SetViewProjection(const glm::mat4& _view, const glm::mat4& _projection);

        //Draw single object
        void Draw(Mesh* _asset, 
              const glm::vec3& _position,
              const glm::vec3& _rotation = glm::vec3(0.0f),
              const glm::vec3& _scale = glm::vec3(1.0f));
        //Draw single object
        void Draw(Mesh* _asset, const glm::mat4& _matrix);

        //Draw multiple objects
        void DrawInstanced(Mesh* _asset, 
                       const std::vector<glm::mat4>& _matrices);
        
        // Enable or disable blending
        void SetBlending(bool enable);
        // Set the blending function (src, dst)
        void SetBlendFunc(GLenum src, GLenum dst);
        // More complete variant: separates color and alpha
        void SetBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
        // Set the blending equation
        void SetBlendEquation(GLenum mode); // GL_FUNC_ADD, GL_FUNC_SUBTRACT, etc.


        //Enable or disable face culling
        void SetFaceCulling(bool enable);
        //Set culling type (front, back, front_and_back)
        void SetCullFace(GLenum mode); // GL_FRONT, GL_BACK, GL_FRONT_AND_BACK


        // Enable or disable depth test
        void SetDepthTest(bool enable);
        // Enable or disable depth write
        void SetDepthMask(bool enable);

        // Enable or disable stencil
        void SetStencilTest(bool enable);
        // Configure stencil function
        void SetStencilFunc(GLenum func, GLint ref, GLuint mask);
        // Configure stencil operation
        void SetStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass);
        

        void BindTarget(Surface* _surface);
        void BindTexture(int _slot, Surface* _surface);
        void BindTexture(int _slot, Texture* _texture);
    }
}