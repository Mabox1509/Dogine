#ifndef _STRING_UTILS_H
#define _STRING_UTILS_H
//[INCLUDES]
#include <string>
#include <vector>

//[NAMESPACE]
namespace StringUtils
{
    // Reemplaza todas las ocurrencias de 'from' por 'to' en 'str'
    std::string Replace(const std::string& str, const std::string& from, const std::string& to);

    // Verifica si 'str' contiene 'substr'
    bool Contains(const std::string& str, const std::string& substr);

    // Verifica si 'str' empieza con 'prefix'
    bool StartsWith(const std::string& str, const std::string& prefix);

    // Verifica si 'str' termina con 'suffix'
    bool EndsWith(const std::string& str, const std::string& suffix);

    // Convierte la cadena a minúsculas
    std::string ToLower(const std::string& str);

    // Convierte la cadena a mayúsculas
    std::string ToUpper(const std::string& str);

    // Elimina espacios en ambos extremos
    std::string Trim(const std::string& str);

    // Elimina espacios al inicio
    std::string TrimLeft(const std::string& str);

    // Elimina espacios al final
    std::string TrimRight(const std::string& str);

    // Divide un string por un delimitador
    std::vector<std::string> Split(const std::string& str, char delimiter);

    // Une strings con un separador
    std::string Join(const std::vector<std::string>& elements, const std::string& separator);

    // Verifica si el string representa un número (entero o decimal)
    bool IsNumeric(const std::string& str);
}

#endif // _STRING_UTILS_H
