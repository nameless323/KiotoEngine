#pragma once

#include <string>

namespace Kioto::FilesystemHelpers
{
std::string GetFilenameFromPath(const std::string& path);
std::wstring GetFilenameFromPath(const std::wstring& path);
std::string GetFileExtension(const std::string& path); // [a_vorontsov] Propagating everywhere, all users should include AssetsSystem. Maybe move at a better place?
bool CheckIfFileExist(const std::wstring& path);
bool CheckIfFileExist(const std::string& path);
std::string ReadFileAsString(const std::string& path);
}