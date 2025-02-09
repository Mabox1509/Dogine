//[INCLUDES]
#include "../pch.h"
#include "../framework.h"
#include "../dogine.h"


//[NAMESPACE]

namespace Dogine
{
    //[IMPLEMENTATION]
    Mesh::Mesh()
    {
        vao = nullptr;

    }
    Mesh::~Mesh()
    {
        //CLEAN UP
        if (vao != nullptr)
        {
            vao->Delete();
            delete vao;
            vao = nullptr;
        }

    }


    void Mesh::Apply()
    {
        if (triangles.size() % 3 != 0)
        {
            Log::Warning("The triangle array is not a multiple of 3\nVertex count: %d", triangles.size());
            //return;
        }
        tris_count = triangles.size();

        //CLEAN UP
        if (vao != nullptr)
        {
            vao->Delete();
            delete vao;
            vao = nullptr;
        }


        //GENERATE DATA
        size_t _vertex_size = (vertices.size() * 11) * sizeof(float);
        GLfloat* _vertex = (GLfloat*)malloc(_vertex_size);

        memset(_vertex, 0x00, _vertex_size);
        for (int _i = 0; _i < vertices.size(); _i++)
        {
            size_t _seek = _i * 11;


            //WRITE POSITION
            _vertex[_seek] = vertices[_i].x;
            _vertex[_seek + 1] = vertices[_i].y;
            _vertex[_seek + 2] = vertices[_i].z;

            //WRITE NORMALS
            if (_i < normals.size())
            {
                _vertex[_seek + 3] = normals[_i].x;
                _vertex[_seek + 4] = normals[_i].y;
                _vertex[_seek + 5] = normals[_i].z;
            }

            //WRITE COLORS
            if (_i < colors.size())
            {
                _vertex[_seek + 6] = colors[_i].x;
                _vertex[_seek + 7] = colors[_i].y;
                _vertex[_seek + 8] = colors[_i].z;
            }

            //WRITE UVs
            if (_i < uvs.size())
            {
                _vertex[_seek + 9] = uvs[_i].x;
                _vertex[_seek + 10] = uvs[_i].y;
            }


            //Log::Message("Vert %d:\n  %f, %f, %f\n  %f, %f, %f\n  %f, %f, %f\n  %f, %f\n", _i, vertices[_i].x, vertices[_i].y, vertices[_i].z, normals[_i].x, normals[_i].y, normals[_i].z, colors[_i].x, colors[_i].y, colors[_i], uvs[0].x, uvs[1].y);
        }



        size_t _tris_size = triangles.size() * sizeof(GLuint);
        GLuint* _tris = (GLuint*)malloc(_tris_size);
        for (size_t i = 0; i < triangles.size(); i++)
        {
            _tris[i] = triangles[i];
        }



        //SETUP VAO, VBO & EBO
        vao = new VAO();
        vao->Bind();

        VBO vbo(_vertex, _vertex_size);
        EBO ebo(_tris, _tris_size);

        vao->LinkAttrib(vbo, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
        vao->LinkAttrib(vbo, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
        vao->LinkAttrib(vbo, 2, 3, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
        vao->LinkAttrib(vbo, 3, 2, GL_FLOAT, 11 * sizeof(float), (void*)(9 * sizeof(float)));
        vao->Unbind();
        vbo.Unbind();
        ebo.Unbind();



        //CLEAN UP
        free(_vertex);
        free(_tris);
    }
    void Mesh::Bind()
    {
        vao->Bind();
    }

    int Mesh::GetTriangles()
    {
        return tris_count;
    }
}