//[INCLUDES]
#include "../pch.h"
#include "../framework.h"
#include "../dogine.h"
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
            Log::Error("SHADER_COMPILATION_ERROR for: %s\n%s", type, infoLog);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            Log::Error("SHADER_LINKING_ERROR for: %s\n%s", type, infoLog);
        }
    }
}




//[NAMESPACE]
namespace Dogine
{
    //[CLASS]
    Shader::Shader(const char* _vertex, const char* _fragment)
    {
        int _compile;

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
        glUseProgram(shader_id);
    }
    void Shader::Delete()
    {
        glDeleteProgram(shader_id);
        Log::Message("Shader with id %d deleted", shader_id);
    }
}