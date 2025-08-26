#pragma once

//[INCLUDES]
#include <string>
#include <vector>
#include <ctime>


//[NAMESPACE]
namespace Dogine
{
    namespace FileSys 
    {
        //[FILE FUNCTIONS]
        bool FileExists(const std::string& path);
        size_t GetFileSize(const std::string& path);

        std::string ReadString(const std::string& path);
        std::vector<char> ReadBinary(const std::string& path);
        std::vector<char> ReadBinaryPartial(const std::string& path, size_t offset, size_t length);

        void WriteString(const std::string& path, const std::string& content);
        void WriteBinary(const std::string& path, const std::vector<char>& data);

        void DeleteFile(const std::string& path);
        void RenameFile(const std::string& oldPath, const std::string& newPath);
        void CopyFile(const std::string& sourcePath, const std::string& destinationPath);


        //[DIRECTORY FUNCTIONS]
        bool DirectoryExists(const std::string& path);
        std::vector<std::string> ListFiles(const std::string& path, bool recursive);
        void CreateDirectory(const std::string& path);
        void DeleteDirectory(const std::string& path, bool recursive);
        void RenameDirectory(const std::string& oldPath, const std::string& newPath);

        //[FILE METADATA FUNCTIONS]
        std::time_t GetLastWriteTime(const std::string& path);   // Última modificación
        std::time_t GetCreationTime(const std::string& path);    // Creación (solo en Windows, no funciona en Linux ext4)
        std::time_t GetLastAccessTime(const std::string& path);  // Último acceso (depende del sistema de archivos)


    } // namespace Filesys
}