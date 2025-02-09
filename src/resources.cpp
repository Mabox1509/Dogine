//[INCLUDES]
#include "../dogine.h"
#include <sstream>
#include <cerrno>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
#include <unordered_map>
#include <zlib/zlib.h>



//[TYPES]
#define RESOURCES_PATH "resources"
typedef struct file_info_t
{
	size_t position;
	size_t size;
} file_info_t;


//[VARIABLES]
std::unordered_map<std::string, file_info_t> raw_files;

std::map<std::string, std::weak_ptr<Dogine::Texture>> textures;
//std::map<std::string, std::weak_ptr<Audio>> audios;
std::map<std::string, std::weak_ptr<Dogine::Shader>> shaders;
std::map<std::string, std::weak_ptr<Dogine::Mesh>> meshes;

//[FUNCTIONS]
std::vector<char> Decompress(const std::vector<char>& compressedData)
{
	if (compressedData.empty()) {
		return {};
	}

	uLongf decompressedSize = compressedData.size() * 4; // Estimación inicial del tamaño descomprimido
	std::vector<char> decompressedData(decompressedSize);

	int result = uncompress(reinterpret_cast<Bytef*>(decompressedData.data()), &decompressedSize,
		reinterpret_cast<const Bytef*>(compressedData.data()), compressedData.size());

	while (result == Z_BUF_ERROR) {
		// Si el buffer no es suficientemente grande, incrementar el tamaño
		decompressedSize *= 2;
		decompressedData.resize(decompressedSize);
		result = uncompress(reinterpret_cast<Bytef*>(decompressedData.data()), &decompressedSize,
			reinterpret_cast<const Bytef*>(compressedData.data()), compressedData.size());
	}

	if (result != Z_OK) {
		throw std::runtime_error("Decompression failed");
	}

	decompressedData.resize(decompressedSize);
	return decompressedData;
}
std::vector<char> GetFile(std::string _filename)
{
	std::ifstream macroFile(RESOURCES_PATH, std::ios::binary);
	if (!macroFile.is_open())
	{
		Log::Error("Cannot open resources file...");
		exit(1);
	}

	auto _entry = raw_files.find(_filename);
	if (_entry == raw_files.end())
	{
		Log::Error("Invalid asset path");
		exit(1);
	}

	file_info_t _info = _entry->second;
	std::vector<char> fileData(_info.size);

	macroFile.seekg(_info.position, std::ios::beg);
	macroFile.read(fileData.data(), _info.size);
	if (!macroFile) {
		std::cerr << "Error reading the file data" << std::endl;
		exit(1);
	}

	macroFile.close();
	return fileData;
}




//[NAMESPACE]
namespace Dogine
{
	//[FUNCTIONS]
	void ResourcesInit()
	{
		Log::Message("Loading resources...");

		std::ifstream macroFile(RESOURCES_PATH, std::ios::binary);
		if (!macroFile.is_open())
		{
			Log::Error("Cannot open resources file...");
			exit(1);
		}

		unsigned int numEntries = 0;
		macroFile.read(reinterpret_cast<char*>(&numEntries), sizeof(numEntries));
		unsigned char buffer[256];

		for (unsigned int i = 0; i < numEntries; ++i)
		{
			// Leer la ruta del archivo
			macroFile.getline(reinterpret_cast<char*>(buffer), sizeof(buffer), '\0');
			std::string filePath(reinterpret_cast<char*>(buffer));

			// Leer el tamaño del archivo
			size_t fileSize;
			macroFile.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));

			// Guardar la posición actual del archivo en el mapa
			file_info_t info;
			info.position = macroFile.tellg();
			info.size = fileSize;
			raw_files[filePath] = info;

			// Saltar el contenido del archivo
			macroFile.seekg(fileSize, std::ios::cur);

