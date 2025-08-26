//[INCLUDES]
#include <dogine/dogine.hpp>


//[NAMESPACE]
namespace Dogine
{
    //[CLASS]
    UBO::UBO(GLsizeiptr _size, GLuint _bindingPoint)
    {
        glGenBuffers(1, &id);
        bindingPoint = _bindingPoint;

        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferData(GL_UNIFORM_BUFFER, _size, nullptr, GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, id); // Asocia binding point
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void UBO::Bind()
    {
        glBindBuffer(GL_UNIFORM_BUFFER, id);
    }

    void UBO::Unbind()
    {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void UBO::Update(GLintptr offset, GLsizeiptr size, const void* data)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void UBO::Delete()
    {
        glDeleteBuffers(1, &id);
    }
}
