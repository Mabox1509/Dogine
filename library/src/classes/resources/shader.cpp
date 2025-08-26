//[INCLUDES]
#include <dogine/dogine.hpp>
#include <dogine/log.hpp>

#include <cerrno>


//[FUNCTIONS]
void compileErrors(unsigned int shader, const char* type)
{
    // Stores status of compilation
    GLint hasCompiled;
    // Character array to store error message in
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            Dogine::Log::Error("SHADER_COMPILATION_ERROR for: %s\n%s", type, infoLog);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            Dogine::Log::Error("SHADER_LINKING_ERROR for: %s\n%s", type, infoLog);
        }
    }
}




//[NAMESPACE]
namespace Dogine
{
    //STATIC
    std::shared_ptr<Shader> Shader::active = nullptr;

    //[CLASS]
    Shader::Shader(const char* _vertex, const char* _fragment)
    {
        GLuint _vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(_vertex_shader, 1, &_vertex, NULL);
        glCompileShader(_vertex_shader);
        compileErrors(_vertex_shader, "VERTEX");

        GLuint _fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(_fragment_shader, 1, &_fragment, NULL);
        glCompileShader(_fragment_shader);
        compileErrors(_fragment_shader, "FRAGMENT");

        shader_id = glCreateProgram();
        glAttachShader(shader_id, _vertex_shader);
        glAttachShader(shader_id, _fragment_shader);
        glLinkProgram(shader_id);

        glDeleteShader(_vertex_shader);
        glDeleteShader(_fragment_shader);
        Log::Message("New shader with id %d", shader_id);
    }
    Shader::~Shader()
    {
        Delete();
    }

    void Shader::Activate()
    {
        active = this;
        glUseProgram(shader_id);
    }
    void Shader::Delete()
    {
        if (active == this)
            active = nullptr;

        glDeleteProgram(shader_id);
        Log::Message("Shader with id %d deleted", shader_id);
    }

    Shader* Shader::GetActive()
    {
        return active;
    }


