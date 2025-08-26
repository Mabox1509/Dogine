#pragma once
//[INCLUDES]
#include <unordered_map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <functional>
#include <string>
#include <memory>
#include <vector>

//[DEFINES]

//[NAMESPACE]
namespace Dogine
{
    //[TYPES]
    typedef struct color_t
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } color_t;

    //Buffers
	class VBO
    {
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
    class UBO
    {
    public:
        GLuint id;          // ID del buffer
        GLuint bindingPoint; // Binding point en el shader

        // Constructor: crea el UBO con tamaño y binding point
        UBO(GLsizeiptr _size, GLuint _binding);

        // Vincula el UBO
        void Bind();
        // Desvincula el UBO
        void Unbind();

        // Actualiza datos en el UBO
        void Update(GLintptr offset, GLsizeiptr size, const void* data);

        // Elimina el buffer
        void Delete();
    };


    enum class Warpmode : uint8_t
    {
        Repeat,
        Mirror,
        Clamp
    };

    class Shader
    {
    private:
        std::unordered_map<std::string, GLuint> uniforms;
        static Shader* active;

    public:
        //[VARIABLES]
        GLuint shader_id;

        //[CONSTRUCTOR]
        Shader(const char* _vertex, const char* _fragment);
        ~Shader();

        //[FUNCTIONS]
        void Activate();
        void Delete();

        static Shader* GetActive();


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
    private:
        GLuint id;
        color_t* pixels;
        int width, height;

        bool mipmaps;
        bool interpolation;
        Warpmode warp;


    public:
        Texture(int _width, int _height, Warpmode _warp, bool _interpolation, bool _mipmaps);
        ~Texture();

        //void TexUnit(Shader& _shader, const char* _uniform, GLuint unit);

        void UpdateContext();

        void SetPixel(int _x, int _y, color_t _pixel);
        color_t GetPixel(int _x, int _y);
        void Apply();

        int GetWidth();
        int GetHeight();
        GLuint GetID();
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

        std::vector<glm::vec3> perpendicular;


        std::vector<GLuint> triangles;


        //[FUNCTIONS]
        Mesh();
        ~Mesh();

        void Apply();
        void Bind();
        void Unbind();


        int GetTriangles();
    };
    class Audio
    {
    public:
        //[PUBLIC]
        std::string name;   // Nombre opcional (debug, organización)
        double base_freq;

        //[CONSTRUCTOR / DESTRUCTOR]
        Audio(const char* _samples, size_t _size, 
            uint8_t _bitDepth, int _sampleRate, int _channels);
        ~Audio();

        //[GETTERS]
        uint8_t GetBitDepth() const;
        int GetSampleRate() const;
        int GetChannels() const;
        uint64_t GetLengthSamples() const;   // largo en muestras (por canal)
        float GetDuration() const;      // largo en segundos
        uint64_t GetLoopStart() const;

        //[SETTERS]
        void SetLoopPoints(uint64_t _start);

        //[CORE]
        float GetSample(double _position, int _channel) const;  
        // _position en "muestras flotantes", para interpolación

    private:
        //[PRIVATE]
        std::vector<char> samples;   // Buffer crudo (int8_t o int16_t)
        uint8_t bit_depth;               // 8 o 16
        int sample_rate;             // Hz
        int channels;                // 1 = mono, 2 = stereo
        uint64_t loop_start;              // Muestra inicio loop
        uint64_t samples_count;           // Muestra fin loop
    };
    class Sprite
    {
    private:
        std::shared_ptr<Texture> texture;  // Textura asociada al sprite
        std::vector<Mesh> frames;          // Meshes de cada frame

    public:
        //[CONSTRUCTOR]
        Sprite(const std::shared_ptr<Texture>& _texture);

        //[FRAME MANAGEMENT]
        void AddFrame(int x, int y, int width, int height,
                      const glm::vec2& pivot = glm::vec2(0.5f, 0.5f),
                      float ppu = 100.0f); // Pixel per unit

        void BindFrame(int _frame);        // Selecciona frame activo

        //[GETTERS]
        const Mesh& GetFrameMesh(int _frame) const;
        int GetFrameCount() const { return static_cast<int>(frames.size()); }

        std::shared_ptr<Texture> GetTexture() const { return texture; }
    };


    class Surface
    {
    private:
        //[VARIABLES]
        int width, height;
        GLuint fbo;
        GLuint color_id;
        GLuint render_id;

    public:
        

        //[FUNCTIONS]
        Surface(int _width, int _height);
        ~Surface();

        int GetWidth();
        int GetHeight();

        GLuint GetFbo();
        GLuint GetTexture();
        GLuint GetRender();

        void Copy(Surface* _dst, bool _copy_depth, bool _copy_stencil);
        void Save(const std::string& _path);

        glm::mat4 GetOrthoMatrix();
    };


    //[VARIABLES]
    extern std::function<void()> OnStart;
    extern std::function<void(int)> OnEnd;

    extern std::function<void(double)> OnUpdate;
    extern std::function<void()> OnDraw;
    

    //[FUNCTIONS]
    void Startup(int _width, int _height, const std::string& _title, const std::string& _savename);

    void End(int _code);

    std::string GetPersistentPath();

} // namespace Dogine