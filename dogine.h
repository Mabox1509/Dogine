#ifndef _DOGINE_H
#define _DOGINE_H
//[INCLUDES]
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>






//[NAMESPACE]
namespace Log 
{
	void Message(std::string _msg, ...);
	void Warning(std::string _msg, ...);
	void Error(std::string _msg, ...);
}

namespace Dogine
{
	#pragma region TYPES
	//[BUFFERS]
	class VBO {

	public:
		GLuint id;
		VBO(GLfloat* _vertices, GLsizeiptr _size);

		void Bind();
		void Unbind();
		void Delete();
	};
	class VAO
	{
	public:
		GLuint id;
		VAO();

		void LinkAttrib(VBO& _VBO, GLuint _layout, GLuint _num_components, GLenum _type, GLsizeiptr _stride, void* offset);
		void Bind();
		void Unbind();
		void Delete();
	};
	class EBO
	{
	public:
		GLuint id;
		EBO(GLuint* _indices, GLsizeiptr _size);

		void Bind();
		void Unbind();
		void Delete();
	};



	//[RESOURCES]
	class Shader
	{
	private:
		std::unordered_map<std::string, GLuint> uniforms;

	public:
		//[VARIABLES]
		GLuint shader_id;

		//[CONSTRUCTOR]
		Shader(const char* _vertex, const char* _fragment);
		~Shader();

		//[FUNCTIONS]
		void Activate();
		void Delete();


		//[UNIFORMS]
		void Set1F(std::string _name, GLfloat _v0);
		void Set2F(std::string _name, GLfloat _v0, GLfloat _v1);
		void Set3F(std::string _name, GLfloat _v0, GLfloat _v1, GLfloat _v2);
		void Set4F(std::string _name, GLfloat _v0, GLfloat _v1, GLfloat _v2, GLfloat _v3);

		void Set1I(std::string _name, GLint _v0);
		void Set2I(std::string _name, GLint _v0, GLint _v1);
		void Set3I(std::string _name, GLint _v0, GLint _v1, GLint _v2);
		void Set4I(std::string _name, GLint _v0, GLint _v1, GLint _v2, GLint _v3);


		void Set1UI(std::string _name, GLuint _v0);
		void Set2UI(std::string _name, GLuint _v0, GLuint _v1);
		void Set3UI(std::string _name, GLuint _v0, GLuint _v1, GLuint _v2);
		void Set4UI(std::string _name, GLuint _v0, GLuint _v1, GLuint _v2, GLuint _v3);
	
		void Set1Fv(std::string _name, GLsizei _count, const GLfloat* _value);
		void Set2Fv(std::string _name, GLsizei _count, const GLfloat* _value);
		void Set3Fv(std::string _name, GLsizei _count, const GLfloat* _value);
		void Set4Fv(std::string _name, GLsizei _count, const GLfloat* _value);

		void Set1Iv(std::string _name, GLsizei _count, const GLint* _value);
		void Set2Iv(std::string _name, GLsizei _count, const GLint* _value);
		void Set3Iv(std::string _name, GLsizei _count, const GLint* _value);
		void Set4Iv(std::string _name, GLsizei _count, const GLint* _value);

		void Set1UIv(std::string _name, GLsizei _count, const GLuint* _value);
		void Set2UIv(std::string _name, GLsizei _count, const GLuint* _value);
		void Set3UIv(std::string _name, GLsizei _count, const GLuint* _value);
		void Set4UIv(std::string _name, GLsizei _count, const GLuint* _value);

		void SetMatrix2Fv(std::string _name, GLsizei _count, GLboolean _transpose, const GLfloat* _value);
		void SetMatrix3Fv(std::string _name, GLsizei _count, GLboolean _transpose, const GLfloat* _value);
		void SetMatrix4Fv(std::string _name, GLsizei _count, GLboolean _transpose, const GLfloat* _value);

		void SetMatrix2x3Fv(std::string _name, GLsizei _count, GLboolean _transpose, const GLfloat* _value);
		void SetMatrix3x2Fv(std::string _name, GLsizei _count, GLboolean _transpose, const GLfloat* _value);
		void SetMatrix2x4Fv(std::string _name, GLsizei _count, GLboolean _transpose, const GLfloat* _value);
		void SetMatrix4x2Fv(std::string _name, GLsizei _count, GLboolean _transpose, const GLfloat* _value);
		void SetMatrix3x4Fv(std::string _name, GLsizei _count, GLboolean _transpose, const GLfloat* _value);
		void SetMatrix4x3Fv(std::string _name, GLsizei _count, GLboolean _transpose, const GLfloat* _value);

	};
	class Texture
	{
		GLuint id;
		unsigned int* buffer;
		int w, h;

	public:
		Texture(int _w, int _h, GLuint _filter, GLuint _warp);
		~Texture();

		void TexUnit(Shader& _shader, const char* _uniform, GLuint unit);

		void Bind(GLuint _slot);
		void Unbind();


		void SetPixel(int _x, int _y, uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a);
		unsigned int GetPixel(int _x, int _y);
		void Apply();

		int GetWidth();
		int GetHeight();

	};
	class Mesh
	{
	private:
		//[PRIVATE]
		VAO* vao;
		GLuint tris_count;



	public:
		//[VARIABLES]
		std::string name;

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> colors;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;


		std::vector<GLuint> triangles;


		//[FUNCTIONS]
		Mesh();
		~Mesh();

		void Apply();
		void Bind();


		int GetTriangles();
	};


	//[TYPES]
	class Camera
	{
	public:
		//[VARIABLES]
		glm::vec3 position;
		glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

		float fov;
		float near_plane;
		float far_plane;

		bool is_orthographic;
		float orthographic_size;


		//[FUNCTIONS]
		Camera(glm::vec3 _position, float _fov, float _near, float _far);

		void Rotate(glm::vec3 _angle);
		glm::mat4 Matrix(float _aspect);
	};
	class Surface
	{
	private:
		//[VARIABLES]
		int w, h;

	public:
		GLuint id;
		GLuint color_id;
		GLuint render_id;

		//[FUNCTIONS]
		Surface(int _w, int _h);
		~Surface();

		void Bind();
		void Unbind();

		int GetWidth();
		int GetHeight();
	};
	#pragma endregion




	#pragma region VARIABLES
	extern std::function<void()> on_start;

	extern std::function<void(double _dt)> on_update;
	extern std::function<void(double _dt, int _w, int _h)> on_draw;
	extern std::function<void(double _dt, int _w, int _h, GLuint _output)> on_postdraw;

	extern int target_framerate;

	extern Surface* application_surface;
	#pragma endregion




	#pragma region FUNCTIONS
	//[MAIN FUNCTIONS]
	void Init(int _w, int _h, const char* _title);
	void End(int _code);

	GLFWwindow* GetWindow();
	int GetWindowWidth();
	int GetWindowHeigth();
	void SetWindowSize(int _w, int _h);
	void CenterWindow();
	void SetWindowName(const char* _title);
		


	//RESOURCES FUNCTION
	void ResourcesInit();
	std::shared_ptr<Texture> LoadTexture(std::string _name);
	std::shared_ptr<Mesh> LoadMesh(std::string _name);
	std::shared_ptr<Shader> LoadShader(std::string _name);
	std::vector<char> LoadRaw(std::string _name);
	#pragma endregion
}
#endif // !_DOGINE_H