			//Log::Message(filePath);
		}

		macroFile.close();

		std::cout << "\n\n";
	}

	std::shared_ptr<Texture> LoadTexture(std::string _name)
	{
		auto _it = textures.find(_name);
		if (_it != textures.end())
		{
			if (!_it->second.expired())
			{
				return _it->second.lock();
			}
		}


		//LOAD DATA
		std::string _path = "Textures/" + _name + ".txc";
		auto _compress = GetFile(_path);
		auto _decompress = Decompress(_compress);

		// READ HEADER VALUES
		unsigned int _w, _h;
		memcpy(&_w, _decompress.data(), sizeof(int));
		memcpy(&_h, _decompress.data() + sizeof(int), sizeof(int));

		char _format = _decompress[8];
		bool _repeat = _decompress[10];
		bool _interpolation = _decompress[11];

		//DUMP DATA
		auto _texture = std::make_shared<Texture>(_w, _h, GL_TEXTURE0, _interpolation ? GL_LINEAR : GL_NEAREST, _repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
		for (int _y = 0; _y < _h; _y++)
		{
			for (int _x = 0; _x < _w; _x++)
			{
				int _seek = 12 + (((_y * _w) + _x) * _format);

				switch (_format)
				{
				case 4:
					_texture->SetPixel(_x, _y, _decompress[_seek], _decompress[_seek + 1], _decompress[_seek + 2], _decompress[_seek + 3]);
					break;

				case 3:
					_texture->SetPixel(_x, _y, _decompress[_seek], _decompress[_seek + 1], _decompress[_seek + 2], 255);
					break;

				case 2:
					_texture->SetPixel(_x, _y, _decompress[_seek], _decompress[_seek], _decompress[_seek], _decompress[_seek + 1]);
					break;

				case 1:
					_texture->SetPixel(_x, _y, _decompress[_seek], _decompress[_seek], _decompress[_seek], 255);
					break;
				}
			}
		}

		_texture->Apply();


		textures[_name] = _texture;
		return _texture;
	}
	std::shared_ptr<Shader> LoadShader(std::string _name)
	{
		auto _it = shaders.find(_name);
		if (_it != shaders.end())
		{
			if (!_it->second.expired())
			{
				return _it->second.lock();
			}
		}


		std::string _path = "Shaders/" + _name;
		std::vector<char> vertexData = GetFile(_path + "/vertex.bnc");
		std::vector<char> fragmentData = GetFile(_path + "/fragment.bnc");

		// Convierte std::vector<char> a std::string
		std::string _vertex(vertexData.begin(), vertexData.end());
		std::string _fragment(fragmentData.begin(), fragmentData.end());


		auto _shader = std::make_shared<Shader>(_vertex.c_str(), _fragment.c_str());

		shaders[_name] = _shader;

		return _shader;
	}
	std::shared_ptr<Mesh> LoadMesh(std::string _name)
	{
		auto _it = meshes.find(_name);
		if (_it != meshes.end())
		{
			if (!_it->second.expired())
			{
				return _it->second.lock();
			}
		}

		//LOAD RAW DATA
		std::string _path = "Meshes/" + _name + ".mdc";
		std::vector<char> _data = GetFile(_path);
		auto _decompress = Decompress(_data);
		//PrintHex(_decompress);

		//CREATE MESH
		auto _mesh = std::make_shared<Mesh>();
		_mesh->name = _name;


		//INTERPRATATE DATA
		size_t _seek;

		size_t _verts_count = *(size_t*)(_decompress.data());

		_mesh->vertices.resize(_verts_count);
		_mesh->colors.resize(_verts_count);
		_mesh->uvs.resize(_verts_count);
		_mesh->normals.resize(_verts_count);
		for (int _i = 0; _i < _verts_count; _i++)
		{
			_seek = 8 + (_i * 36);

			//READ POSITION (x, y, z) -> (x, z, y)
			float _x = *(float*)(_decompress.data() + _seek); _seek += sizeof(float);
			float _z = *(float*)(_decompress.data() + _seek); _seek += sizeof(float);
			float _y = *(float*)(_decompress.data() + _seek); _seek += sizeof(float);
			_mesh->vertices[_i] = glm::vec3(_x, _y, _z);


			//READ NORMALS (x, y, z) -> (x, z, y)
			float _xn = *(float*)(_decompress.data() + _seek); _seek += sizeof(float);
			float _zn = *(float*)(_decompress.data() + _seek); _seek += sizeof(float);
			float _yn = *(float*)(_decompress.data() + _seek); _seek += sizeof(float);
			_mesh->normals[_i] = glm::normalize(glm::vec3(_xn, _yn, _zn));


			//READ UVs
			float _u = *(float*)(_decompress.data() + _seek); _seek += sizeof(float);
			float _v = *(float*)(_decompress.data() + _seek); _seek += sizeof(float);
			_mesh->uvs[_i] = glm::vec2(_u, _v);

			//READ COLORS
			unsigned char _r = *(_decompress.data() + _seek); _seek++;
			unsigned char _g = *(_decompress.data() + _seek); _seek++;
			unsigned char _b = *(_decompress.data() + _seek); _seek++;
			_mesh->colors[_i] = glm::vec3((_r / 255.0f), (_g / 255.0f), (_b / 255.0f));
		}
		_seek++;


		//std::cout << _seek << std::endl;
		size_t _tris_count = *(size_t*)(_decompress.data() + _seek); _seek += sizeof(size_t);
		_mesh->triangles.resize(_tris_count * 3);
		//std::cout << _tris_count << std::endl;

		for (int _i = 0; _i < _tris_count; _i++)
		{
			uint32_t _a = *(uint32_t*)(_decompress.data() + _seek); _seek += sizeof(uint32_t);
			uint32_t _b = *(uint32_t*)(_decompress.data() + _seek); _seek += sizeof(uint32_t);
			uint32_t _c = *(uint32_t*)(_decompress.data() + _seek); _seek += sizeof(uint32_t);


			_mesh->triangles[(_i * 3)] = _a;
			_mesh->triangles[(_i * 3) + 1] = _b;
			_mesh->triangles[(_i * 3) + 2] = _c;
			//std::cout << _a << ", " << _b << ", " << _c << std::endl;
		}






		//Apply
		_mesh->Apply();

		//Return
		meshes[_name] = _mesh;
		return _mesh;
	}
}