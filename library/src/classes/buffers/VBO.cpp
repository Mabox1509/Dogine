//[INCLUDES]
#include <dogine/dogine.hpp>


//[NAMESPACE]
namespace Dogine
{
	//[CLASS]
	VBO::VBO(GLfloat* _vertices, GLsizeiptr _size)
	{
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, _size, _vertices, GL_STATIC_DRAW);
	}

	void VBO::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, id);
	}

	void VBO::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void VBO::Delete()
	{
		glDeleteBuffers(1, &id);
	}
}