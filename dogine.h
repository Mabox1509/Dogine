#ifndef _DOGINE_H
#define _DOGINE_H
//[INCLUDES]
#include <vector>
#include <string>
#include <memory>
#include <functional>


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
	public:
		//[VARIABLES]
		GLuint shader_id;

		//[CONSTRUCTOR]
		Shader(const char* _vertex, const char* _fragment);
		~Shader();

		//[FUNCTIONS]
		void Activate();
		void Delete();
	};
	class Texture
	{
		unsigned int* buffer;
		int w, h;

	public:
		GLuint id;

		Texture(int _w, int _h, GLenum _slot, GLuint _filter, GLuint _warp);
		~Texture();

		void TexUnit(Shader& _shader, const char* _uniform, GLuint unit);

		void Bind();
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
		glm::mat4 Matrix(glm::mat4 _object, float _aspect);
	};
	#pragma endregion




	#pragma region VARIABLES
	extern std::function<void()> on_start;

	extern std::function<void(double _dt)> on_update;
	extern std::function<void(double _dt)> on_draw;
	extern std::function<void(double _dt)> on_gui;

	extern int target_framerate;
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
	#pragma endregion
}
#endif // !_DOGINE_H