    //UNIFORMS
    void Shader::Set1F(std::string _name, GLfloat _v0)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform1f(uniforms[_name], _v0);
    }
    void Shader::Set2F(std::string _name, GLfloat _v0, GLfloat _v1)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform2f(uniforms[_name], _v0, _v1);
    }
    void Shader::Set3F(std::string _name, GLfloat _v0, GLfloat _v1, GLfloat _v2)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform3f(uniforms[_name], _v0, _v1, _v2);
    }
    void Shader::Set4F(std::string _name, GLfloat _v0, GLfloat _v1, GLfloat _v2, GLfloat _v3)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform4f(uniforms[_name], _v0, _v1, _v2, _v3);
    }

    void Shader::Set1I(std::string _name, GLint _v0)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform1i(uniforms[_name], _v0);
    }
    void Shader::Set2I(std::string _name, GLint _v0, GLint _v1)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform2i(uniforms[_name], _v0, _v1);
    }
    void Shader::Set3I(std::string _name, GLint _v0, GLint _v1, GLint _v2)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform3i(uniforms[_name], _v0, _v1, _v2);
    }
    void Shader::Set4I(std::string _name, GLint _v0, GLint _v1, GLint _v2, GLint _v3)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform4i(uniforms[_name], _v0, _v1, _v2, _v3);
    }

    void Shader::Set1UI(std::string _name, GLuint _v0)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform1ui(uniforms[_name], _v0);
    }
    void Shader::Set2UI(std::string _name, GLuint _v0, GLuint _v1)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform2ui(uniforms[_name], _v0, _v1);
    }
    void Shader::Set3UI(std::string _name, GLuint _v0, GLuint _v1, GLuint _v2)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform3ui(uniforms[_name], _v0, _v1, _v2);
    }
    void Shader::Set4UI(std::string _name, GLuint _v0, GLuint _v1, GLuint _v2, GLuint _v3)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform4ui(uniforms[_name], _v0, _v1, _v2, _v3);
    }

    void Shader::Set1Fv(std::string _name, GLsizei _count, const GLfloat* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform1fv(uniforms[_name], _count, _value);
    }
    void Shader::Set2Fv(std::string _name, GLsizei _count, const GLfloat* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform2fv(uniforms[_name], _count, _value);
    }
    void Shader::Set3Fv(std::string _name, GLsizei _count, const GLfloat* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform3fv(uniforms[_name], _count, _value);
    }
    void Shader::Set4Fv(std::string _name, GLsizei _count, const GLfloat* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform4fv(uniforms[_name], _count, _value);
    }

    void Shader::Set1Iv(std::string _name, GLsizei _count, const GLint* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform1iv(uniforms[_name], _count, _value);
    }
    void Shader::Set2Iv(std::string _name, GLsizei _count, const GLint* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform2iv(uniforms[_name], _count, _value);
    }
    void Shader::Set3Iv(std::string _name, GLsizei _count, const GLint* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform3iv(uniforms[_name], _count, _value);
    }
    void Shader::Set4Iv(std::string _name, GLsizei _count, const GLint* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform4iv(uniforms[_name], _count, _value);
    }

    void Shader::Set1UIv(std::string _name, GLsizei _count, const GLuint* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform1uiv(uniforms[_name], _count, _value);
    }
    void Shader::Set2UIv(std::string _name, GLsizei _count, const GLuint* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform2uiv(uniforms[_name], _count, _value);
    }
    void Shader::Set3UIv(std::string _name, GLsizei _count, const GLuint* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform3uiv(uniforms[_name], _count, _value);
    }
    void Shader::Set4UIv(std::string _name, GLsizei _count, const GLuint* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniform4uiv(uniforms[_name], _count, _value);
    }

    void Shader::SetMatrix2Fv(std::string _name, GLsizei _count, GLboolean _transpose, const GLfloat* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniformMatrix2fv(uniforms[_name], _count, _transpose, _value);
    }
    void Shader::SetMatrix3Fv(std::string _name, GLsizei _count, GLboolean _transpose, const GLfloat* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniformMatrix3fv(uniforms[_name], _count, _transpose, _value);
    }
    void Shader::SetMatrix4Fv(std::string _name, GLsizei _count, GLboolean _transpose, const GLfloat* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniformMatrix4fv(uniforms[_name], _count, _transpose, _value);
    }

    void Shader::SetMatrix2x3Fv(std::string _name, GLsizei _count, GLboolean _transpose, const GLfloat* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniformMatrix2x3fv(uniforms[_name], _count, _transpose, _value);
    }
    void Shader::SetMatrix3x2Fv(std::string _name, GLsizei _count, GLboolean _transpose, const GLfloat* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniformMatrix3x2fv(uniforms[_name], _count, _transpose, _value);
    }
    void Shader::SetMatrix2x4Fv(std::string _name, GLsizei _count, GLboolean _transpose, const GLfloat* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniformMatrix2x4fv(uniforms[_name], _count, _transpose, _value);
    }
    void Shader::SetMatrix4x2Fv(std::string _name, GLsizei _count, GLboolean _transpose, const GLfloat* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniformMatrix4x2fv(uniforms[_name], _count, _transpose, _value);
    }
    void Shader::SetMatrix3x4Fv(std::string _name, GLsizei _count, GLboolean _transpose, const GLfloat* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniformMatrix3x4fv(uniforms[_name], _count, _transpose, _value);
    }
    void Shader::SetMatrix4x3Fv(std::string _name, GLsizei _count, GLboolean _transpose, const GLfloat* _value)
    {
        if (uniforms.find(_name) == uniforms.end())
        {
            uniforms[_name] = glGetUniformLocation(shader_id, _name.c_str());
        }
        glUniformMatrix4x3fv(uniforms[_name], _count, _transpose, _value);
    }

}