//[INCLUDES]
#include <dogine/dogine.hpp>


//[NAMESPACE]
namespace Dogine
{
	//[CLASS]
	EBO::EBO(GLuint* _indices, GLsizeiptr _size)
	{
		glGenBuffers(1, &id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _size, _indices, GL_STATIC_DRAW);
	}

	void EBO::Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	}

	void EBO::Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void EBO::Delete()
	{
		glDeleteBuffers(1, &id);
	}
}