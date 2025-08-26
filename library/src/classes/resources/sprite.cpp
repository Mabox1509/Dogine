//[INCLUDES]
#include <dogine/dogine.hpp>
#include <dogine/log.hpp>
#include <cstring>

//[NAMESPACE]
namespace Dogine
{
    //[IMPLEMENTATION]

    // Constructor
    Sprite::Sprite(const std::shared_ptr<Texture>& _texture)
    {
        texture = _texture;
    }

    // Agregar un frame al sprite
    void Sprite::AddFrame(int x, int y, int width, int height,
                          const glm::vec2& pivot, float ppu)
    {
        Mesh mesh;

        // Convertir a unidades del mundo usando ppu
        float w = width / ppu;
        float h = height / ppu;

        // Ajustar pivote (se centra por defecto en 0.5, 0.5)
        float ox = (pivot.x) * w;
        float oy = (pivot.y) * h;

        // Coordenadas en espacio local
        glm::vec3 v0(-ox, -oy, 0.0f);      // abajo-izquierda
        glm::vec3 v1(-ox + w, -oy, 0.0f);  // abajo-derecha
        glm::vec3 v2(-ox + w, -oy + h, 0.0f); // arriba-derecha
        glm::vec3 v3(-ox, -oy + h, 0.0f);  // arriba-izquierda

        mesh.vertices = { v0, v1, v2, v3 };

        // Triángulos (dos)
        mesh.triangles = { 0, 1, 2, 0, 2, 3 };

        // UVs (en base a la textura)
        float tw = static_cast<float>(texture->GetWidth());
        float th = static_cast<float>(texture->GetHeight());

        float u0 = static_cast<float>(x) / tw;
        float v0_uv = static_cast<float>(y) / th;
        float u1 = static_cast<float>(x + width) / tw;
        float v1_uv = static_cast<float>(y + height) / th;

        // Nota: Dependiendo de si tu OpenGL usa UV invertidos (0 arriba o abajo)
        mesh.uvs = {
            { u0, v1_uv }, // abajo-izquierda
            { u1, v1_uv }, // abajo-derecha
            { u1, v0_uv }, // arriba-derecha
            { u0, v0_uv }  // arriba-izquierda
        };

        // Normales (hacia adelante en Z)
        mesh.normals = {
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f}
        };

        // Colores default (blanco)
        mesh.colors = {
            {1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f}
        };

        // Aplicar el mesh (sube datos a GPU, genera VAO/VBO/EBO)
        mesh.Apply();

        // Guardar frame
        frames.push_back(std::move(mesh));
    }

    // Vincular un frame activo (bind)
    void Sprite::BindFrame(int _frame)
    {
        if (_frame < 0 || _frame >= static_cast<int>(frames.size()))
        {
            Log::Warning("Sprite::BindFrame -> Frame inválido (%d)", _frame);
            return;
        }

        frames[_frame].Bind();
    }

    // Obtener referencia a un frame
    const Mesh& Sprite::GetFrameMesh(int _frame) const
    {
        if (_frame < 0 || _frame >= static_cast<int>(frames.size()))
        {
            Log::Error("Sprite::GetFrameMesh -> Frame inválido (%d)", _frame);
            static Mesh dummy;
            return dummy;
        }

        return frames[_frame];
    }
}
