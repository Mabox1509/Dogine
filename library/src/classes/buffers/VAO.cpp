//[INCLUDES]
#include <dogine/dogine.hpp>


//[NAMESPACE]
namespace Dogine
{
	//[CLASS]
	VAO::VAO()
	{
		glGenVertexArrays(1, &id);
	}

	void VAO::LinkAttrib(VBO& _VBO, GLuint _layout, GLuint _num_components, GLenum _type, GLsizeiptr _stride, void* offset)
	{
		_VBO.Bind();

		glVertexAttribPointer(_layout, _num_components, _type, GL_FALSE, _stride, offset);
		glEnableVertexAttribArray(_layout);

		_VBO.Unbind();
	}
	void VAO::Bind()
	{
		glBindVertexArray(id);
	}

	void VAO::Unbind()
	{
		glBindVertexArray(0);
	}

	void VAO::Delete()
	{
		glDeleteVertexArrays(1, &id);
	}
}