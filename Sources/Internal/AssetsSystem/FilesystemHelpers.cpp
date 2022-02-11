#include "AssetsSystem/FilesystemHelpers.h"

#include <cassert>
#include <fstream>
#include <sstream>

#include "Core/CoreHelpers.h"

namespace Kioto::FilesystemHelpers
{

std::string GetFileExtension(const std::string& path)
{
    size_t lastPeriod = path.find_last_of('.');
    if (lastPeriod == std::string::npos)
        assert(false);
    return path.substr(lastPeriod);
}

bool CheckIfFileExist(const std::wstring& path)
{
    FILE* file = nullptr;
    if (fopen_s(&file, WstrToStr(path).c_str(), "r") == 0)
    {
        fclose(file);
        return true;
    }
    return false;
}

bool CheckIfFileExist(const std::string& path)
{
    FILE* file = nullptr;
    if (fopen_s(&file, path.c_str(), "r") == 0)
    {
        fclose(file);
        return true;
    }
    return false;
}

std::string ReadFileAsString(const std::string& path)
{
    std::ifstream f(path);
    std::stringstream buffer;
    buffer << f.rdbuf();
    return buffer.str();
}

std::string GetFilenameFromPath(const std::string& path)
{
    return path.substr(path.find_last_of("/\\") + 1);
}

std::wstring GetFilenameFromPath(const std::wstring& path)
{
    return path.substr(path.find_last_of(L"/\\") + 1);
}

}
