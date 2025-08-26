#pragma once

//[INCLUDES]
#include <vector>

//[NAMESPACE]
namespace Dogine
{
    namespace Compression
    {
        // Función para comprimir un array de bytes
        std::vector<char> Compress(const std::vector<char>& input);

        // Función para descomprimir un array de bytes
        std::vector<char> Decompress(const std::vector<char>& input);
    }
}