//[INCLUDES]
#include <dogine/graphics.hpp>
#include <dogine/window.hpp>

#include <iostream>
#include <cstdarg>
#include <cstdio>
#include <chrono>
#include <iomanip>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//[PRIVATE TYPES]

//[PRIVATE VARIABLES]
Dogine::UBO viewproc_ubo(sizeof(glm::mat4) * 3, 0);

//[PRIVATE FUNCTIONS]


//[NAMESPACE]
namespace Dogine
{
    namespace Graphics
    {
        //[FUNCTIONS]
        void SetViewProjection(const glm::mat4& _view, const glm::mat4& _projection)
        {
            glm::mat4 _viewproc = _projection * _view;

            // Enviar matrices al UBO consecutivamente
            viewproc_ubo.Update(0, sizeof(glm::mat4), &_view[0]);
            viewproc_ubo.Update(sizeof(glm::mat4), sizeof(glm::mat4), &_projection[0]);
            viewproc_ubo.Update(sizeof(glm::mat4) * 2, sizeof(glm::mat4), &_viewproc[0]);
        }


        void Draw(Mesh* _asset, 
              const glm::vec3& _position,
              const glm::vec3& _rotation,
              const glm::vec3& _scale)
        {
            glm::mat4 _matrix = glm::mat4(1.0f);


            _matrix = glm::translate(_matrix, _position);
            _matrix = glm::rotate(_matrix, glm::radians(_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            _matrix = glm::rotate(_matrix, glm::radians(_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            _matrix = glm::rotate(_matrix, glm::radians(_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            _matrix = glm::scale(_matrix, _scale);

            auto _shader = Dogine::Shader::GetActive();
            _shader->SetMatrix4Fv("u_Model", 1, GL_FALSE, glm::value_ptr(_matrix));


            _asset->Bind();
            glDrawElements(GL_TRIANGLES, _asset->GetTriangles(), GL_UNSIGNED_INT, 0);
        }
        void Draw(Mesh* _asset, const glm::mat4& _matrix)
        {
            auto _shader = Dogine::Shader::GetActive();
            _shader->SetMatrix4Fv("u_Model", 1, GL_FALSE, glm::value_ptr(_matrix));


            _asset->Bind();
            glDrawElements(GL_TRIANGLES, _asset->GetTriangles(), GL_UNSIGNED_INT, 0);
        }
        void DrawInstanced(Mesh* _asset, 
                   const std::vector<glm::mat4>& _matrices)
        {
            if (!_asset || _matrices.empty()) return;

            auto _shader = Dogine::Shader::GetActive();
            if (!_shader) return;

            _asset->Bind(); // Bind VAO del mesh

            // Creamos un VBO temporal para las matrices
            VBO instanceVBO((GLfloat*)_matrices.data(), _matrices.size() * sizeof(glm::mat4));

            // Cada matriz necesita 4 atributos vec4
            GLuint baseAttrib = 3; // Asumiendo que los primeros 3 o 4 layouts están ocupados por vertex attributes
            for (int i = 0; i < 4; ++i)
            {
                glEnableVertexAttribArray(baseAttrib + i);
                glVertexAttribPointer(
                    baseAttrib + i,        // layout
                    4,                     // vec4
                    GL_FLOAT,              // type
                    GL_FALSE,              // normalized
                    sizeof(glm::mat4),     // stride
                    (void*)(sizeof(glm::vec4) * i) // offset
                );
                glVertexAttribDivisor(baseAttrib + i, 1); // Incrementa por instancia
            }

            // Dibujar todas las instancias
            glDrawElementsInstanced(
                GL_TRIANGLES,
                _asset->GetTriangles(),
                GL_UNSIGNED_INT,
                0,
                static_cast<GLsizei>(_matrices.size())
            );

            // Desenlazamos el VBO de instancias
            for (int i = 0; i < 4; ++i)
                glDisableVertexAttribArray(baseAttrib + i);

            _asset->Unbind();
        }

        


        void SetBlending(bool enable)
        {
            if (enable) glEnable(GL_BLEND);
            else glDisable(GL_BLEND);
        }
        void SetBlendFunc(GLenum src, GLenum dst)
        {
            glBlendFunc(src, dst);
        }
        void SetBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
        {
            glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
        }
        void SetBlendEquation(GLenum mode)
        {
            glBlendEquation(mode);
        }

        void SetFaceCulling(bool enable)
        {
            if (enable) glEnable(GL_CULL_FACE);
            else glDisable(GL_CULL_FACE);
        }
        void SetCullFace(GLenum mode)
        {
            glCullFace(mode); // GL_FRONT, GL_BACK, GL_FRONT_AND_BACK
        }

        void SetDepthTest(bool enable)
        {
            if (enable) glEnable(GL_DEPTH_TEST);
            else glDisable(GL_DEPTH_TEST);
        }
        void SetDepthMask(bool enable)
        {
            glDepthMask(enable ? GL_TRUE : GL_FALSE);
        }

        void SetStencilTest(bool enable)
        {
            if (enable) glEnable(GL_STENCIL_TEST);
            else glDisable(GL_STENCIL_TEST);
        }
        void SetStencilFunc(GLenum func, GLint ref, GLuint mask)
        {
            glStencilFunc(func, ref, mask);
        }
        void SetStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass)
        {
            glStencilOp(sfail, dpfail, dppass);
        }

        void BindTarget(Surface* _surface)
        {
            if (_surface == nullptr)
            {
                glViewport(0, 0, Window::GetWidth(), Window::GetHeight());
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
            else
            {
                glViewport(0, 0, _surface->GetWidth(), _surface->GetHeight());
                glBindFramebuffer(GL_FRAMEBUFFER, _surface->GetFbo());
            }
        }
        void BindTexture(int _slot, Surface* _surface)
        {
            glActiveTexture(GL_TEXTURE0 + _slot);

            if (_surface == nullptr)
            {
                glBindTexture(GL_TEXTURE_2D, 0);
            } 
            else
            {
                glBindTexture(GL_TEXTURE_2D, _surface->GetTexture());
            }
        }
        void BindTexture(int _slot, Texture* _texture)
        {
            glActiveTexture(GL_TEXTURE0 + _slot);

            if(_texture == nullptr)
            {
                glBindTexture(GL_TEXTURE_2D, 0);
                return;
            }

            glBindTexture(GL_TEXTURE_2D, _texture->GetID());
        }

    } // namespace Log
} // namespace Dogine

